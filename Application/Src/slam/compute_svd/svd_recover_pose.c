#include "svd.h"
#include "data_structs.h"
#include "math_structs.h"

// Jacobi for SVD
/*
void svd_mat3(mat3 M, mat3 *U, vec3 *S, mat3 *V) {
  for (int i = 0 ; i < 3 ; i++) for (int j = 0 ; j < 3 ; j++) {
      U->m[i][j] = M.m[i][j];
      V->m[i][j] = (i == j ? 1 : 0);
  }

  for (int iter = 0 ; iter < 50 ; iter++) {
    for (int p = 0 ; p < 2 ; p++) {
      for (int q = p+1 ; q < 3 ; q++) {

        float alpha = 0, beta = 0, gamma = 0;
        for (int k = 0 ; k < 3 ; k++) {
          alpha += U->m[k][p]*U->m[k][p];
          beta  += U->m[k][q]*U->m[k][q];
          gamma += U->m[k][p]*U->m[k][q];
        }
        if (fabs(gamma) < 1e-10) continue;
        float zeta = (beta - alpha)/(2 * gamma);
        float t = (zeta >= 0 ? 1.0f : -1.0f)/(fabs(zeta) + sqrt(1+zeta*zeta));
        float c = 1/sqrt(1+t*t);
        float s = c*t;

        for (int k = 0 ; k < 3 ; k++) {
          float up = U->m[k][p], uq = U->m[k][q];
          U->m[k][p] = c*up - s*uq;
          U->m[k][q] = s*up + c*uq;
        }

        for (int k = 0 ; k < 3 ; k++) {
          float vp = V->m[k][p], vq = V->m[k][q];
          V->m[k][p] = c*vp - s*vq;
          V->m[k][q] = s*vp + c*vq;
        }
      }
    }
  }

  float temp_s[3]; 

  for (int j = 0 ; j < 3 ; j++) {
    float norm = 0;
    for (int i = 0 ; i < 3 ; i++) norm += U->m[i][j]*U->m[i][j];
    norm = sqrt(norm);
    temp_s[j] = norm;
    for (int i = 0 ; i < 3 ; i++) U->m[i][j] /= (norm > 1e-12 ? norm : 1);
  }

  S->x = temp_s[0]; S->y = temp_s[1]; S->z = temp_s[2];
}
*/

void svd_mat3(mat3 M, mat3 *U, vec3 *S, mat3 *V) {
    // Initialize U with M, V with identity
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            U->m[i][j] = M.m[i][j];
            V->m[i][j] = (i == j ? 1.0f : 0.0f);
        }
    }

    // Jacobi iterations for SVD
    for (int iter = 0; iter < 50; iter++) {
        int converged = 1;
        
        for (int p = 0; p < 2; p++) {
            for (int q = p + 1; q < 3; q++) {
                // Compute the 2x2 submatrix for this pair
                float a11 = 0, a12 = 0, a22 = 0;
                for (int k = 0; k < 3; k++) {
                    a11 += U->m[k][p] * U->m[k][p];
                    a22 += U->m[k][q] * U->m[k][q];
                    a12 += U->m[k][p] * U->m[k][q];
                }
                
                if (fabs(a12) < 1e-10f) continue;
                converged = 0;
                
                // Compute Jacobi rotation
                float tau = (a22 - a11) / (2.0f * a12);
                float t = (tau >= 0 ? 1.0f : -1.0f) / (fabs(tau) + sqrt(1.0f + tau * tau));
                float c = 1.0f / sqrt(1.0f + t * t);
                float s = c * t;
                
                // Apply rotation to U
                for (int k = 0; k < 3; k++) {
                    float up = U->m[k][p];
                    float uq = U->m[k][q];
                    U->m[k][p] = c * up - s * uq;
                    U->m[k][q] = s * up + c * uq;
                }
                
                // Apply rotation to V
                for (int k = 0; k < 3; k++) {
                    float vp = V->m[k][p];
                    float vq = V->m[k][q];
                    V->m[k][p] = c * vp - s * vq;
                    V->m[k][q] = s * vp + c * vq;
                }
            }
        }
        
        if (converged) break;
    }
    
    // Extract singular values and normalize U columns
    for (int j = 0; j < 3; j++) {
        float norm = 0.0f;
        for (int i = 0; i < 3; i++) {
            norm += U->m[i][j] * U->m[i][j];
        }
        norm = sqrtf(norm);
        
        // Store singular value
        if (j == 0) S->x = norm;
        else if (j == 1) S->y = norm;
        else S->z = norm;
        
        // Normalize U column
        if (norm > 1e-12f) {
            for (int i = 0; i < 3; i++) {
                U->m[i][j] /= norm;
            }
        }
    }
    
    // Sort singular values in descending order
    // This is important for consistent results
    if (S->x < S->y) {
        float temp = S->x; S->x = S->y; S->y = temp;
        // Swap columns in U and V
        for (int i = 0; i < 3; i++) {
            float temp_u = U->m[i][0]; U->m[i][0] = U->m[i][1]; U->m[i][1] = temp_u;
            float temp_v = V->m[i][0]; V->m[i][0] = V->m[i][1]; V->m[i][1] = temp_v;
        }
    }
    if (S->y < S->z) {
        float temp = S->y; S->y = S->z; S->z = temp;
        for (int i = 0; i < 3; i++) {
            float temp_u = U->m[i][1]; U->m[i][1] = U->m[i][2]; U->m[i][2] = temp_u;
            float temp_v = V->m[i][1]; V->m[i][1] = V->m[i][2]; V->m[i][2] = temp_v;
        }
    }
    if (S->x < S->y) {
        float temp = S->x; S->x = S->y; S->y = temp;
        for (int i = 0; i < 3; i++) {
            float temp_u = U->m[i][0]; U->m[i][0] = U->m[i][1]; U->m[i][1] = temp_u;
            float temp_v = V->m[i][0]; V->m[i][0] = V->m[i][1]; V->m[i][1] = temp_v;
        }
    }
}

void rigid_transform(vec3 *A, vec3 *B, int N, mat3 *R, vec3 *t, float* s) {
  vec3 muA = {0,0,0}, muB = {0,0,0};
  for(int i = 0 ; i < N ; i++) {
    muA.x += A[i].x; muA.y += A[i].y; muA.z += A[i].z;
    muB.x += B[i].x; muB.y += B[i].y; muB.z += B[i].z;
  }
  muA.x /= N; muA.y /= N; muA.z /= N;
  muB.x /= N; muB.y /= N; muB.z /= N;

  float PA[N][3], PB[N][3];
  float varA = 0.0f;
  for(int i = 0 ; i < N ; i++) {
    PA[i][0] = A[i].x - muA.x;
    PA[i][1] = A[i].y - muA.y;
    PA[i][2] = A[i].z - muA.z;
    PB[i][0] = B[i].x - muB.x;
    PB[i][1] = B[i].y - muB.y;
    PB[i][2] = B[i].z - muB.z;

    varA += PA[i][0]*PA[i][0] + PA[i][1]*PA[i][1] + PA[i][2]*PA[i][2];
  }

  mat3 H = {0};
  for(int i = 0 ; i < N ; i++) {
    for(int r = 0 ; r < 3 ; r++) for(int c = 0 ; c < 3 ; c++) {
      H.m[r][c] += PA[i][r] * PB[i][c];
    }
  }

  mat3 U, V;
  vec3 S;
  svd_mat3(H,&U,&S,&V);

  float sigma = S.x + S.y + S.z;
  *s = sigma / varA;

  mat3 Ut;
  mat3_transpose(U, &Ut);
  mat3_mult(V, Ut, R);

  if (mat3_det(R) < 0) {
    for(int i = 0 ; i < 3 ; i++) V.m[i][2] *= -1;
    mat3_mult(V,Ut,R);
  }

  float rmu[3], temp[3];

  for(int i = 0 ; i < 3 ; i++) rmu[i] = R->m[i][0]*muA.x + R->m[i][1]*muA.y + R->m[i][2]*muA.z;
  for(int i = 0 ; i < 3 ; i++) temp[i] = (&muB.x)[i]-rmu[i];

  t->x = temp[0]; t->y = temp[1]; t->z = temp[2];
}

void recover_pose_svd(vec3 *kp1, vec3 *kp2, int size, mat3 *R, vec3 *t, float *s) {

  rigid_transform(kp1, kp2, size, R, t, s);

  return;
}
