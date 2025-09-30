#pragma once

#include "../data_structs.h"

Match* match_features(const Feature *features1, int num_features1, const Feature *features2, int num_features2, int *num_matches, int max_distance);
Correspondence* get_correspondences(const Feature *features1, const Feature *features2, const Match *matches, int num_matches);