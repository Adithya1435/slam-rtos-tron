#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stb_image.h"
#include "data_structs.h"

void load_image (const char* file, Img* image, bool *status) {

  int ch;
  image->pixels = stbi_load(file, &image->w, &image->h, &ch, 1);

  if (!image->pixels) {
    *status = false;
    return;
  }

  *status = true;
  //printf("Loaded image %s\n", file);

  return;
}

void free_img (Img* image) {

  if (image->pixels) {
    stbi_image_free(image->pixels);
    image->pixels = NULL;
  }
  //image->status = false;

  return;
}
/*
void print_pixel (Img* frame, int u, int v) {

  uint8_t* p = get_pixel(frame, u, v);
  printf("(%d, %d) : %d\n", u, v, p[0]);

  return;
} */

int load_frames(char *file1, Img *frame1, char *file2, Img *frame2) {

  bool status1, status2;
  load_image(file1, frame1, &status1);
  load_image(file2, frame2, &status2);

  if (!status1 || !status2) {
    printf("Error loading frames!\n");
    free_img(frame1);
    free_img(frame2);

    return 1;
  }
  return 0;
}