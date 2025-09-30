################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mtk3_bsp2/mtkernel/kernel/tstdlib/bitop.c \
../mtk3_bsp2/mtkernel/kernel/tstdlib/string.c 

OBJS += \
./mtk3_bsp2/mtkernel/kernel/tstdlib/bitop.o \
./mtk3_bsp2/mtkernel/kernel/tstdlib/string.o 

C_DEPS += \
./mtk3_bsp2/mtkernel/kernel/tstdlib/bitop.d \
./mtk3_bsp2/mtkernel/kernel/tstdlib/string.d 


# Each subdirectory must supply rules for building sources it contributes
mtk3_bsp2/mtkernel/kernel/tstdlib/%.o mtk3_bsp2/mtkernel/kernel/tstdlib/%.su mtk3_bsp2/mtkernel/kernel/tstdlib/%.cyclo: ../mtk3_bsp2/mtkernel/kernel/tstdlib/%.c mtk3_bsp2/mtkernel/kernel/tstdlib/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DSTM32N657xx -DUSE_FULL_ASSERT -DUSE_FULL_LL_DRIVER -DVECT_TAB_SRAM -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -D_STM32CUBE_DISCOVERY_N657_ -DMTKBSP_STM32CUBE -DTARGET_GRP_DIR=stm32_cube -DTARGET_CPU_DIR=stm32n6 '-DTARGET_DIR=stm32_cube/discovery_stm32n657' -c -I../../Inc -I../../../../Middlewares/AI_Runtime/Npu/ll_aton -I../../../../Middlewares/AI_Runtime/Npu/Devices/STM32N6XX -I../../../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc -I../../../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/Include -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/DSP/Include -I../../../../STM32Cube_FW_N6/Drivers/BSP/Components/Common -I../../../../STM32Cube_FW_N6/Drivers/BSP/STM32N6570-DK -I../../../../Middlewares/Camera_Middleware/ISP_Library/isp/Inc -I../../../../Middlewares/Camera_Middleware/ISP_Library/evision/Inc -I../../../../Middlewares/AI_Runtime/Inc -I../../../../STM32Cube_FW_N6/Utilities/lcd -I../../../../Middlewares/Camera_Middleware -I../../../../STM32Cube_FW_N6/Drivers/BSP/Components/aps256xx -I../../../../Middlewares/Camera_Middleware/sensors -I../../../../Middlewares/Camera_Middleware/sensors/imx335 -I../../../../Middlewares/Camera_Middleware/sensors/vd6g -I../../../../Middlewares/Camera_Middleware/sensors/vd55g1 -I../../../../Middlewares/Fusion/Inc -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/config" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/include" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/mtkernel/kernel/knlinc" -I../../../../Application/Inc -I../../../../Application/Inc/slam -I../../../../Application/Inc/slam/feature_detect -I../../../../Application/Inc/slam/feature_match -I../../../../Application/Inc/slam/init_map -I../../../../Application/Inc/slam/tracking -Oz -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-tstdlib

clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-tstdlib:
	-$(RM) ./mtk3_bsp2/mtkernel/kernel/tstdlib/bitop.cyclo ./mtk3_bsp2/mtkernel/kernel/tstdlib/bitop.d ./mtk3_bsp2/mtkernel/kernel/tstdlib/bitop.o ./mtk3_bsp2/mtkernel/kernel/tstdlib/bitop.su ./mtk3_bsp2/mtkernel/kernel/tstdlib/string.cyclo ./mtk3_bsp2/mtkernel/kernel/tstdlib/string.d ./mtk3_bsp2/mtkernel/kernel/tstdlib/string.o ./mtk3_bsp2/mtkernel/kernel/tstdlib/string.su

.PHONY: clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-tstdlib

