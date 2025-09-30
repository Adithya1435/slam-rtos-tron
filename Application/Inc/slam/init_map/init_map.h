#pragma once

#include "../data_structs.h"
#include "../triangulate.h"

#define SCALING_FACTOR_DEPTH_MODEL (0.02)
#define MAX_HAMMING_DISTANCE 15

//extern Img frame1, frame2;
int init_map(Img *frame1, Img *frame2, const mat3 K_MAT, int *num, vec3 *world_points, int max, Pose* pose, char *getRaw, RawKP *raw_match1, RawKP *raw_match2, vec2 *norm_1, vec2 *norm_2);
void get_raw_points (RawKP *points1, RawKP *points2, int size);