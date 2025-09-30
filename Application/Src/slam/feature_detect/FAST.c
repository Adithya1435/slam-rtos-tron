#include "data_structs.h"
#include "feature_detect/feature_detect.h"

float is_fast_corner(Img *image, int x, int y, int threshold) {
  if (x < 3 || y < 3 || x >= (image->w - 3) || y >= (image->h - 3)) return 0.0f;
  
  uint8_t center = image->pixels[y * image->w + x];
  uint8_t vals[16];
  
  int circle[16][2] = {
    {0, -3}, {1, -3}, {2, -2}, {3, -1},
    {3, 0}, {3, 1}, {2, 2}, {1, 3},
    {0, 3}, {-1, 3}, {-2, 2}, {-3, 1},
    {-3, 0}, {-3, -1}, {-2, -2}, {-1, -3}
  };

  int cmp[16];
  for (int i = 0; i < 16; i++) {
    int dx = circle[i][0], dy = circle[i][1];
    uint8_t pixel = image->pixels[(y + dy) * image->w + (x + dx)];
    vals[i] = pixel;
    if (pixel > center + threshold)
      cmp[i] = 1;
    else if (pixel < center - threshold)
      cmp[i] = -1;
    else
      cmp[i] = 0;
  }

  for (int start = 0; start < 16; start++) {
    int brighter_run = 0, darker_run = 0;
    int min_diff = 255;

    for (int k = 0; k < 16 + FAST_ARC_THRESHOLD; k++) {
      int idx = (start + k) % 16;

      if (cmp[idx] == 1) {
        brighter_run++;
        darker_run = 0;
        int diff = vals[idx] - center;
        if (diff < min_diff) min_diff = diff;
      } else if (cmp[idx] == -1) {
        darker_run++;
        brighter_run = 0;
        int diff = center - vals[idx];
        if (diff < min_diff) min_diff = diff;
      } else {
        brighter_run = darker_run = 0;
        min_diff = 255;
      }

      if (brighter_run >= FAST_ARC_THRESHOLD || darker_run >= FAST_ARC_THRESHOLD) return (float)min_diff;
    }
  }

  return 0.0f;
}

Keypoint* detect_fast_keypoints(Img *image, int *num_keypoints) {
  Keypoint *all_kp = malloc(image->w * image->h * sizeof(Keypoint));
  *num_keypoints = 0;
  
  for (int y = 3; y < image->h - 3; y++) {
    for (int x = 3; x < image->w - 3; x++) {
      float resp = is_fast_corner(image, x, y, FAST_THRESHOLD);
      if (resp > 0.0f) {
        all_kp[*num_keypoints].x = x;
        all_kp[*num_keypoints].y = y;
        all_kp[*num_keypoints].response = resp;
        (*num_keypoints)++;
      }
    }
  }

  if (ENABLE_NMS) {
    Keypoint *nms_kp = malloc(*num_keypoints * sizeof(Keypoint));
    int nms_count = 0;

    for (int i = 0; i < *num_keypoints; i++) {
      int x = all_kp[i].x;
      int y = all_kp[i].y;
      float resp = all_kp[i].response;
      int is_max = 1;

      for (int j = 0; j < *num_keypoints; j++) {
        if (i == j) continue;
        if (abs(all_kp[j].x - x) <= 1 && abs(all_kp[j].y - y) <= 1) {
          if (all_kp[j].response > resp) {
            is_max = 0;
            break;
          }
        }
      }

      if (is_max) nms_kp[nms_count++] = all_kp[i];
    }

    free(all_kp);
    *num_keypoints = nms_count;

    return realloc(nms_kp, nms_count * sizeof(Keypoint)); 
  }

  else return realloc(all_kp, *num_keypoints * sizeof(Keypoint));

}
