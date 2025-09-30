#include "math_structs.h"
#include "feature_detect/feature_detect.h"
#include "feature_match/feature_match.h"
#include "init_map/init_map.h"

#include <string.h>

int num_matches;
mat3 K_MAT_INV;

RawKP *match1, *match2;
vec2 *norm1, *norm2;

void save_vec3(const char *filename, vec3 *points, int count) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror("Failed to open file");
    return;
  }

  size_t written = fwrite(points, sizeof(vec3), count, fp);
  if (written != (size_t)count) {
    perror("Failed to write all points");
  }

  fclose(fp);
}

Correspondence* match_and_correspond (int num1, int num2, Feature *features_1, Feature* features_2, int *num_matches) {

  Match *matches = match_features(features_1, num1, features_2, num2, num_matches, MAX_HAMMING_DISTANCE);

  Correspondence *correspondences = get_correspondences(features_1, features_2, matches, *num_matches);

  return correspondences;
}

void normalize_pixel_to_vec (RawKP *kp1, RawKP *kp2, vec2 *kp1_norm, vec2 *kp2_norm, int size) {
  int i = 0;
  while (i < size) {
    vec3 curr_pixel1 = {kp1[i].x, kp1[i].y, 1.0f};
    vec3 dir1 = product_mat3_vec3(K_MAT_INV, curr_pixel1);
    kp1_norm[i].x = dir1.x/dir1.z;
    kp1_norm[i].y = dir1.y/dir1.z;

    //printf("Norm = %e\n", (kp1_norm[i].x*kp1_norm[i].x + kp1_norm[i].y*kp1_norm[i].y));

    vec3 curr_pixel2 = {kp2[i].x, kp2[i].y, 1.0f};
    vec3 dir2 = product_mat3_vec3(K_MAT_INV, curr_pixel2);
    kp2_norm[i].x = dir2.x/dir2.z;
    kp2_norm[i].y = dir2.y/dir2.z;

    i++;
  }
}

float get_depth_from_map (int u, int v, uint8_t *depth) {
  return (float)depth[v * WIDTH + u];
}

void extract_depth_from_npu (RawKP *kp1, uint8_t *depth1, float *kp1_depth, RawKP *kp2, uint8_t *depth2, float *kp2_depth, int size) {
  for (int i = 0 ; i < size ; i++) {
    float temp = get_depth_from_map(kp1[i].x, kp1[i].y, depth1);
    if (fabs(temp) < 1e-4) temp = 1e-4;
    kp1_depth[i] = (255 - (0.8)*temp) * SCALING_FACTOR_DEPTH_MODEL;
    
    temp = get_depth_from_map(kp2[i].x, kp2[i].y, depth2);
    if (fabs(temp) < 1e-4) temp = 1e-4;
    kp2_depth[i] = (255 - (0.8)*temp) * SCALING_FACTOR_DEPTH_MODEL;

  }

  return;
}

void pack_to_point3 (vec2 *kp1_norm, float *kp1_depth, vec3 *kp1_cam1, vec2 *kp2_norm, float* kp2_depth, vec3 *kp2_cam2, int size) {
  for (int i = 0 ; i < size ; i++) {
    kp1_cam1[i].x = kp1_norm[i].x*kp1_depth[i]; kp1_cam1[i].y = kp1_norm[i].y*kp1_depth[i];
    //kp1_cam1[i].x = kp1_norm[i].x; kp1_cam1[i].y = kp1_norm[i].y;
    kp1_cam1[i].z = kp1_depth[i];

    kp2_cam2[i].x = kp2_norm[i].x*kp2_depth[i]; kp2_cam2[i].y = kp2_norm[i].y*kp2_depth[i];
    kp2_cam2[i].z = kp2_depth[i];
  }

  return;
}

void get_raw_points (RawKP *points1, RawKP *points2, int size) {
  for (int i = 0 ; i < size ; i++) {
    if (i == num_matches) break;

    points1[i] = match1[i];
    points2[i] = match2[i];
  }

  return;
}

void get_norm_points (vec2 *p1, vec2 *p2, int size) {
  for (int i = 0 ; i < size ; i++) {
    if (i == num_matches) break;

    p1[i] = norm1[i];
    p2[i] = norm2[i];
  }

  return;
}

int init_map(Img *frame1, Img *frame2, const mat3 K_MAT, int *num, vec3 *world_points, int max, Pose* pose, char *getRaw, RawKP *raw_match1, RawKP *raw_match2, vec2 *norm_1, vec2 *norm_2) {

  //if (load_frames(frame_1, &frame1, frame_2, &frame2)) return 1;

  K_MAT_INV = compute_inv(K_MAT);

  int num_features1, num_features2;
  Feature *features_1 = extract_features(frame1, &num_features1);
  Feature *features_2 = extract_features(frame2, &num_features2);

  Correspondence* correspondences = match_and_correspond(num_features1, num_features2, features_1, features_2, &num_matches);

  *num = num_matches;
  
  RawKP kp1_raw_match[num_matches], kp2_raw_match[num_matches];

  for (int i = 0 ; i < num_matches ; i++) {
    kp1_raw_match[i].x = correspondences->x1;
    kp1_raw_match[i].y = correspondences->y1;

    kp2_raw_match[i].x = correspondences->x2;
    kp2_raw_match[i].y = correspondences->y2;

    correspondences++;
  }

  match1 = kp1_raw_match;
  match2 = kp2_raw_match;

  if (strcmp(getRaw, "yes") == 0) get_raw_points(raw_match1, raw_match2, max);


  vec2 kp1_norm[num_matches], kp2_norm[num_matches];
  //float kp1_depth[num_matches], kp2_depth[num_matches];

  //vec3 kp1_cam1[num_matches], kp2_cam2[num_matches];

  normalize_pixel_to_vec(kp1_raw_match, kp2_raw_match, kp1_norm, kp2_norm, num_matches);

  norm1 = kp1_norm;
  norm2 = kp2_norm;

  //get_norm_points(norm_1, norm_2, max);

  mat3 E;
  compute_essential(kp1_norm, kp2_norm, num_matches, &E);
  //mat3_print("E", E);

  mat3 R1, R2; vec3 t1, t2;
  decompose_essential(&E, &R1, &R2, &t1, &t2);
  choose_correct_pose(kp1_norm, kp2_norm, num_matches, &R1, &R2, &t1, &t2, &pose->R, &pose->t);
  
  //mat3_print("R est", R_est);
  //vec3_print("t est", t_est);
  //float norm_t = sqrt(pose->t.x*pose->t.x + pose->t.y*pose->t.y + pose->t.z*pose->t.z);
  //float baseline = 1.0;
  //vec3 t_scaled = { pose->t.x * baseline / norm_t , pose->t.y * baseline / norm_t, pose->t.z * baseline / norm_t };
  //pose->t = t_scaled;

  triangulate_all(kp1_norm, kp2_norm, num_matches, &pose->R, &pose->t, world_points);
  pose->s = 1.0f;

  /*

  extract_depth_from_npu(kp1_raw_match, depth1, kp1_depth, kp2_raw_match, depth2, kp2_depth, num_matches);

  pack_to_point3(kp1_norm, kp1_depth, kp1_cam1, kp2_norm, kp2_depth, kp2_cam2, num_matches);

  for (int i = 0 ; i < max ; i++) {
    if (i == num_matches) break;
    world_points[i] = kp1_cam1[i];
  }

  save_vec3("bin/x1.raw", kp1_cam1, num_matches);
  save_vec3("bin/x2.raw", kp2_cam2, num_matches);

  recover_pose_svd(kp2_cam2, kp1_cam1, num_matches, &pose->R, &pose->t, &pose->s);

  int check = 3;

  printf("---SVD CHECK---\n");
  vec3 rot = product_mat3_vec3(pose->R, kp1_cam1[check]);
  vec3 temp = vec3_add(rot, pose->t);
  vec3_print("est", temp);
  vec3_print("actual", kp2_cam2[check]);
  puts("---------");
  */

  return 0;
}
