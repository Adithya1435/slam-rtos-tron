#include "map.h"

mat3 K_MAT_INV1;

void normalize_to_dir (RawKP* kp, vec2* kp_norm, int size) {
  for (int i = 0; i < size; i++) {
    vec3 curr_pixel1 = {kp[i].x, kp[i].y, 1.0f};
    vec3 dir1 = product_mat3_vec3(K_MAT_INV1, curr_pixel1);

    kp_norm[i].x = dir1.x / dir1.z;
    kp_norm[i].y = dir1.y / dir1.z;
  }
}

void project_to_world (vec3* cam, Pose* pose, vec3* world, int size) {
  mat3 R_INV = compute_inv(pose->R);
  
  for (int i = 0; i < size; i++) {
    vec3 rot;
    rot.x = cam->x - pose->t.x;
    rot.y = cam->y - pose->t.y;
    rot.z = cam->z - pose->t.z;

    world[i] = product_mat3_vec3(R_INV, rot); 
  }
}

void map_curr_frame (Img* currkf, Pose *currPose, Img* prevkf, Pose* prevPose, mat3 K_MAT, vec3* global_points, int* num) {

  K_MAT_INV1 = compute_inv(K_MAT);

  int num_features1, num_features2;

  Feature* feature1 = extract_features(currkf, &num_features1);
  Feature* feature2 = extract_features(prevkf, &num_features2);

  int num_matches;
  Correspondence* correspondences = match_and_correspond(num_features1, num_features2, feature1, feature2, &num_matches);

  *num = num_matches;
  
  RawKP kp1_raw_match[num_matches], kp2_raw_match[num_matches];

  for (int i = 0 ; i < num_matches ; i++) {
    kp1_raw_match[i].x = correspondences->x1;
    kp1_raw_match[i].y = correspondences->y1;

    kp2_raw_match[i].x = correspondences->x2;
    kp2_raw_match[i].y = correspondences->y2;

    correspondences++;
  }

  vec2 kp1_norm[num_matches], kp2_norm[num_matches];

  normalize_pixel_to_vec(kp1_raw_match, kp2_raw_match, kp1_norm, kp2_norm, num_matches);

  vec3 currCam[num_matches];

  triangulate_all(kp1_norm, kp2_norm, num_matches, &currPose->R, &currPose->t, currCam);

  project_to_world(currCam, currPose, global_points, num_matches);
}