################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../mtk3_bsp2/mtkernel/kernel/tkernel/cpuctl.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/device.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/deviceio.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/eventflag.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/int.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/klock.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/mailbox.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/memory.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/mempfix.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/mempool.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/messagebuf.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/misc_calls.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/mutex.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/objname.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/power.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/rendezvous.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/semaphore.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/task.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/task_manage.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/task_sync.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/time_calls.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/timer.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/tkinit.c \
../mtk3_bsp2/mtkernel/kernel/tkernel/wait.c 

OBJS += \
./mtk3_bsp2/mtkernel/kernel/tkernel/cpuctl.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/device.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/deviceio.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/eventflag.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/int.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/klock.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/mailbox.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/memory.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/mempfix.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/mempool.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/messagebuf.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/misc_calls.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/mutex.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/objname.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/power.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/rendezvous.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/semaphore.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/task.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/task_manage.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/task_sync.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/time_calls.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/timer.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/tkinit.o \
./mtk3_bsp2/mtkernel/kernel/tkernel/wait.o 

C_DEPS += \
./mtk3_bsp2/mtkernel/kernel/tkernel/cpuctl.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/device.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/deviceio.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/eventflag.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/int.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/klock.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/mailbox.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/memory.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/mempfix.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/mempool.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/messagebuf.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/misc_calls.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/mutex.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/objname.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/power.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/rendezvous.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/semaphore.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/task.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/task_manage.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/task_sync.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/time_calls.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/timer.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/tkinit.d \
./mtk3_bsp2/mtkernel/kernel/tkernel/wait.d 


# Each subdirectory must supply rules for building sources it contributes
mtk3_bsp2/mtkernel/kernel/tkernel/%.o mtk3_bsp2/mtkernel/kernel/tkernel/%.su mtk3_bsp2/mtkernel/kernel/tkernel/%.cyclo: ../mtk3_bsp2/mtkernel/kernel/tkernel/%.c mtk3_bsp2/mtkernel/kernel/tkernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m55 -std=gnu11 -g3 -DSTM32N657xx -DUSE_FULL_ASSERT -DUSE_FULL_LL_DRIVER -DVECT_TAB_SRAM -DLL_ATON_DUMP_DEBUG_API -DLL_ATON_PLATFORM=LL_ATON_PLAT_STM32N6 -DLL_ATON_OSAL=LL_ATON_OSAL_BARE_METAL -DLL_ATON_RT_MODE=LL_ATON_RT_ASYNC -DLL_ATON_SW_FALLBACK -DLL_ATON_DBG_BUFFER_INFO_EXCLUDED=1 -D_STM32CUBE_DISCOVERY_N657_ -DMTKBSP_STM32CUBE -DTARGET_GRP_DIR=stm32_cube -DTARGET_CPU_DIR=stm32n6 '-DTARGET_DIR=stm32_cube/discovery_stm32n657' -c -I../../Inc -I../../../../Middlewares/AI_Runtime/Npu/ll_aton -I../../../../Middlewares/AI_Runtime/Npu/Devices/STM32N6XX -I../../../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc -I../../../../STM32Cube_FW_N6/Drivers/STM32N6xx_HAL_Driver/Inc/Legacy -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/Device/ST/STM32N6xx/Include -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/Include -I../../../../STM32Cube_FW_N6/Drivers/CMSIS/DSP/Include -I../../../../STM32Cube_FW_N6/Drivers/BSP/Components/Common -I../../../../STM32Cube_FW_N6/Drivers/BSP/STM32N6570-DK -I../../../../Middlewares/Camera_Middleware/ISP_Library/isp/Inc -I../../../../Middlewares/Camera_Middleware/ISP_Library/evision/Inc -I../../../../Middlewares/AI_Runtime/Inc -I../../../../STM32Cube_FW_N6/Utilities/lcd -I../../../../Middlewares/Camera_Middleware -I../../../../STM32Cube_FW_N6/Drivers/BSP/Components/aps256xx -I../../../../Middlewares/Camera_Middleware/sensors -I../../../../Middlewares/Camera_Middleware/sensors/imx335 -I../../../../Middlewares/Camera_Middleware/sensors/vd6g -I../../../../Middlewares/Camera_Middleware/sensors/vd55g1 -I../../../../Middlewares/Fusion/Inc -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/config" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/include" -I"C:/Users/Adi/Music/Stm32_img/STM32N6-GettingStarted-ImageClassification-main/slam_tron/Core/STM32N6570-DK/STM32CubeIDE/mtk3_bsp2/mtkernel/kernel/knlinc" -I../../../../Application/Inc -I../../../../Application/Inc/slam -I../../../../Application/Inc/slam/feature_detect -I../../../../Application/Inc/slam/feature_match -I../../../../Application/Inc/slam/init_map -I../../../../Application/Inc/slam/tracking -Oz -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -mcmse -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-tkernel

clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-tkernel:
	-$(RM) ./mtk3_bsp2/mtkernel/kernel/tkernel/cpuctl.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/cpuctl.d ./mtk3_bsp2/mtkernel/kernel/tkernel/cpuctl.o ./mtk3_bsp2/mtkernel/kernel/tkernel/cpuctl.su ./mtk3_bsp2/mtkernel/kernel/tkernel/device.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/device.d ./mtk3_bsp2/mtkernel/kernel/tkernel/device.o ./mtk3_bsp2/mtkernel/kernel/tkernel/device.su ./mtk3_bsp2/mtkernel/kernel/tkernel/deviceio.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/deviceio.d ./mtk3_bsp2/mtkernel/kernel/tkernel/deviceio.o ./mtk3_bsp2/mtkernel/kernel/tkernel/deviceio.su ./mtk3_bsp2/mtkernel/kernel/tkernel/eventflag.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/eventflag.d ./mtk3_bsp2/mtkernel/kernel/tkernel/eventflag.o ./mtk3_bsp2/mtkernel/kernel/tkernel/eventflag.su ./mtk3_bsp2/mtkernel/kernel/tkernel/int.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/int.d ./mtk3_bsp2/mtkernel/kernel/tkernel/int.o ./mtk3_bsp2/mtkernel/kernel/tkernel/int.su ./mtk3_bsp2/mtkernel/kernel/tkernel/klock.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/klock.d ./mtk3_bsp2/mtkernel/kernel/tkernel/klock.o ./mtk3_bsp2/mtkernel/kernel/tkernel/klock.su ./mtk3_bsp2/mtkernel/kernel/tkernel/mailbox.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/mailbox.d ./mtk3_bsp2/mtkernel/kernel/tkernel/mailbox.o ./mtk3_bsp2/mtkernel/kernel/tkernel/mailbox.su ./mtk3_bsp2/mtkernel/kernel/tkernel/memory.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/memory.d ./mtk3_bsp2/mtkernel/kernel/tkernel/memory.o ./mtk3_bsp2/mtkernel/kernel/tkernel/memory.su ./mtk3_bsp2/mtkernel/kernel/tkernel/mempfix.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/mempfix.d ./mtk3_bsp2/mtkernel/kernel/tkernel/mempfix.o ./mtk3_bsp2/mtkernel/kernel/tkernel/mempfix.su ./mtk3_bsp2/mtkernel/kernel/tkernel/mempool.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/mempool.d ./mtk3_bsp2/mtkernel/kernel/tkernel/mempool.o ./mtk3_bsp2/mtkernel/kernel/tkernel/mempool.su ./mtk3_bsp2/mtkernel/kernel/tkernel/messagebuf.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/messagebuf.d ./mtk3_bsp2/mtkernel/kernel/tkernel/messagebuf.o ./mtk3_bsp2/mtkernel/kernel/tkernel/messagebuf.su ./mtk3_bsp2/mtkernel/kernel/tkernel/misc_calls.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/misc_calls.d ./mtk3_bsp2/mtkernel/kernel/tkernel/misc_calls.o ./mtk3_bsp2/mtkernel/kernel/tkernel/misc_calls.su ./mtk3_bsp2/mtkernel/kernel/tkernel/mutex.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/mutex.d ./mtk3_bsp2/mtkernel/kernel/tkernel/mutex.o ./mtk3_bsp2/mtkernel/kernel/tkernel/mutex.su ./mtk3_bsp2/mtkernel/kernel/tkernel/objname.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/objname.d ./mtk3_bsp2/mtkernel/kernel/tkernel/objname.o ./mtk3_bsp2/mtkernel/kernel/tkernel/objname.su ./mtk3_bsp2/mtkernel/kernel/tkernel/power.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/power.d ./mtk3_bsp2/mtkernel/kernel/tkernel/power.o ./mtk3_bsp2/mtkernel/kernel/tkernel/power.su ./mtk3_bsp2/mtkernel/kernel/tkernel/rendezvous.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/rendezvous.d ./mtk3_bsp2/mtkernel/kernel/tkernel/rendezvous.o ./mtk3_bsp2/mtkernel/kernel/tkernel/rendezvous.su ./mtk3_bsp2/mtkernel/kernel/tkernel/semaphore.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/semaphore.d ./mtk3_bsp2/mtkernel/kernel/tkernel/semaphore.o ./mtk3_bsp2/mtkernel/kernel/tkernel/semaphore.su ./mtk3_bsp2/mtkernel/kernel/tkernel/task.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/task.d ./mtk3_bsp2/mtkernel/kernel/tkernel/task.o ./mtk3_bsp2/mtkernel/kernel/tkernel/task.su ./mtk3_bsp2/mtkernel/kernel/tkernel/task_manage.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/task_manage.d ./mtk3_bsp2/mtkernel/kernel/tkernel/task_manage.o ./mtk3_bsp2/mtkernel/kernel/tkernel/task_manage.su ./mtk3_bsp2/mtkernel/kernel/tkernel/task_sync.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/task_sync.d ./mtk3_bsp2/mtkernel/kernel/tkernel/task_sync.o ./mtk3_bsp2/mtkernel/kernel/tkernel/task_sync.su ./mtk3_bsp2/mtkernel/kernel/tkernel/time_calls.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/time_calls.d ./mtk3_bsp2/mtkernel/kernel/tkernel/time_calls.o ./mtk3_bsp2/mtkernel/kernel/tkernel/time_calls.su ./mtk3_bsp2/mtkernel/kernel/tkernel/timer.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/timer.d ./mtk3_bsp2/mtkernel/kernel/tkernel/timer.o ./mtk3_bsp2/mtkernel/kernel/tkernel/timer.su ./mtk3_bsp2/mtkernel/kernel/tkernel/tkinit.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/tkinit.d ./mtk3_bsp2/mtkernel/kernel/tkernel/tkinit.o ./mtk3_bsp2/mtkernel/kernel/tkernel/tkinit.su ./mtk3_bsp2/mtkernel/kernel/tkernel/wait.cyclo ./mtk3_bsp2/mtkernel/kernel/tkernel/wait.d ./mtk3_bsp2/mtkernel/kernel/tkernel/wait.o ./mtk3_bsp2/mtkernel/kernel/tkernel/wait.su

.PHONY: clean-mtk3_bsp2-2f-mtkernel-2f-kernel-2f-tkernel

