#include "feature_detect/feature_detect.h"

int pattern[BRIEF_DESCRIPTOR_SIZE][4];
int pattern_initialized = 0;

void initialize_brief_pattern(int pattern[BRIEF_DESCRIPTOR_SIZE][4]) {
  static int initialized = 0;
  if (!initialized) {
    srand(42);
    generate_test_pattern(pattern, BRIEF_DESCRIPTOR_SIZE);
    initialized = 1;
  }
}

Feature* extract_features(Img *img, int *num_features) {
  int pattern[BRIEF_DESCRIPTOR_SIZE][4];
  initialize_brief_pattern(pattern);

  int num_kps = 0;
  Keypoint *keypoints = detect_fast_keypoints(img, &num_kps);
  if (!keypoints || num_kps == 0) {
    *num_features = 0;
    return NULL;
  }

  Feature *features = malloc(num_kps * sizeof(Feature));
  if (!features) {
    free(keypoints);
    *num_features = 0;
    return NULL;
  }

  for (int i = 0; i < num_kps; i++) {
    features[i].keypoint = keypoints[i];
    float theta = compute_orientation(img, keypoints[i], PATCH_SIZE);
    features[i].descriptor = compute_rbrief_descriptor(img, keypoints[i], pattern, theta);
  }

  free(keypoints);
  *num_features = num_kps;
  return features;
}

static inline int hamming_distance(uint64_t a, uint64_t b) {
  return __builtin_popcountll(a ^ b);
}

int brief_distance(const BriefDescriptor *d1, const BriefDescriptor *d2) {
  int distance = 0;
  for (int i = 0; i < BRIEF_ARRAY_SIZE; i++) {
    distance += hamming_distance(d1->descriptor[i], d2->descriptor[i]);
  }
  return distance;
}
