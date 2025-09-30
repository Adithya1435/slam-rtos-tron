#ifndef IMU_INT_H
#define IMU_INT_H

#include "stm32n6xx_hal_conf.h"
#include "stm32n6xx_hal.h"
#include "stm32_lcd_ex.h"
#include "arm_math.h"
#include "nnlib.h"
#include "math_structs.h"

#include "Fusion.h"
#include <stdbool.h>

extern mat3 R_imu;
extern vec3 t_imu;


void IMU_Init(void);


#endif
