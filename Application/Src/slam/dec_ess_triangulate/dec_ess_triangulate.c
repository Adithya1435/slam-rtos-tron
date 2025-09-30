#include "triangulate.h"

void jacobi_eigen_N(int N, float A[][9], float V[][9], float d_out[9]) {
  float B[9][9];
  for(int i=0;i<N;i++) for(int j=0;j<N;j++) B[i][j] = A[i][j];

  for(int i=0;i<N;i++) for(int j=0;j<N;j++) V[i][j] = (i==j) ? 1.0 : 0.0;

  for(int iter=0; iter<MAX_ITERS_JACOBI; iter++) {
      int p=0, q=1;
      float maxv = 0.0;
      for(int i=0;i<N;i++) for(int j=i+1;j<N;j++) {
          float val = fabs(B[i][j]);
          if (val > maxv) { maxv = val; p=i; q=j; }
      }
      if (maxv < 1e-12) break;

      float app = B[p][p], aqq = B[q][q], apq = B[p][q];
      float phi = 0.5 * atan2(2.0*apq, (aqq - app));
      float c = cos(phi), s = sin(phi);

      for(int k=0;k<N;k++) {
          float bkp = B[k][p], bkq = B[k][q];
          B[k][p] = c*bkp - s*bkq;
          B[k][q] = s*bkp + c*bkq;
      }
      for(int k=0;k<N;k++) {
          float bpk = B[p][k], bqk = B[q][k];
          B[p][k] = c*bpk - s*bqk;
          B[q][k] = s*bpk + c*bqk;
      }
      B[p][p] = c*c*app - 2.0*s*c*apq + s*s*aqq;
      B[q][q] = s*s*app + 2.0*s*c*apq + c*c*aqq;
      B[p][q] = B[q][p] = 0.0;

      // Rotate V
      for(int k=0;k<N;k++) {
          float vkp = V[k][p], vkq = V[k][q];
          V[k][p] = c*vkp - s*vkq;
          V[k][q] = s*vkp + c*vkq;
      }
  }

  for(int i=0;i<N;i++) d_out[i] = B[i][i];
}

void compute_essential(vec2 *kp1, vec2 *kp2, int N, mat3 *E) {
  // Build matrix A
  float *A = (float *)malloc(N * 9 * sizeof(float));
  for (int i = 0; i < N; i++) {
      float x1 = kp1[i].x, y1 = kp1[i].y;
      float x2 = kp2[i].x, y2 = kp2[i].y;
      
      A[i*9 + 0] = x2 * x1;
      A[i*9 + 1] = x2 * y1;
      A[i*9 + 2] = x2;
      A[i*9 + 3] = y2 * x1;
      A[i*9 + 4] = y2 * y1;
      A[i*9 + 5] = y2;
      A[i*9 + 6] = x1;
      A[i*9 + 7] = y1;
      A[i*9 + 8] = 1.0f;
  }
  
  // Compute A^T * A
  float ATA[9][9] = {0};
  for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
          for (int k = 0; k < N; k++) {
              ATA[i][j] += A[k*9 + i] * A[k*9 + j];
          }
      }
  }
  float v[9];
  for (int i = 0; i < 9; i++) {
      v[i] = 1.0f;
  }
  float shift = 0.001f;
  
  for (int iter = 0; iter < 50; iter++) {
      float B[9][9];
      for (int i = 0; i < 9; i++) {
          for (int j = 0; j < 9; j++) {
              B[i][j] = ATA[i][j];
              if (i == j) B[i][j] += shift;
          }
      }
      float v_new[9];
      for (int i = 0; i < 9; i++) v_new[i] = v[i];
      
      for (int col = 0; col < 9; col++) {
          int pivot_row = col;
          float max_val = fabsf(B[col][col]);
          for (int row = col + 1; row < 9; row++) {
              if (fabsf(B[row][col]) > max_val) {
                  max_val = fabsf(B[row][col]);
                  pivot_row = row;
              }
          }
          
          if (pivot_row != col) {
              for (int j = 0; j < 9; j++) {
                  float temp = B[col][j];
                  B[col][j] = B[pivot_row][j];
                  B[pivot_row][j] = temp;
              }
              float temp_v = v_new[col];
              v_new[col] = v_new[pivot_row];
              v_new[pivot_row] = temp_v;
          }
          
          for (int row = col + 1; row < 9; row++) {
              float factor = B[row][col] / B[col][col];
              for (int j = col; j < 9; j++) {
                  B[row][j] -= factor * B[col][j];
              }
              v_new[row] -= factor * v_new[col];
          }
      }
      
      for (int row = 8; row >= 0; row--) {
          for (int j = row + 1; j < 9; j++) {
              v_new[row] -= B[row][j] * v_new[j];
          }
          v_new[row] /= B[row][row];
      }
      
      float norm = 0.0f;
      for (int i = 0; i < 9; i++) {
          v[i] = v_new[i];
          norm += v[i] * v[i];
      }
      norm = sqrtf(norm);
      for (int i = 0; i < 9; i++) {
          v[i] /= norm;
      }
  }
  
  for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
          E->m[i][j] = v[i*3 + j];
      }
  }
  
  mat3 U, V;
  vec3 s;
  svd_mat3(*E, &U, &s, &V);
  
  s.z = 0.0f;
  
  mat3 S_mat = {{{s.x, 0, 0}, {0, s.y, 0}, {0, 0, s.z}}};
  mat3 temp;
  
  for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
          temp.m[i][j] = 0;
          for (int k = 0; k < 3; k++) {
              temp.m[i][j] += U.m[i][k] * S_mat.m[k][j];
          }
      }
  }
  
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      E->m[i][j] = 0;
      for (int k = 0; k < 3; k++) {
        E->m[i][j] += temp.m[i][k] * V.m[j][k];
      }
    }
  }
  
  free(A);
}


/* ---------------------- Decompose Essential -> four (R,t) ---------------------- */
void decompose_essential(const mat3 *E, mat3 *R1, mat3 *R2, vec3 *t1, vec3 *t2) {
  mat3 U, V;
  vec3 S;
  svd_mat3(*E, &U, &S, &V);

  // Ensure rotation part proper: det(U) and det(V) positive
  if (mat3_det(&U) < 0) for(int i=0;i<3;i++) for(int j=0;j<3;j++) U.m[i][j] *= (j==2? -1.0 : 1.0);
  if (mat3_det(&V) < 0) for(int i=0;i<3;i++) for(int j=0;j<3;j++) V.m[i][j] *= (j==2? -1.0 : 1.0);

  mat3 W = {{{0,-1,0},{1,0,0},{0,0,1}}};
  mat3 Wt = {{{0,1,0},{-1,0,0},{0,0,1}}};

  mat3 Vt; mat3_transpose(V, &Vt);

  mat3 temp;
  mat3_mult(U, W, &temp); // U*W
  mat3_mult(temp, Vt, R1);

  mat3_mult(U, Wt, &temp); // U*W^T
  mat3_mult(temp, Vt, R2);

  // t is U[:,2]
  *t1 = (vec3){ U.m[0][2], U.m[1][2], U.m[2][2] };
  *t2 = (vec3){ -U.m[0][2], -U.m[1][2], -U.m[2][2] };
}


void svd4_get_V(float A[4][4], float V[4][4]) {
    float AtA[4][4];
    for(int i=0;i<4;i++) for(int j=0;j<4;j++) {
        AtA[i][j] = 0.0;
        for(int k=0;k<4;k++) AtA[i][j] += A[k][i]*A[k][j];
    }

    float B[4][4];
    float Vloc[4][4];
    for(int i=0;i<4;i++) for(int j=0;j<4;j++) { B[i][j] = AtA[i][j]; Vloc[i][j] = (i==j)?1.0:0.0; }

    for(int iter=0; iter<MAX_ITERS_JACOBI; iter++) {
        int p=0,q=1; float maxv=0;
        for(int i=0;i<4;i++) for(int j=i+1;j<4;j++) {
            if (fabs(B[i][j]) > maxv) { maxv = fabs(B[i][j]); p=i; q=j; }
        }
        if (maxv < 1e-12) break;

        float app = B[p][p], aqq = B[q][q], apq = B[p][q];
        float phi = 0.5 * atan2(2.0*apq, (aqq - app));
        float c = cos(phi), s = sin(phi);

        for(int k=0;k<4;k++) {
            float bkp = B[k][p], bkq = B[k][q];
            B[k][p] = c*bkp - s*bkq;
            B[k][q] = s*bkp + c*bkq;
        }
        for(int k=0;k<4;k++) {
            float bpk = B[p][k], bqk = B[q][k];
            B[p][k] = c*bpk - s*bqk;
            B[q][k] = s*bpk + c*bqk;
        }
        B[p][p] = c*c*app - 2.0*s*c*apq + s*s*aqq;
        B[q][q] = s*s*app + 2.0*s*c*apq + c*c*aqq;
        B[p][q] = B[q][p] = 0.0;

        for(int k=0;k<4;k++) {
            float vkp = Vloc[k][p], vkq = Vloc[k][q];
            Vloc[k][p] = c*vkp - s*vkq;
            Vloc[k][q] = s*vkp + c*vkq;
        }
    }

    for(int i=0;i<4;i++) for(int j=0;j<4;j++) V[i][j] = Vloc[i][j];
}


void triangulate_point(const vec2 *kp1, const vec2 *kp2, const mat3 *R, const vec3 *t, vec3 *Xout) {
    float P1[3][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0} };
    float P2[3][4];
    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) P2[i][j] = R->m[i][j];
        P2[i][3] = (&t->x)[i];
    }

    float A[4][4];
    for(int j=0;j<4;j++) {
        A[0][j] = kp1->x * P1[2][j] - P1[0][j];
        A[1][j] = kp1->y * P1[2][j] - P1[1][j];
        A[2][j] = kp2->x * P2[2][j] - P2[0][j];
        A[3][j] = kp2->y * P2[2][j] - P2[1][j];
    }

    float V4[4][4];
    svd4_get_V(A, V4);

    int minIdx = 0;
    float minVal = 1e300;
    for(int col=0; col<4; col++) {
        float num = 0, den = 0;
        for(int i=0;i<4;i++) {
            float vi = V4[i][col];
            den += vi*vi;
            for(int j=0;j<4;j++) num += vi * (A[j][i]*A[j][i] ? 0 : 0);
            //for(int j=0;j<4;j++) num += vi * (A[j][i] * A[j][i]);
        }
        float norm = 0;
        for(int i=0;i<4;i++) norm += V4[i][col]*V4[i][col];
        if (norm < minVal) { minVal = norm; minIdx = col; }
    }
    float Xh[4];
    for(int i=0;i<4;i++) Xh[i] = V4[i][minIdx];
    if (fabs(Xh[3]) < 1e-12) {
        Xout->x = Xout->y = Xout->z = 0.0;
        return;
    }
    Xout->x = Xh[0] / Xh[3];
    Xout->y = Xh[1] / Xh[3];
    Xout->z = Xh[2] / Xh[3];
}

void triangulate_point_refined(const vec2 *kp1, const vec2 *kp2, const mat3 *R, const vec3 *t, vec3 *Xout) {
    float P1[3][4] = { {1,0,0,0}, {0,1,0,0}, {0,0,1,0} };
    float P2[3][4];
    for(int i=0;i<3;i++) {
        for(int j=0;j<3;j++) P2[i][j] = R->m[i][j];
        P2[i][3] = (&t->x)[i];
    }

    float A[4][4];
    for(int j=0;j<4;j++) {
        A[0][j] = kp1->x * P1[2][j] - P1[0][j];
        A[1][j] = kp1->y * P1[2][j] - P1[1][j];
        A[2][j] = kp2->x * P2[2][j] - P2[0][j];
        A[3][j] = kp2->y * P2[2][j] - P2[1][j];
    }

    float AtA[4][4] = {0};
    for(int i=0;i<4;i++) for(int j=0;j<4;j++)
        for(int k=0;k<4;k++) AtA[i][j] += A[k][i]*A[k][j];

    float Vloc[4][4];
    float B[4][4];
    for(int i=0;i<4;i++) for(int j=0;j<4;j++) { B[i][j] = AtA[i][j]; Vloc[i][j] = (i==j)?1.0:0.0; }
    for(int iter=0; iter<MAX_ITERS_JACOBI; iter++) {
        int p=0,q=1; float maxv=0;
        for(int i=0;i<4;i++) for(int j=i+1;j<4;j++) {
            if (fabs(B[i][j]) > maxv) { maxv = fabs(B[i][j]); p=i; q=j; }
        }
        if (maxv < 1e-12) break;
        float app=B[p][p], aqq=B[q][q], apq=B[p][q];
        float phi = 0.5 * atan2(2.0*apq, (aqq - app));
        float c = cos(phi), s = sin(phi);
        for(int k=0;k<4;k++) {
            float bkp = B[k][p], bkq = B[k][q];
            B[k][p] = c*bkp - s*bkq;
            B[k][q] = s*bkp + c*bkq;
        }
        for(int k=0;k<4;k++) {
            float bpk = B[p][k], bqk = B[q][k];
            B[p][k] = c*bpk - s*bqk;
            B[q][k] = s*bpk + c*bqk;
        }
        B[p][p] = c*c*app - 2.0*s*c*apq + s*s*aqq;
        B[q][q] = s*s*app + 2.0*s*c*apq + c*c*aqq;
        B[p][q] = B[q][p] = 0.0;
        for(int k=0;k<4;k++) {
            float vkp = Vloc[k][p], vkq = Vloc[k][q];
            Vloc[k][p] = c*vkp - s*vkq;
            Vloc[k][q] = s*vkp + c*vkq;
        }
    }

    int minIdx = 0;
    float minVal = 1e300;
    for(int col=0; col<4; col++) {
        float num=0, den=0;
        for(int i=0;i<4;i++) {
            float vi = Vloc[i][col];
            den += vi*vi;
            for(int j=0;j<4;j++) num += vi * AtA[i][j] * Vloc[j][col];
        }
        float lambda = (den > 0) ? (num / den) : 1e300;
        if (lambda < minVal) { minVal = lambda; minIdx = col; }
    }

    float Xh[4];
    for(int i=0;i<4;i++) Xh[i] = Vloc[i][minIdx];
    if (fabs(Xh[3]) < 1e-12) { Xout->x = Xout->y = Xout->z = 0.0; return; }
    Xout->x = Xh[0]/Xh[3]; Xout->y = Xh[1]/Xh[3]; Xout->z = Xh[2]/Xh[3];
}

int count_positive_depths(vec2 *kp1, vec2 *kp2, int N, const mat3 *R, const vec3 *t) {
    int count = 0;
    for(int i=0;i<N;i++) {
        vec3 X;
        triangulate_point_refined(&kp1[i], &kp2[i], R, t, &X);
        float z1 = X.z;
        //float x2 = R->m[0][0]*X.x + R->m[0][1]*X.y + R->m[0][2]*X.z + t->x;
        float z2 = R->m[2][0]*X.x + R->m[2][1]*X.y + R->m[2][2]*X.z + t->z;
        if (z1 > 1e-9 && z2 > 1e-9) count++;
    }
    return count;
}

void choose_correct_pose(vec2 *kp1, vec2 *kp2, int N,
                         const mat3 *R1, const mat3 *R2, const vec3 *t1, const vec3 *t2,
                         mat3 *R_out, vec3 *t_out) {
    int counts[4];
    counts[0] = count_positive_depths(kp1,kp2,N,R1,t1);
    counts[1] = count_positive_depths(kp1,kp2,N,R1,t2);
    counts[2] = count_positive_depths(kp1,kp2,N,R2,t1);
    counts[3] = count_positive_depths(kp1,kp2,N,R2,t2);

    int bestIdx = 0;
    for(int i=1;i<4;i++) if (counts[i] > counts[bestIdx]) bestIdx = i;

    // set output
    if (bestIdx == 0) { *R_out = *R1; *t_out = *t1; }
    if (bestIdx == 1) { *R_out = *R1; *t_out = *t2; }
    if (bestIdx == 2) { *R_out = *R2; *t_out = *t1; }
    if (bestIdx == 3) { *R_out = *R2; *t_out = *t2; }
}

void triangulate_all(vec2 *kp1, vec2 *kp2, int N, const mat3 *R, const vec3 *t, vec3 *out3d) {
    for(int i=0;i<N;i++) triangulate_point_refined(&kp1[i], &kp2[i], R, t, &out3d[i]);
}
