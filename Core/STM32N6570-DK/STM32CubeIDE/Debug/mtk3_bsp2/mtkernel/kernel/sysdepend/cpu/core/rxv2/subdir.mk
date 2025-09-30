################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/cpu_cntl.c \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/exc_hdr.c \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/interrupt.c \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_main.c \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/vector_tbl.c 

S_UPPER_SRCS += \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/dispatch.S \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/int_asm.S \
../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_hdl.S 

OBJS += \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/cpu_cntl.o \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/dispatch.o \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/exc_hdr.o \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/int_asm.o \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/interrupt.o \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_hdl.o \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_main.o \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/vector_tbl.o 

S_UPPER_DEPS += \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/dispatch.d \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/int_asm.d \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_hdl.d 

C_DEPS += \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/cpu_cntl.d \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/exc_hdr.d \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/interrupt.d \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_main.d \
./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/vector_tbl.d 


# Each subdirectory must supply rules for building sources it contributes
mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/%.o mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/%.su mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/%.cyclo: ../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/%.c mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DSTM32N657xx -DUSE_FULL_ASSERT -DUSE_FULL_LL_DRIVER -DVECT_TAB_SRAM -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -D_STM32CUBE_DISCOVERY_N657_ -DMTKBSP_STM32CUBE -DTARGET_GRP_DIR=stm32_cube -DTARGET_CPU_DIR=stm32n6 '-DTARGET_DIR=stm32_cube/discovery_stm32n657' -c -I../../Inc -I../../../../Middlewares/AI_Runtime/Npu/ll_aton -I../../../../Middlewares/AI_Runtime/Npu/Devices/STM32N6XX -I../../../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc -I../../../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/Include -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/DSP/Include -I../../../../STM32Cube_FW_N6/Drivers/BSP/Components/Common -I../../../../STM32Cube_FW_N6/Drivers/BSP/STM32N6570-DK -I../../../../Middlewares/Camera_Middleware/ISP_Library/isp/Inc -I../../../../Middlewares/Camera_Middleware/ISP_Library/evision/Inc -I../../../../Middlewares/AI_Runtime/Inc -I../../../../STM32Cube_FW_N6/Utilities/lcd -I../../../../Middlewares/Camera_Middleware -I../../../../STM32Cube_FW_N6/Drivers/BSP/Components/aps256xx -I../../../../Middlewares/Camera_Middleware/sensors -I../../../../Middlewares/Camera_Middleware/sensors/imx335 -I../../../../Middlewares/Camera_Middleware/sensors/vd6g -I../../../../Middlewares/Camera_Middleware/sensors/vd55g1 -I../../../../Middlewares/Fusion/Inc -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/config" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/include" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/mtkernel/kernel/knlinc" -I../../../../Application/Inc -I../../../../Application/Inc/slam -I../../../../Application/Inc/slam/feature_detect -I../../../../Application/Inc/slam/feature_match -I../../../../Application/Inc/slam/init_map -I../../../../Application/Inc/slam/tracking -Oz -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/%.o: ../mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/%.S mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m55 -g3 -DDEBUG -D_STM32CUBE_DISCOVERY_N657_ -c -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/config" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/include" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/mtkernel/kernel/knlinc" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-sysdepend-2f-cpu-2f-core-2f-rxv2

clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-sysdepend-2f-cpu-2f-core-2f-rxv2:
	-$(RM) ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/cpu_cntl.cyclo ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/cpu_cntl.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/cpu_cntl.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/cpu_cntl.su ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/dispatch.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/dispatch.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/exc_hdr.cyclo ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/exc_hdr.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/exc_hdr.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/exc_hdr.su ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/int_asm.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/int_asm.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/interrupt.cyclo ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/interrupt.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/interrupt.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/interrupt.su ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_hdl.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_hdl.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_main.cyclo ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_main.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_main.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/reset_main.su ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/vector_tbl.cyclo ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/vector_tbl.d ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/vector_tbl.o ./mtk3_bsp2/mtkernel/kernel/sysdepend/cpu/core/rxv2/vector_tbl.su

.PHONY: clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-sysdepend-2f-cpu-2f-core-2f-rxv2

