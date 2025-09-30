#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <limits.h>

typedef struct Vec2 {
  float x, y;
} vec2;

typedef struct Vec3 {
  float x, y, z;
} vec3;

typedef struct Vec4 {
  float x, y, z, w;
} vec4;

typedef struct Mat2 {
  float m[2][2];
} mat2;

typedef struct Mat3 {
  float m[3][3];
} mat3;

typedef struct Mat4 {
  float m[4][4];
} mat4;

void init_mat2 (mat2* A);
void init_vec2 (vec2* v);

vec2 vec2_add (vec2 a, vec2 b);
vec3 vec3_add (vec3 a, vec3 b);
vec4 vec4_add (vec4 a, vec4 b);
float vec3_norm (vec3 a, vec3 b);

float mat2_det(mat2* M);

vec3 product_mat3_vec3(mat3 mat, vec3 vec);
float mat3_det(mat3 *M);
void mat3_mult(mat3 A, mat3 B, mat3 *R);
void mat3_transpose(mat3 A, mat3 *AT);
mat3 mat3_identity();
mat3 mat3_add(const mat3 A, const mat3 B);
mat3 mat3_scale(const mat3 A, float s);
vec3 apply_rot(const mat3 R, const vec3 v);
float norm3(const vec3 v);
void mat3_copy(mat3 *dst, const mat3 *src);


mat3 compute_inv(const mat3 mat);

void mat3_print(const char* name, mat3 M);
void vec3_print (const char* name, vec3 V);