#pragma once

#include "data_structs.h"

void recover_pose_svd(vec3 *kp1, vec3 *kp2, int size, mat3 *R, vec3 *t, float *s);
void svd_mat3(mat3 M, mat3 *U, vec3 *S, mat3 *V);