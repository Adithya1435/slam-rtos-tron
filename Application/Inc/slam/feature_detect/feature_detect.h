#pragma once

#include "../data_structs.h"

#define FAST_THRESHOLD 20
#define FAST_ARC_THRESHOLD 10
#define ENABLE_NMS 1
#define BRIEF_DESCRIPTOR_SIZE 256
#define PATCH_SIZE 31
#define BRIEF_ARRAY_SIZE (BRIEF_DESCRIPTOR_SIZE / 64)

Keypoint* detect_fast_keypoints(Img *image, int *num_keypoints);
BriefDescriptor compute_rbrief_descriptor(Img *image, Keypoint kp, int pattern[][4], float theta);
void generate_test_pattern(int pattern[][4], int size);
Feature* extract_features(Img *img, int *num_features);
int brief_distance(const BriefDescriptor *d1, const BriefDescriptor *d2);
float compute_orientation(Img *image, Keypoint kp, int patch_size);