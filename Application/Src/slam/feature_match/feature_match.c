#include "data_structs.h"
#include "feature_detect/feature_detect.h"
#include "feature_match/feature_match.h"

#include <stdio.h>

Match* match_features(const Feature *features1, int num_features1, const Feature *features2, int num_features2, int *num_matches, int max_distance) {

  Match *matches = malloc(num_features1 * sizeof(Match));
  if (!matches) {
    *num_matches = 0;
    return NULL;
  }
  *num_matches = 0;

  for (int i = 0; i < num_features1; i++) {
    int best_idx = -1;
    int best_dist = INT_MAX;
    int second_best = INT_MAX;

    for (int j = 0; j < num_features2; j++) {
      int dist = brief_distance(&features1[i].descriptor, &features2[j].descriptor);
      if (dist < best_dist) {
        second_best = best_dist;
        best_dist = dist;
        best_idx = j;
      } else if (dist < second_best) {
        second_best = dist;
      }
    }

    // Lowe's ratio test
    if (best_idx != -1 && best_dist < max_distance && (second_best == INT_MAX || best_dist < 0.8f * second_best)) {

      matches[*num_matches].idx1 = i;
      matches[*num_matches].idx2 = best_idx;
      matches[*num_matches].distance = best_dist;
      matches[*num_matches].confidence = 1.0f - ((float)best_dist / max_distance);
      (*num_matches)++;
    }
  }

  return realloc(matches, *num_matches * sizeof(Match));
}

Correspondence* get_correspondences(const Feature *features1, const Feature *features2, const Match *matches, int num_matches) {
  if (num_matches <= 0) return NULL;

  Correspondence *corrs = malloc(num_matches * sizeof(Correspondence));
  if (!corrs) return NULL;

  for (int i = 0; i < num_matches; i++) {
    int idx1 = matches[i].idx1;
    int idx2 = matches[i].idx2;

    corrs[i].x1 = features1[idx1].keypoint.x;
    corrs[i].y1 = features1[idx1].keypoint.y;
    corrs[i].x2 = features2[idx2].keypoint.x;
    corrs[i].y2 = features2[idx2].keypoint.y;
    corrs[i].confidence = matches[i].confidence;
  }

  return corrs;
}
