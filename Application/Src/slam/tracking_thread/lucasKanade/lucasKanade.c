#include "tracking/lucasKanade.h"

uint8_t flag = false;
float gaussian_kernel[PATCH_SIZE][PATCH_SIZE];

uint8_t* get_pixel (Img* image, int u, int v) {

  return (image->pixels + (v * image->w + u));
}

void init_gaussian_kernel(float sigma) {
  float sum = 0.0f;
  int center = HALF_PATCH;

  for (int i = 0; i < PATCH_SIZE; i++) {
    for (int j = 0; j < PATCH_SIZE; j++) {
      int x = j - center;
      int y = i - center;
      float g = expf(-(x*x + y*y) / (2.0f * sigma * sigma));
      gaussian_kernel[i][j] = g;
      sum += g;
    }
  }
  for (int i = 0; i < PATCH_SIZE; i++) {
    for (int j = 0; j < PATCH_SIZE; j++) {
      gaussian_kernel[i][j] /= sum;
    }
  }
}

vec2 compute_gradient (Img* img, int u, int v) {
  if (u <= 0 || v <= 0 || u >= img->w || v >= img->h) return (vec2){0.0f, 0.0f};

  vec2 grad;

  grad.x = (float)(*(get_pixel(img, (u + 1), v)) - *(get_pixel(img, (u - 1), v))) / 2.0f;
  grad.y = (float)(*(get_pixel(img, u, (v + 1))) - *(get_pixel(img, u, (v - 1)))) / 2.0f;

  return grad;
}

void debug_cords(int u, int v) {
  if (u < 0 || v < 0 || u > WIDTH || v > HEIGHT) //printf("Out of bounds!\n");

  return;
}

void update_tensor (mat2* A, vec2 grad, float w) {
  A->m[0][0] += w * grad.x * grad.x;
  A->m[0][1] += w * grad.x * grad.y;
  A->m[1][0] += w * grad.x * grad.y;
  A->m[1][1] += w * grad.y * grad.y;
}

vec2 lucasKanade (Img* prevFrame, Img* currFrame, vec2* prevPoint, vec2 estPoint) {

  init_gaussian_kernel(HALF_PATCH/2.0f);

  vec2 result = estPoint;
  float du, dv;
  int iter = 0;

  while (true) {
    mat2 ATA = (mat2){0};
    vec2 ATb = (vec2){0};

    init_mat2(&ATA);
    init_vec2(&ATb);

    for (int i = -HALF_PATCH; i <= HALF_PATCH; i++) {
      for (int j = -HALF_PATCH; j <= HALF_PATCH; j++) {
        
        int u_prev = (int)(prevPoint->x + j);
        int v_prev = (int)(prevPoint->y + i);

        int u_curr = (int)(result.x + j);
        int v_curr = (int)(result.y + i);

        uint8_t* I_prev = get_pixel(prevFrame, u_prev, v_prev);
        uint8_t* I_curr = get_pixel(currFrame, u_curr, v_curr);
        
        debug_cords(u_prev, v_prev);
        debug_cords(u_curr, v_curr);

        float I_t = (float)(*I_curr) - (float)(*I_prev);

        vec2 grad_prev = compute_gradient(prevFrame, u_prev, v_prev);
        float w = gaussian_kernel[i + HALF_PATCH][j + HALF_PATCH];
        update_tensor(&ATA, grad_prev, w);

        ATb.x += w * grad_prev.x * (-I_t);
        ATb.y += w * grad_prev.y * (-I_t);
      }
    }

    float det = mat2_det(&ATA);
    if (fabs(det) < 1e-6) break;

    du = (ATA.m[1][1]*ATb.x - ATA.m[0][1]*ATb.y) / det;
    dv = (-ATA.m[1][0]*ATb.x + ATA.m[0][0]*ATb.y) / det;

    result.x += du;
    result.y += dv;

    float disp = (du*du + dv*dv);

    if (disp < CONV_THRESHOLD) {
      flag = true;
      //printf("Converged!\n");
      break;
    }

    if (iter >= MAX_ITERATIONS) {
      //printf("Max iterations reached : %e\n", disp);
      flag = true;
      break;
    }

    iter++;
  }

  if (!flag) printf("Lucas Kanade failed!\n");

  return result;
}

