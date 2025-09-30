#include "data_structs.h"
#include "feature_detect/feature_detect.h"

void generate_test_pattern(int pattern[][4], int size) {
  for (int i = 0; i < size; i++) {
    pattern[i][0] = rand() % 31 - 15;
    pattern[i][1] = rand() % 31 - 15;
    pattern[i][2] = rand() % 31 - 15;
    pattern[i][3] = rand() % 31 - 15;
  }
}

float compute_orientation(Img *image, Keypoint kp, int patch_size) {
  int half = patch_size / 2;
  float m10 = 0.0f, m01 = 0.0f;

  for (int dy = -half; dy <= half; dy++) {
    for (int dx = -half; dx <= half; dx++) {
      int x = kp.x + dx;
      int y = kp.y + dy;

      if (x < 0 || x >= image->w || y < 0 || y >= image->h) continue;

      uint8_t I = image->pixels[y * image->w + x];
      m10 += dx * I;
      m01 += dy * I;
    }
}

  return atan2f(m01, m10);
}

BriefDescriptor compute_rbrief_descriptor(Img *image, Keypoint kp, int pattern[][4], float theta) {
  BriefDescriptor desc = {0};
  float cos_theta = cosf(theta);
  float sin_theta = sinf(theta);

  for (int i = 0; i < BRIEF_DESCRIPTOR_SIZE; i++) {
    int dx1 = pattern[i][0], dy1 = pattern[i][1];
    int dx2 = pattern[i][2], dy2 = pattern[i][3];

    int rx1 = (int)roundf(dx1 * cos_theta - dy1 * sin_theta);
    int ry1 = (int)roundf(dx1 * sin_theta + dy1 * cos_theta);
    int rx2 = (int)roundf(dx2 * cos_theta - dy2 * sin_theta);
    int ry2 = (int)roundf(dx2 * sin_theta + dy2 * cos_theta);

    int x1 = kp.x + rx1;
    int y1 = kp.y + ry1;
    int x2 = kp.x + rx2;
    int y2 = kp.y + ry2;

    if (x1 < 0 || x1 >= image->w || y1 < 0 || y1 >= image->h || x2 < 0 || x2 >= image->w || y2 < 0 || y2 >= image->h) continue;

    uint8_t val1 = image->pixels[y1 * image->w + x1];
    uint8_t val2 = image->pixels[y2 * image->w + x2];

    if (val1 < val2) {
      int bit_pos = i % 64;
      int array_pos = i / 64;
      desc.descriptor[array_pos] |= (1ULL << bit_pos);
    }
  }

  return desc;
}

BriefDescriptor compute_keypoint_rbrief(Img *image, Keypoint kp, int pattern[][4], int patch_size) {
  float theta = compute_orientation(image, kp, patch_size);
  return compute_rbrief_descriptor(image, kp, pattern, theta);
}
