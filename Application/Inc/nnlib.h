#ifndef MTK3BSP2_NNLIB_H
#define MTK3BSP2_NNLIB_H

#include "cmw_camera.h"
#include "stm32n6570_discovery_bus.h"
#include "stm32n6570_discovery_lcd.h"
#include "stm32n6570_discovery_xspi.h"
#include "stm32n6570_discovery.h"
#include "stm32_lcd.h"
#include "app_fuseprogramming.h"
#include "stm32_lcd_ex.h"
#include "ll_aton_rt_user_api.h"
#include "app_camerapipeline.h"
#include "main.h"
#include <stdio.h>
#include "app_config.h"
#include "crop_img.h"
//#include "stlogo.h"
#include "arm_math.h"
#include <stdint.h>


extern const char *classes_table[];

#define MAX_NUMBER_OUTPUT 5
#define LCD_FG_WIDTH  SCREEN_WIDTH
#define LCD_FG_HEIGHT SCREEN_HEIGHT
#define LCD_FG_FRAMEBUFFER_SIZE  (LCD_FG_WIDTH * LCD_FG_HEIGHT * 2)

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;



typedef struct
{
  uint32_t X0;
  uint32_t Y0;
  uint32_t XSize;
  uint32_t YSize;
} Rectangle_TypeDef;

/* Lcd Background area */
extern Rectangle_TypeDef lcd_bg_area;

/* Lcd Foreground area */
extern Rectangle_TypeDef lcd_fg_area;

#define NUMBER_COLORS 10
extern const uint32_t colors[NUMBER_COLORS];

extern volatile int32_t cameraFrameReceived;
extern uint8_t *nn_in;
extern BSP_LCD_LayerConfig_t LayerConfig;
extern uint8_t *pp_input;
extern char const *nn_top1_output_class_name;
extern float nn_top1_output_class_proba;

#define ALIGN_TO_16(value) (((value) + 15) & ~15)

/* for models not multiple of 16; needs a working buffer */
#if (NN_WIDTH * NN_BPP) != ALIGN_TO_16(NN_WIDTH * NN_BPP)
#define DCMIPP_OUT_NN_LEN (ALIGN_TO_16(NN_WIDTH * NN_BPP) * NN_HEIGHT)
#define DCMIPP_OUT_NN_BUFF_LEN (DCMIPP_OUT_NN_LEN + 32 - DCMIPP_OUT_NN_LEN%32)


__attribute__ ((aligned (32)))
extern uint8_t dcmipp_out_nn[DCMIPP_OUT_NN_BUFF_LEN];
#else
extern uint8_t *dcmipp_out_nn;
#endif

#define DEPTH_MAP_OUTPUT_WIDTH 224
#define DEPTH_MAP_OUTPUT_HEIGHT 224

/* Lcd Background Buffer */
__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
extern uint8_t lcd_bg_buffer[800 * 480 * 2];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
extern uint8_t zero_buffer[800 * 480 * 2];

/* Lcd Foreground Buffer */
__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
extern uint8_t lcd_fg_buffer[2][LCD_FG_WIDTH * LCD_FG_HEIGHT * 2];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
extern uint8_t depth_map[224][224];
//static int lcd_fg_buffer_rd_idx;

extern uint8_t depth_map_display_buffer[224 * 224 * 2];

void Mem_init(void);
void SystemClock_Config(void);
void Run_Inference(void);
void NeuralNetwork_init(uint32_t *nnin_length, uint8_t *nn_out[], int8_t *number_output, int32_t nn_out_len[]);
void process_depth_map(void);
void display_depth_map(void);
void LCD_init(void);
void Display_Image(uint8_t* p_data, uint32_t x_res, uint32_t y_res, uint32_t x_pos, uint32_t y_pos);


#endif
