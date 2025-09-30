#include "init_map/init_map.h"
#include "tracking/track_frames.h"
#include "loader.h"
#include "intrinsics.h"
#include "file_manager.h"

#define MAX_WORLD_POINTS 1000
 
void save_files (Img frame1, Img frame2, RawKP *kp1, RawKP* kp2, int size) {
  save_raw("bin/frame1.raw", frame1.pixels, frame1.w, frame1.h);
  save_points("bin/kp1.raw", kp1, size);

  save_raw("bin/frame2.raw", frame2.pixels, frame1.w, frame1.h);
  save_points("bin/kp2.raw", kp2, size);

  return;
}

Pose init_pose() {
  Pose initPose = {0};
  initPose.R = mat3_identity();
  initPose.t = (vec3){0.0f, 0.0f, 0.0f};
  initPose.s = 1.0f;

  return initPose;
}

int main() {

  // Current world points
  vec3 CURRENT_WORLD_POINTS[MAX_WORLD_POINTS];

  Pose ALL_POSES[20];

  // Current Pose
  Pose currPose;

  // Assign space for depth map
  uint8_t *depth1 = (uint8_t *) malloc(sizeof(uint8_t) * HEIGHT * WIDTH);
  uint8_t *depth2 = (uint8_t *) malloc(sizeof(uint8_t) * HEIGHT * WIDTH);

  // Generate depth map
  get_depth_map("bin/depth_map_00.raw", depth1, HEIGHT, WIDTH);
  get_depth_map("bin/depth_map_01.raw", depth2, HEIGHT, WIDTH);

  // Map Init (get raw 2D points)
  RawKP pixels1[MAX_WORLD_POINTS], pixels2[MAX_WORLD_POINTS];
  int num_matches;
  Img frame1, frame2;
  if (load_frames("dataset/00.png", &frame1, "dataset/01.png", &frame2)) return 1;
  init_map(&frame1, &frame2, K_MAT, depth1, depth2, &num_matches, CURRENT_WORLD_POINTS, (int) MAX_WORLD_POINTS, &currPose, "yes", pixels1, pixels2);

  int minSize = MAX_WORLD_POINTS;
  if (num_matches < (int)MAX_WORLD_POINTS) minSize = num_matches;

  printf("----\n num matches = %d\n----\n", minSize);

  // Save raw 2D points
  save_files (frame1, frame2, pixels1, pixels2, minSize);

  // Save world points
  save_world_points("bin/WorldPoints.bin", CURRENT_WORLD_POINTS, minSize);

  // Print world space points
  printf("World space points :\n");
  for (int i = 0 ; i < 10 ; i++) vec3_print("Point", CURRENT_WORLD_POINTS[i]);
  puts("...");

  ALL_POSES[0] = init_pose();
  ALL_POSES[1] = currPose;
  Pose currOptimalPose = currPose;

  // Print the current pose
  printf("Recovered pose :\n");
  mat3_print("R", currPose.R);
  vec3_print("t", currPose.t);
  printf("s : %f\n", currPose.s);

  printf("-----------------------------------------\n");
  printf("Map Initialization successful!\n");
  printf("-----------------------------------------\n");
  printf("Tracking...\n");
  
  // Tracking
  uint8_t KF_st;
  Img frame3; bool st;
  load_image("dataset/02.png", &frame3, &st);
  tracking_thread(CURRENT_WORLD_POINTS, K_MAT, &frame3, &frame2, &currPose, minSize, &KF_st);

  ALL_POSES[2] = currOptimalPose;

  // Print the current pose
  printf("Recovered pose tracking :\n");
  mat3_print("R", currOptimalPose.R);
  vec3_print("t", currOptimalPose.t);
  printf("s : %f\n", currOptimalPose.s);
  
  // Frame 4
  Img frame4; bool st4;
  load_image("dataset/03.png", &frame4, &st4);
  tracking_thread(CURRENT_WORLD_POINTS, K_MAT, &frame4, &frame3, &currOptimalPose, minSize, &KF_st);

  ALL_POSES[3] = currOptimalPose;

  // Frame 5
  Img frame5; bool st5;
  load_image("dataset/04.png", &frame5, &st5);
  tracking_thread(CURRENT_WORLD_POINTS, K_MAT, &frame5, &frame4, &currOptimalPose, minSize, &KF_st);

  ALL_POSES[4] = currOptimalPose;

  save_poses("bin/poses.raw", ALL_POSES, 5);

  return 0;
}