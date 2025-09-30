#include "tracking/track_frames.h"

mat3 K_CAM;
int* inv_idx;
Pose currOptimalPose;

void reProjectPoints (vec3* worldPoints, Pose* pose, vec2* estPixels, int num) {
  for (int i = 0 ; i < num ; i++) {

    vec3 rot = product_mat3_vec3(pose->R, worldPoints[i]);
    vec3 rot_scal = {rot.x*pose->s, rot.y*pose->s, rot.z*pose->s};
    vec3 dir = vec3_add(rot_scal, pose->t);

    if (dir.z <= 0) {
      estPixels[i] = (vec2){-1, -1};
      *(inv_idx++) = i;
      continue;
    }
    vec3 pixel = product_mat3_vec3(K_CAM, dir);


    estPixels[i] = (vec2){pixel.x/pixel.z, pixel.y/pixel.z};
  }

  return;
}

void testLK (vec3* worldPoints, Img* currFrame, Img* prevFrame, Pose* prevPose, vec2* estPixels, vec2* lkPixels, int num) {
  for (int i = 0 ; i < num ; i++) {

    if (estPixels[i].x == -1) {
      lkPixels[i] = (vec2){-1, -1};
      *(inv_idx++) = i;

      continue;
    }
    vec3 rot = product_mat3_vec3(prevPose->R, worldPoints[i]);
    vec3 rot_scal = {rot.x*prevPose->s, rot.y*prevPose->s, rot.z*prevPose->s};
    vec3 dir = vec3_add(rot_scal, prevPose->t);

    if (dir.z <= 0) {
      *(inv_idx++) = i;
      continue;
    }

    vec3 pixel = product_mat3_vec3(K_CAM, dir);

    vec2 prevUV = {pixel.x/pixel.z, pixel.y/pixel.z};
    lkPixels[i] = lucasKanade(prevFrame, currFrame, &prevUV, estPixels[i]);

  }

  return;
}

int bin_search(int* arr, int size, int target) {
  int left = 0;
  int right = size - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2;

    if (arr[mid] == target) return mid;
    else if (arr[mid] < target) left = mid + 1;
    else right = mid - 1;
  }

  return -1;
}

void computeProjectionError (vec2* estPixels, vec2* lkPixels, float* error, int num, int* invalid_idx, int size) {
  for (int i = 0; i < num; i++) {
    if (bin_search(invalid_idx, size, i) == -1)
      error[i] = (estPixels[i].x - lkPixels[i].x)*(estPixels[i].x - lkPixels[i].x) + (estPixels[i].y - lkPixels[i].y)*(estPixels[i].y - lkPixels[i].y);
  }

  return;
}

void getImuPose() {
  // get relative pose (replace non void return)
}

void estAbsPose() { 
  // est abs pose (replace non void return)
}

void updateOptimalPose (Pose* pose) {
  Pose temp = currOptimalPose, Tfw;
  mat3_mult(temp.R, pose->R, &Tfw.R);
  Tfw.t = vec3_add(product_mat3_vec3(temp.R, (vec3){pose->t.x * temp.s, pose->t.y * temp.s, pose->t.z * temp.s}), temp.t);
  Tfw.s = temp.s * pose->s;

  currOptimalPose = Tfw;

  return;
}

uint8_t decideKF(Pose* currPose) {
  float dist = vec3_norm(currPose->t, currOptimalPose.t);

  if (dist < TRANSLATION_THRESHOLD) return true;

  return false;
}

int tracking_thread (vec3* worldPoints, const mat3 K_MAT, Img* currFrame, Img* recentFrame, Pose* recentPose, int numPoints, uint8_t *status) {

  K_CAM = K_MAT;

  //Pose* relPose = getImuPose();
  //Pose* absPose = estAbsPose(relPose);

  Pose* absPose = recentPose; // Guess

  int invalid_idx[numPoints];
  inv_idx = invalid_idx;

  vec2 estPixels[numPoints];

  reProjectPoints(worldPoints, absPose, estPixels, numPoints);

  printf("Done reprojection\n");

  vec2 lkPixels[numPoints];

  testLK(worldPoints, currFrame, recentFrame, recentPose, estPixels, lkPixels, numPoints);

  float ProjectionError[numPoints];

  int inv_size = ((inv_idx) - (invalid_idx));

  printf("inv_size = %d\n", inv_size);

  computeProjectionError(estPixels, lkPixels, ProjectionError, numPoints, invalid_idx, inv_size);

  printf("-------------\nError :\n");
  for (int i = 0; i < 10; i++) printf("%e\n", ProjectionError[i]);

  pose_only_bundle_adjustment(worldPoints, lkPixels, numPoints, &absPose->R, &absPose->t, K_CAM, 10, 3.0f);

  reProjectPoints(worldPoints, absPose, estPixels, numPoints);

  computeProjectionError(estPixels, lkPixels, ProjectionError, numPoints, invalid_idx, inv_size);
  printf("-------------\nError :\n");
  for (int i = 0; i < 10; i++) printf("%e\n", ProjectionError[i]);

  *status = decideKF(absPose);

  updateOptimalPose(absPose);

  return 0;
}
