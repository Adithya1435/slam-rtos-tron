#pragma once

#include "data_structs.h"
#include "svd.h"

#define MAX_ITERS_JACOBI 100
#define EPS 1e-12

void compute_essential(vec2 *kp1, vec2 *kp2, int N, mat3 *E);
void decompose_essential(const mat3 *E, mat3 *R1, mat3 *R2, vec3 *t1, vec3 *t2);
void choose_correct_pose(vec2 *kp1, vec2 *kp2, int N, const mat3 *R1, const mat3 *R2, const vec3 *t1, const vec3 *t2, mat3 *R_out, vec3 *t_out);
void triangulate_all(vec2 *kp1, vec2 *kp2, int N, const mat3 *R, const vec3 *t, vec3 *out3d);