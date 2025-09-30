#pragma once

#include "data_structs.h"
#include <stdbool.h>

void pose_only_bundle_adjustment(
    const vec3 *points3D,
    const vec2 *points2D,
    int N,
    mat3 *R,
    vec3 *t,
    const mat3 K,
    int max_iters,
    float huber_delta // set to <= 0 to disable Huber (use 0 or negative)
);