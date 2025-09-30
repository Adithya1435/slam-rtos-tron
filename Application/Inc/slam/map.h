#pragma once

#include "data_structs.h"
#include "feature_detect/feature_detect.h"
#include "feature_match/feature_match.h"
#include "triangulate.h"
#include "init_map/init_map.h"

void map_curr_frame (Img* currkf, Pose *currPose, Img* prevkf, Pose* prevPose, mat3 K_MAT, vec3* global_points, int* num);