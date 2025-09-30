#pragma once

#include "../data_structs.h"

#define MAX_ITERATIONS 300
#define PATCH_SIZE 31
#define HALF_PATCH 15
#define CONV_THRESHOLD (1e-4)

vec2 lucasKanade (Img* prevFrame, Img* currFrame, vec2* prevPoint, vec2 estPoint);