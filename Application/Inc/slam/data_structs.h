#pragma once

#include "math_structs.h"

#define WIDTH 1242
#define HEIGHT 375

typedef struct {
  uint8_t *pixels;
  int w, h;
} Img;

typedef struct {
  int x, y;
  float response;
} Keypoint;

typedef struct {
  int x;
  int y;
} RawKP;

typedef struct {
  int x, y, z;
} Point3;

typedef struct {
  uint64_t descriptor[4];
} BriefDescriptor;

typedef struct {
  Keypoint keypoint;
  BriefDescriptor descriptor;
} Feature;

typedef struct {
  int idx1, idx2;
  int distance;
  float confidence;
} Match;

typedef struct {
  int x1, y1;
  int x2, y2;
  float confidence;
} Correspondence;

typedef struct {
  mat3 R;
  vec3 t;
  float s;
} Pose;