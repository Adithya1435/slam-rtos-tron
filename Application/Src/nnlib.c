/**
 ******************************************************************************
 * @file    main.c
 * @author  GPM Application Team
 *
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "nnlib.h"
#include "stm32n6xx.h"
#include "stm32n6xx_hal.h"
#include "stm32n6570_discovery_lcd.h"
#include "stm32_lcd_ex.h"

ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;


Rectangle_TypeDef lcd_bg_area = {
#if ASPECT_RATIO_MODE == ASPECT_RATIO_CROP || ASPECT_RATIO_MODE == ASPECT_RATIO_FIT
  .X0 = (LCD_FG_WIDTH - LCD_FG_HEIGHT) / 2,
#else
  .X0 = 0,
#endif
  .Y0 = 0,
  .XSize = 0,
  .YSize = 0,
};

Rectangle_TypeDef lcd_fg_area = {
  .X0 = 0,
  .Y0 = 0,
  .XSize = LCD_FG_WIDTH,
  .YSize = LCD_FG_HEIGHT,
};

const uint32_t colors[NUMBER_COLORS] = {
    UTIL_LCD_COLOR_GREEN,
    UTIL_LCD_COLOR_RED,
    UTIL_LCD_COLOR_CYAN,
    UTIL_LCD_COLOR_MAGENTA,
    UTIL_LCD_COLOR_YELLOW,
    UTIL_LCD_COLOR_GRAY,
    UTIL_LCD_COLOR_BLACK,
    UTIL_LCD_COLOR_BROWN,
    UTIL_LCD_COLOR_BLUE,
    UTIL_LCD_COLOR_ORANGE
};

uint8_t *nn_in;
BSP_LCD_LayerConfig_t LayerConfig = {0};
uint8_t *pp_input;
char const *nn_top1_output_class_name;
float nn_top1_output_class_proba;

#if (NN_WIDTH * NN_BPP) != ALIGN_TO_16(NN_WIDTH * NN_BPP)
__attribute__ ((aligned (32)))
uint8_t dcmipp_out_nn[DCMIPP_OUT_NN_BUFF_LEN];
#else
uint8_t *dcmipp_out_nn;
#endif

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
uint8_t lcd_bg_buffer[800 * 480 * 2];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
uint8_t zero_buffer[800 * 480 * 2];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
uint8_t lcd_fg_buffer[2][LCD_FG_WIDTH * LCD_FG_HEIGHT * 2];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
uint8_t depth_map[224][224];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
uint8_t depth_map_display_buffer[224 * 224 * 2];


CLASSES_TABLE;


void Mem_init(void);
void SystemClock_Config(void);
void NPURam_enable(void);
void NPUCache_config(void);
void LCD_init(void);
void Security_Config(void);
void set_clk_sleep_mode(void);
void IAC_Config(void);
void Display_WelcomeScreen(void);
void Run_Inference(void);
void NeuralNetwork_init(uint32_t *nnin_length, uint8_t *nn_out[], int8_t *number_output, int32_t nn_out_len[]);
void process_depth_map(void);
void display_depth_map(void);


LL_ATON_DECLARE_NAMED_NN_INSTANCE_AND_INTERFACE(Default);



void Mem_init(void)
{
  /* Power on ICACHE */
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_ICACTIVE_Msk;

  SCB_EnableICache();

  #if defined(USE_DCACHE)
    /* Power on DCACHE */
  MEMSYSCTL->MSCR |= MEMSYSCTL_MSCR_DCACTIVE_Msk;
  SCB_EnableDCache();
  #endif


  NPURam_enable();

  Fuse_Programming();

  NPUCache_config();

  /*** External RAM and NOR Flash *********************************************/
  BSP_XSPI_RAM_Init(0);
  BSP_XSPI_RAM_EnableMemoryMappedMode(0);

  BSP_XSPI_NOR_Init_t NOR_Init;
  NOR_Init.InterfaceMode = BSP_XSPI_NOR_OPI_MODE;
  NOR_Init.TransferRate = BSP_XSPI_NOR_DTR_TRANSFER;
  BSP_XSPI_NOR_Init(0, &NOR_Init);
  BSP_XSPI_NOR_EnableMemoryMappedMode(0);

  /* Set all required IPs as secure privileged */
  Security_Config();

  IAC_Config();
  set_clk_sleep_mode();

}

void Run_Inference(void) {
  LL_ATON_RT_RetValues_t ll_aton_rt_ret;

  do
  {
    ll_aton_rt_ret = LL_ATON_RT_RunEpochBlock(&NN_Instance_Default);

    /* Wait for next event */
    if (ll_aton_rt_ret == LL_ATON_RT_WFE)
    {
      LL_ATON_OSAL_WFE();
    }
  } while (ll_aton_rt_ret != LL_ATON_RT_DONE);

  LL_ATON_RT_Reset_Network(&NN_Instance_Default);
}



void NeuralNetwork_init(uint32_t *nnin_length, uint8_t *nn_out[], int8_t *number_output, int32_t nn_out_len[])
{
  const LL_Buffer_InfoTypeDef *nn_in_info = LL_ATON_Input_Buffers_Info(&NN_Instance_Default);
  const LL_Buffer_InfoTypeDef *nn_out_info = LL_ATON_Output_Buffers_Info(&NN_Instance_Default);

  // Get the input buffer address
  nn_in = (uint8_t *) LL_Buffer_addr_start(&nn_in_info[0]);

  /* Count number of outputs */
  while (nn_out_info[*number_output].name != NULL)
  {
    (*number_output)++;
  }
  assert(*number_output <= MAX_NUMBER_OUTPUT);

  for (int i = 0; i < *number_output; i++)
  {
    // Get the output buffers address
    nn_out[i] = (uint8_t *) LL_Buffer_addr_start(&nn_out_info[i]);
    nn_out_len[i] = LL_Buffer_len(&nn_out_info[i]);
  }

  *nnin_length = LL_Buffer_len(&nn_in_info[0]);

  LL_ATON_RT_RuntimeInit();
  LL_ATON_RT_Init_Network(&NN_Instance_Default);
}

void NPURam_enable(void)
{
  __HAL_RCC_NPU_CLK_ENABLE();
  __HAL_RCC_NPU_FORCE_RESET();
  __HAL_RCC_NPU_RELEASE_RESET();

  /* Enable NPU RAMs (4x448KB) */
  __HAL_RCC_AXISRAM3_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_ENABLE();
  __HAL_RCC_RAMCFG_CLK_ENABLE();
  RAMCFG_HandleTypeDef hramcfg = {0};
  hramcfg.Instance =  RAMCFG_SRAM3_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
  hramcfg.Instance =  RAMCFG_SRAM4_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
  hramcfg.Instance =  RAMCFG_SRAM5_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
  hramcfg.Instance =  RAMCFG_SRAM6_AXI;
  HAL_RAMCFG_EnableAXISRAM(&hramcfg);
}

void set_clk_sleep_mode(void)
{
  /*** Enable sleep mode support during NPU inference *************************/
  /* Configure peripheral clocks to remain active during sleep mode */
  /* Keep all IP's enabled during WFE so they can wake up CPU. Fine tune
   * this if you want to save maximum power
   */
  __HAL_RCC_XSPI1_CLK_SLEEP_ENABLE();    /* For display frame buffer */
  __HAL_RCC_XSPI2_CLK_SLEEP_ENABLE();    /* For NN weights */
  __HAL_RCC_NPU_CLK_SLEEP_ENABLE();      /* For NN inference */
  __HAL_RCC_CACHEAXI_CLK_SLEEP_ENABLE(); /* For NN inference */
  __HAL_RCC_LTDC_CLK_SLEEP_ENABLE();     /* For display */
  __HAL_RCC_DMA2D_CLK_SLEEP_ENABLE();    /* For display */
  __HAL_RCC_DCMIPP_CLK_SLEEP_ENABLE();   /* For camera configuration retention */
  __HAL_RCC_CSI_CLK_SLEEP_ENABLE();      /* For camera configuration retention */

  __HAL_RCC_FLEXRAM_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM1_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM2_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM3_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM4_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM5_MEM_CLK_SLEEP_ENABLE();
  __HAL_RCC_AXISRAM6_MEM_CLK_SLEEP_ENABLE();

}

void NPUCache_config(void)
{

  npu_cache_init();
  npu_cache_enable();
}

void Security_Config(void)
{
  __HAL_RCC_RIFSC_CLK_ENABLE();
  RIMC_MasterConfig_t RIMC_master = {0};
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_NPU, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DMA2D, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DCMIPP, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC1 , &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC2 , &RIMC_master);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_NPU , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DMA2D , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_CSI    , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DCMIPP , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDC   , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL1 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL2 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
}

void IAC_Config(void)
{
/* Configure IAC to trap illegal access events */
  __HAL_RCC_IAC_CLK_ENABLE();
  __HAL_RCC_IAC_FORCE_RESET();
  __HAL_RCC_IAC_RELEASE_RESET();
}

void IAC_IRQHandler(void)
{
  while (1)
  {
  }
}


void process_depth_map(void)
{
    uint8_t *raw_depth_8bit = (uint8_t *)pp_input;
    int temp_count = 0;
        for(int i=0; i<224; i++){
        	for(int j=0; j<224; j++){
        		uint8_t* depth = (uint8_t *)pp_input;
        		depth_map[i][j] = depth[temp_count];
        		temp_count++;
        	}
        }

    uint8_t *display_map_rgb565 = depth_map_display_buffer;
    uint32_t map_pixel_count = 224 * 224;

    // 1. Find min/max depth values for normalization (for 8-bit data)
    uint8_t min_depth = raw_depth_8bit[0];
    uint8_t max_depth = raw_depth_8bit[0];
    for (uint32_t i = 1; i < map_pixel_count; i++) {
        if (raw_depth_8bit[i] < min_depth) min_depth = raw_depth_8bit[i];
        if (raw_depth_8bit[i] > max_depth) max_depth = raw_depth_8bit[i];
    }

    if (min_depth == max_depth) {
        memset(display_map_rgb565, 0, map_pixel_count * 2); // Display black if no depth variation
        SCB_CleanInvalidateDCache_by_Addr((uint32_t*)display_map_rgb565, map_pixel_count * 2);
        return;
    }

    float log_min_val = log10f((float)min_depth + 1.0f); // +1 to avoid log(0)
    float log_max_val = log10f((float)max_depth + 1.0f);
    float log_range = log_max_val - log_min_val;

    if (log_range == 0.0f) log_range = 1.0f; // Prevent division by zero

    for (uint32_t i = 0; i < map_pixel_count; i++) {
        float current_log_depth = log10f((float)raw_depth_8bit[i] + 1.0f);

        float normalized_val = (current_log_depth - log_min_val) / log_range;
        uint8_t r, g, b;

        if (normalized_val < 0.25f) { // Blue dominant
            r = 0;
            g = (uint8_t)(255 * normalized_val * 4);
            b = 255;
        } else if (normalized_val < 0.5f) { // Blue to Green
            r = 0;
            g = 255;
            b = (uint8_t)(255 * (1 - (normalized_val - 0.25f) * 4));
        } else if (normalized_val < 0.75f) { // Green to Yellow
            r = (uint8_t)(255 * (normalized_val - 0.5f) * 4);
            g = 255;
            b = 0;
        } else { // Yellow to Red
            r = 255;
            g = (uint8_t)(255 * (1 - (normalized_val - 0.75f) * 4));
            b = 0;
        }

        // Convert 8-bit RGB to 16-bit RGB565
        uint16_t rgb565_pixel = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        display_map_rgb565[i * 2] = (uint8_t)(rgb565_pixel & 0xFF);         // Low byte
        display_map_rgb565[i * 2 + 1] = (uint8_t)(rgb565_pixel >> 8);     // High byte
    }
    SCB_CleanInvalidateDCache_by_Addr((uint32_t*)display_map_rgb565, map_pixel_count * 2);

}

void Display_Image(uint8_t* p_data, uint32_t x_res, uint32_t y_res, uint32_t x_pos, uint32_t y_pos)
{
  /* This function writes directly to the active frame buffer of Layer 1 */
 BSP_LCD_FillRGBRect(0, x_pos, y_pos, p_data, x_res, y_res);
}

int int_to_str(int num, char *buf) {
    char temp[12];  // Enough for 32-bit int
    int i = 0;

    if(num == 0) {
        buf[0] = '0';
        return 1;
    }

    // Handle negative numbers
    int is_neg = 0;
    if(num < 0) {
        is_neg = 1;
        num = -num;
    }

    // Convert digits to string in reverse
    while(num > 0) {
        temp[i++] = (num % 10) + '0';
        num /= 10;
    }

    int j = 0;
    if(is_neg) {
        buf[j++] = '-';
    }

    // Reverse digits into buffer
    while(i > 0) {
        buf[j++] = temp[--i];
    }

    return j; // number of characters written
}

void array_to_string(int arr[], int size, char *str) {
    int pos = 0;

    for(int i = 0; i < size; i++) {
        int len = int_to_str(arr[i], &str[pos]);
        pos += len;

        // Add space if not last element
//        if(i != size - 1) {
//            str[pos++] = '';
//        }
    }

    str[pos] = '\0'; // null-terminate string
}
/**
 * @brief Display the processed depth map on the LCD
 * @param ctx Application context
 */
void display_depth_map(void)
{
    //Display the depth map fullscreen.
	//BSP_LCD_DisplayOn(0);
    Display_Image((uint8_t*)depth_map_display_buffer, 224, 224, 288, 128);
    //BSP_LCD_DisplayOff(0);
	 //BSP_LCD_FillRGBRect(0, 23, 32, (uint8_t*)ctx->depth_map_display_buffer, DEPTH_MAP_OUTPUT_WIDTH, DEPTH_MAP_OUTPUT_HEIGHT);

//	for(int i=1; i< 25; i++){
//
//		char buffer[46];
//
//		array_to_string(depth_map[i], 45, buffer);
//
//		UTIL_LCDEx_PrintfAtLine(i, "%s", buffer);
//	}
}

void LCD_init(void)
{
  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);

  for(int i=0; i<800 * 480 * 2; i++){
	  zero_buffer[i] = 0;
  }

//  /* Preview layer Init */
  LayerConfig.X0          = lcd_bg_area.X0;
  LayerConfig.Y0          = lcd_bg_area.Y0;
  LayerConfig.X1          = lcd_bg_area.X0 + lcd_bg_area.XSize;
  LayerConfig.Y1          = lcd_bg_area.Y0 + lcd_bg_area.YSize;
  LayerConfig.PixelFormat = LCD_PIXEL_FORMAT_RGB565;
  LayerConfig.Address     = (uint32_t) zero_buffer;

  BSP_LCD_ConfigLayer(0, LTDC_LAYER_1, &LayerConfig);
//
  LayerConfig.X0 = lcd_fg_area.X0;
  LayerConfig.Y0 = lcd_fg_area.Y0;
  LayerConfig.X1 = lcd_fg_area.X0 + lcd_fg_area.XSize;
  LayerConfig.Y1 = lcd_fg_area.Y0 + lcd_fg_area.YSize;
  LayerConfig.PixelFormat = LCD_PIXEL_FORMAT_ARGB4444;
  LayerConfig.Address = (uint32_t) lcd_fg_buffer; /* External XSPI1 PSRAM */

  BSP_LCD_ConfigLayer(0, LTDC_LAYER_2, &LayerConfig);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  UTIL_LCD_SetLayer(LTDC_LAYER_2);
  UTIL_LCD_Clear(0x00000000);
  UTIL_LCD_SetFont(&Font20);
  UTIL_LCD_SetBackColor(0x40000000);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
}

/**
  * @brief  DCMIPP Clock Config for DCMIPP.
  * @param  hdcmipp  DCMIPP Handle
  *         Being __weak it can be overwritten by the application
  * @retval HAL_status
  */
HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp)
{
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};
  HAL_StatusTypeDef ret = HAL_OK;

  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_DCMIPP;
  RCC_PeriphCLKInitStruct.DcmippClockSelection = RCC_DCMIPPCLKSOURCE_IC17;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC17].ClockSelection = RCC_ICCLKSOURCE_PLL2;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC17].ClockDivider = 3;
  ret = HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
  if (ret)
  {
    return ret;
  }

  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CSI;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC18].ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC18].ClockDivider = 40;
  ret = HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);
  if (ret)
  {
    return ret;
  }

  return ret;
}

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};

  /* Ensure VDDCORE=0.9V before increasing the system frequency */
  BSP_SMPS_Init(SMPS_VOLTAGE_OVERDRIVE);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;

  /* PLL1 = 64 x 25 / 2 = 800MHz */
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 2;
  RCC_OscInitStruct.PLL1.PLLN = 25;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;
  RCC_OscInitStruct.PLL1.PLLP1 = 1;
  RCC_OscInitStruct.PLL1.PLLP2 = 1;

  /* PLL2 = 64 x 125 / 8 = 1000MHz */
  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL2.PLLM = 8;
  RCC_OscInitStruct.PLL2.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLN = 125;
  RCC_OscInitStruct.PLL2.PLLP1 = 1;
  RCC_OscInitStruct.PLL2.PLLP2 = 1;

  /* PLL3 = (64 x 225 / 8) / (1 * 2) = 900MHz */
  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL3.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL3.PLLM = 8;
  RCC_OscInitStruct.PLL3.PLLN = 225;
  RCC_OscInitStruct.PLL3.PLLFractional = 0;
  RCC_OscInitStruct.PLL3.PLLP1 = 1;
  RCC_OscInitStruct.PLL3.PLLP2 = 2;

  /* PLL4 = (64 x 225 / 8) / (6 * 6) = 50 MHz */
  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL4.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL4.PLLM = 8;
  RCC_OscInitStruct.PLL4.PLLFractional = 0;
  RCC_OscInitStruct.PLL4.PLLN = 225;
  RCC_OscInitStruct.PLL4.PLLP1 = 6;
  RCC_OscInitStruct.PLL4.PLLP2 = 6;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }

  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK |
                                 RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |
                                 RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK4 |
                                 RCC_CLOCKTYPE_PCLK5);

  /* CPU CLock (sysa_ck) = ic1_ck = PLL1 output/ic1_divider = 800 MHz */
  RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider = 1;

  /* AXI Clock (sysb_ck) = ic2_ck = PLL1 output/ic2_divider = 400 MHz */
  RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC2Selection.ClockDivider = 2;

  /* NPU Clock (sysc_ck) = ic6_ck = PLL2 output/ic6_divider = 1000 MHz */
  RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL2;
  RCC_ClkInitStruct.IC6Selection.ClockDivider = 1;

  /* AXISRAM3/4/5/6 Clock (sysd_ck) = ic11_ck = PLL3 output/ic11_divider = 900 MHz */
  RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL3;
  RCC_ClkInitStruct.IC11Selection.ClockDivider = 1;

  /* HCLK = sysb_ck / HCLK divider = 200 MHz */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;

  /* PCLKx = HCLK / PCLKx divider = 200 MHz */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    while(1);
  }

  RCC_PeriphCLKInitStruct.PeriphClockSelection = 0;

  /* XSPI1 kernel clock (ck_ker_xspi1) = HCLK = 200MHz */
  RCC_PeriphCLKInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_XSPI1;
  RCC_PeriphCLKInitStruct.Xspi1ClockSelection = RCC_XSPI1CLKSOURCE_HCLK;

  /* XSPI2 kernel clock (ck_ker_xspi1) = HCLK =  200MHz */
  RCC_PeriphCLKInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_XSPI2;
  RCC_PeriphCLKInitStruct.Xspi2ClockSelection = RCC_XSPI2CLKSOURCE_HCLK;

  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
  {
    while (1);
  }
}

void HAL_CACHEAXI_MspInit(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  __HAL_RCC_CACHEAXIRAM_MEM_CLK_ENABLE();
  __HAL_RCC_CACHEAXI_CLK_ENABLE();
  __HAL_RCC_CACHEAXI_FORCE_RESET();
  __HAL_RCC_CACHEAXI_RELEASE_RESET();
}

void HAL_CACHEAXI_MspDeInit(CACHEAXI_HandleTypeDef *hcacheaxi)
{
  __HAL_RCC_CACHEAXIRAM_MEM_CLK_DISABLE();
  __HAL_RCC_CACHEAXI_CLK_DISABLE();
  __HAL_RCC_CACHEAXI_FORCE_RESET();
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  UNUSED(file);
  UNUSED(line);
  __BKPT(0);
  while (1)
  {
  }
}

#endif
