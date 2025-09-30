#include "tracking/pose_only_ba.h"

static void skew3(const vec3 v, float S[3][3]) {
  S[0][0] = 0.0f;    S[0][1] = -v.z;  S[0][2] = v.y;
  S[1][0] = v.z;    S[1][1] = 0.0f;   S[1][2] = -v.x;
  S[2][0] = -v.y;   S[2][1] = v.x;    S[2][2] = 0.0f;
}

static mat3 so3_exp(const vec3 phi) {
  float theta = norm3(phi);
  mat3 R;
  if (theta < 1e-8f) {
    float S[3][3]; skew3(phi, S);
    mat3 I = mat3_identity();
    for (int i=0;i<3;i++) for (int j=0;j<3;j++) R.m[i][j] = I.m[i][j] + S[i][j];
    return R;
  }
  vec3 n = { phi.x/theta, phi.y/theta, phi.z/theta };
  float nx = n.x, ny = n.y, nz = n.z;
  float S[3][3]; skew3(n, S);

  float cos_t = cosf(theta), sin_t = sinf(theta);
  mat3 I = mat3_identity();
  mat3 nn; 
  nn.m[0][0] = nx*nx; nn.m[0][1] = nx*ny; nn.m[0][2] = nx*nz;
  nn.m[1][0] = ny*nx; nn.m[1][1] = ny*ny; nn.m[1][2] = ny*nz;
  nn.m[2][0] = nz*nx; nn.m[2][1] = nz*ny; nn.m[2][2] = nz*nz;

  mat3 term1 = mat3_scale(I, cos_t);
  mat3 term2 = mat3_scale(nn, (1.0f - cos_t));
  mat3 term3;
  for (int i=0;i<3;i++) for (int j=0;j<3;j++) term3.m[i][j] = S[i][j] * sin_t;

  R = mat3_add(mat3_add(term1, term2), term3);
  return R;
}

static mat3 compute_V_matrix(const vec3 phi) {
  float theta = norm3(phi);
  mat3 I = mat3_identity();
  float S_raw[3][3]; skew3(phi, S_raw);
  mat3 S;
  for(int i=0;i<3;i++) for(int j=0;j<3;j++) S.m[i][j] = S_raw[i][j];
  mat3 S2;
  mat3_mult(S, S, &S2);

  if (theta < 1e-8f) {
    mat3 term1 = mat3_scale(S, 0.5f);
    mat3 term2 = mat3_scale(S2, 1.0f/6.0f);
    return mat3_add(mat3_add(I, term1), term2);
  }

  float theta2 = theta*theta;
  float sin_t = sinf(theta);

  float a = (1.0f - cosf(theta)) / theta2;
  float b = (theta - sin_t) / (theta2 * theta);

  mat3 termA = mat3_scale(S, a);
  mat3 termB = mat3_scale(S2, b);
  return mat3_add(mat3_add(I, termA), termB);
}

static void se3_exp(const float xi[6], mat3 *R_delta_out, vec3 *trans_out) {
  vec3 rho = { xi[0], xi[1], xi[2] };
  vec3 phi = { xi[3], xi[4], xi[5] };
  mat3 R_delta = so3_exp(phi);
  mat3 V = compute_V_matrix(phi);
  vec3 trans = product_mat3_vec3(V, rho);
  mat3_copy(R_delta_out, &R_delta);
  trans_out->x = trans.x; trans_out->y = trans.y; trans_out->z = trans.z;
}

static void update_pose_left(mat3 *R, vec3 *t, const float xi[6]) {
  mat3 R_delta; vec3 trans;
  se3_exp(xi, &R_delta, &trans);
  mat3 Rnew;
  mat3_mult(R_delta, *R, &Rnew);
  vec3 Rt = product_mat3_vec3(R_delta, *t);
  vec3 tnew = { Rt.x + trans.x, Rt.y + trans.y, Rt.z + trans.z };
  mat3_copy(R, &Rnew);
  t->x = tnew.x; t->y = tnew.y; t->z = tnew.z;
}

static bool solve_6x6(const float A_in[6][6], const float b_in[6], float x_out[6]) {
  double M[6][7];
  for (int i=0;i<6;i++) {
    for (int j=0;j<6;j++) M[i][j] = (double)A_in[i][j];
    M[i][6] = (double)b_in[i];
  }

  for (int col = 0; col < 6; col++) {
    int pivot = col;
    double maxv = fabs(M[col][col]);
    for (int r = col+1; r < 6; r++) {
      double v = fabs(M[r][col]);
      if (v > maxv) { maxv = v; pivot = r; }
    }
    if (maxv < 1e-12) return false;

    if (pivot != col) {
      for (int c = col; c < 7; c++) { double tmp = M[col][c]; M[col][c] = M[pivot][c]; M[pivot][c] = tmp; }
    }

    double diag = M[col][col];
    for (int c = col; c < 7; c++) M[col][c] /= diag;
    for (int r = 0; r < 6; r++) {
      if (r == col) continue;
      double factor = M[r][col];
      if (factor == 0.0) continue;
      for (int c = col; c < 7; c++) M[r][c] -= factor * M[col][c];
    }
  }

  for (int i=0;i<6;i++) x_out[i] = (float)M[i][6];
  return true;
}

static float huber_weight_scalar(float err2, float huber_delta) {
  if (err2 <= huber_delta*huber_delta) return 1.0f;
  float err = sqrtf(err2);
  return (huber_delta / err);
}

void pose_only_bundle_adjustment(
  const vec3 *points3D,
  const vec2 *points2D,
  int N,
  mat3 *R,
  vec3 *t,
  const mat3 K,
  int max_iters,
  float huber_delta
) {
  if (N <= 0) return;

  const float eps_dx2 = 1e-12f; // convergence on dx^2
  for (int iter = 0; iter < max_iters; ++iter) {
    float H[6][6];
    float b[6];
    for (int i=0;i<6;i++) { b[i]=0.0f; for (int j=0;j<6;j++) H[i][j]=0.0f; }
    double total_error = 0.0;

    float fx = K.m[0][0];
    float fy = K.m[1][1];

    for (int i = 0; i < N; ++i) {
      vec3 Pc = product_mat3_vec3(*R, points3D[i]);
      Pc.x += t->x; Pc.y += t->y; Pc.z += t->z;

      if (Pc.z <= 1e-6f) {
        continue;
      }

      float Xc = Pc.x, Yc = Pc.y, Zc = Pc.z;
      float invZ = 1.0f / Zc;
      float invZ2 = invZ * invZ;

      float u_proj = fx * Xc * invZ + K.m[0][2];
      float v_proj = fy * Yc * invZ + K.m[1][2];

      float rx = points2D[i].x - u_proj;
      float ry = points2D[i].y - v_proj;

      float err2 = rx*rx + ry*ry;
      total_error += err2;

      float Jpi[2][3];
      Jpi[0][0] = fx * invZ;
      Jpi[0][1] = 0.0f;
      Jpi[0][2] = -fx * Xc * invZ2;
      Jpi[1][0] = 0.0f;
      Jpi[1][1] = fy * invZ;
      Jpi[1][2] = -fy * Yc * invZ2;

      float skewP[3][3];
      skew3(Pc, skewP);
      float negSkewP[3][3];
      for (int r2=0;r2<3;r2++) for (int c2=0;c2<3;c2++) negSkewP[r2][c2] = -skewP[r2][c2];

      float J[2][6];
      float A[2][3];
      for (int r2=0;r2<2;r2++) for (int c2=0;c2<3;c2++) {
        A[r2][c2] = Jpi[r2][c2];
      }
      float B[2][3];
      for (int r2=0;r2<2;r2++) {
        for (int c2=0;c2<3;c2++) {
          float s = 0.0f;
          for (int k=0;k<3;k++) s += Jpi[r2][k] * negSkewP[k][c2];
          B[r2][c2] = s;
        }
      }
      for (int c2=0;c2<3;c2++) { J[0][c2] = -A[0][c2]; J[1][c2] = -A[1][c2]; }
      for (int c2=0;c2<3;c2++) { J[0][3+c2] = -B[0][c2]; J[1][3+c2] = -B[1][c2]; }

      float w = 1.0f;
      if (huber_delta > 0.0f) w = huber_weight_scalar(err2, huber_delta);

      for (int ii = 0; ii < 6; ii++) {
        float Jtr = J[0][ii] * rx + J[1][ii] * ry;
        b[ii] += w * Jtr;
        for (int jj = 0; jj < 6; jj++) {
          float val = J[0][ii]*J[0][jj] + J[1][ii]*J[1][jj];
          H[ii][jj] += w * val;
        }
      }
    }

    bool H_all_zero = true;
    for (int i=0;i<6 && H_all_zero;i++) for (int j=0;j<6;j++) if (fabsf(H[i][j]) > 1e-18f) { H_all_zero = false; break; }
    if (H_all_zero) break;

    float bneg[6];
    for (int i=0;i<6;i++) bneg[i] = -b[i];

    float dx[6];
    bool solved = solve_6x6(H, bneg, dx);
    if (!solved) {
        break;
    }

    double dx2 = 0.0;
    for (int i=0;i<6;i++) dx2 += (double)dx[i]*dx[i];
    if (dx2 < eps_dx2) {
      break;
    }

    update_pose_left(R, t, dx);

    }
}
