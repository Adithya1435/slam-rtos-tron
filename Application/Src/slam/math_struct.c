#include "math_structs.h"

mat3 compute_inv(const mat3 mat) {
  float a = mat.m[0][0], b = mat.m[0][1], c = mat.m[0][2];
  float d = mat.m[1][0], e = mat.m[1][1], f = mat.m[1][2];
  float g = mat.m[2][0], h = mat.m[2][1], i = mat.m[2][2];
  
  mat3 matOut;

  float det = a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
  if (det == 0.0f) return matOut;

  float invDet = 1.0f / det;

  matOut.m[0][0] =  (e*i - f*h) * invDet;
  matOut.m[0][1] = -(b*i - c*h) * invDet;
  matOut.m[0][2] =  (b*f - c*e) * invDet;

  matOut.m[1][0] = -(d*i - f*g) * invDet;
  matOut.m[1][1] =  (a*i - c*g) * invDet;
  matOut.m[1][2] = -(a*f - c*d) * invDet;

  matOut.m[2][0] =  (d*h - e*g) * invDet;
  matOut.m[2][1] = -(a*h - b*g) * invDet;
  matOut.m[2][2] =  (a*e - b*d) * invDet;

  return matOut;
}

vec3 product_mat3_vec3(mat3 mat, vec3 vec) {
  vec3 out;
  out.x = (mat.m[0][0] * vec.x) + (mat.m[0][1] * vec.y) + (mat.m[0][2] * vec.z);
  out.y = (mat.m[1][0] * vec.x) + (mat.m[1][1] * vec.y) + (mat.m[1][2] * vec.z);
  out.z = (mat.m[2][0] * vec.x) + (mat.m[2][1] * vec.y) + (mat.m[2][2] * vec.z);

  return out;
}

void init_mat2 (mat2* A) {
  A->m[0][0] = 0.0f;
  A->m[1][0] = 0.0f;
  A->m[0][1] = 0.0f;
  A->m[1][1] = 0.0f;
}

void init_vec2 (vec2* v) {
  v->x = 0.0f;
  v->y = 0.0f;
}

float mat3_det (mat3 *M) {
  return M->m[0][0]*(M->m[1][1]*M->m[2][2] - M->m[1][2]*M->m[2][1]) - M->m[0][1]*(M->m[1][0]*M->m[2][2]-M->m[1][2]*M->m[2][0]) + M->m[0][2]*(M->m[1][0]*M->m[2][1]-M->m[1][1]*M->m[2][0]);
}

float mat2_det (mat2* M) {
  return (M->m[0][0]*M->m[1][1] - M->m[0][1]*M->m[1][0]);
}

void mat3_mult (mat3 A, mat3 B, mat3 *R) {
  for(int i = 0 ; i < 3 ; i++) for(int j = 0 ; j < 3 ; j++) {
    R->m[i][j]=0;
    for(int k = 0 ; k < 3 ; k++) R->m[i][j] += A.m[i][k]*B.m[k][j];
  }
}

void mat3_transpose (mat3 A, mat3 *AT) {
  for(int i = 0 ; i < 3 ; i++) for(int j = 0 ; j < 3 ; j++) AT->m[j][i] = A.m[i][j];
}

mat3 mat3_identity() {
  mat3 I; memset(I.m, 0, sizeof(I.m));
  I.m[0][0]=I.m[1][1]=I.m[2][2]=1.0f;
  return I;
}

mat3 mat3_add(const mat3 A, const mat3 B) {
  mat3 R;
  for (int i=0;i<3;i++) for (int j=0;j<3;j++) R.m[i][j]=A.m[i][j]+B.m[i][j];
  return R;
}

mat3 mat3_scale(const mat3 A, float s) {
  mat3 R;
  for (int i=0;i<3;i++) for (int j=0;j<3;j++) R.m[i][j]=A.m[i][j]*s;
  return R;
}

vec3 apply_rot(const mat3 R, const vec3 v) { return product_mat3_vec3(R, v); }

float norm3(const vec3 v) { return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z); }

void mat3_copy(mat3 *dst, const mat3 *src) { memcpy(dst->m, src->m, sizeof(dst->m)); }

vec3 vec3_add (vec3 a, vec3 b) {
  return (vec3){(a.x + b.x), (a.y + b.y), (a.z + b.z)};
}

float vec3_norm (vec3 a, vec3 b) {
  float x_n = (a.x - b.x)*(a.x - b.x);
  float y_n = (a.y - b.y)*(a.y - b.y);
  float z_n = (a.z - b.z)*(a.z - b.z);

  return (x_n + y_n + z_n);
}

void mat3_print (const char* name, mat3 M) {
  printf("%s:\n", name);
  for (int i = 0 ; i < 3 ; i++) printf("  % .6f % .6f % .6f\n", M.m[i][0], M.m[i][1], M.m[i][2]);

  return;
}

void vec3_print (const char* name, vec3 V) {
  printf("%s: [%.6f, %.6f, %.6f]\n", name, V.x, V.y, V.z);

  return;
}
