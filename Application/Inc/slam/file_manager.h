#pragma once

#include <stdint.h>
#include <stdio.h>
#include "data_structs.h"

void save_raw(const char *filename, uint8_t *image, int width, int height) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror("Failed to open file");
    return;
  }

  size_t written = fwrite(image, sizeof(uint8_t), width * height, fp);
  if (written != (size_t)(width * height)) {
    perror("Failed to write full image");
  }

  fclose(fp);
}

void save_points(const char *filename, RawKP *points, int count) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror("Failed to open file");
    return;
  }

  size_t written = fwrite(points, sizeof(RawKP), count, fp);
  if (written != (size_t)count) {
    perror("Failed to write all points");
  }

  fclose(fp);
}

void save_poses(const char *filename, Pose *poses, int count) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) {
    perror("Failed to open file");
    return;
  }

  size_t written = fwrite(poses, sizeof(Pose), count, fp);
  if (written != (size_t)count) {
    perror("Failed to write all points");
  }

  fclose(fp);
}

int get_depth_map (char *file, uint8_t *depth, int H, int W) {

  FILE *f = fopen(file, "rb");
  if (!f) { perror("File open"); return 1; }

  size_t n_read = fread(depth, 1, H*W, f);
  if (n_read == 0) fprintf(stderr, "Error reading depth data\n");
  fclose(f);

  // Read depth buffer from npu

  return 0;
}

void save_world_points (char *filename, vec3 *points, int N) {
  FILE *f = fopen(filename, "wb");
  if (!f) { perror("fopen"); return; }

  fwrite(points, sizeof(vec3), N, f);
  fclose(f);
}

void save_norm (char *filename, vec2 *points, int N) {
  FILE *f = fopen(filename, "wb");
  if (!f) { perror("fopen"); return; }

  fwrite(points, sizeof(vec2), N, f);
  fclose(f);
}