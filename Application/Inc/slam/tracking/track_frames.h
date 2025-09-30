#pragma once

#include "lucasKanade.h"
#include "pose_only_ba.h"

#define TRANSLATION_THRESHOLD 0.2

int tracking_thread (vec3* worldPoints, const mat3 K_MAT, Img* currFrame, Img* recentFrame, Pose* recentPose, int numPoints, uint8_t* status);

extern Pose currOptimalPose;