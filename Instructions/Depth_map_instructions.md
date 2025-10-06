# Depth Map Test (depth_map_rtos.bin)

## Objective
This binary file demonstrates real-time depth estimation and visualization using the onboard camera and LCD display under TRON's RTOS environment. It's to independently showcase our progress in the design of our custom ML model that we trained to make the real-time depth map model run on the microcontroller board. This is used for replacing the conventional LIDAR data in the final SLAM system which we were unfortunately not able to finish due to time constraints. This system estimates object depth based on camera input, applies post-processing filters, and displays the post-processed depth map on the LCD screen.

---

## 1. Flashing Instructions
1. Flash the `depth_map_rtos.bin` file to the STM32 board using **STM32CubeProgrammer** after signing it as mentioned below.

**(Flash Application Command):**
```bash
STM32_Programmer_CLI -c port=SWD mode=HOTPLUG -el MX66UW1G45G_STM32N6570-DK.stldr -hardRst -w depth_map_rtos.bin 0x70100000

```

2. Connect the board to your PC via USB and ensure power is supplied.  
3. Once flashing is complete, reset or power-cycle the board to begin execution.

---

## 2. Test Procedure

### 1. Camera Placement
1. Keep the camera near some objects under well-lit conditions.  
2. Ensure that the environment has sufficient and uniform lighting for accurate depth estimation.

### 2. Depth Estimation and Display
3. As you move the camera closer or farther from the objects, the system estimates the depth in real time.  
4. The captured image is processed through the RTOS pipeline, and the resulting depth map is displayed on the LCD screen.

---

## 3. Expected Output
- The LCD screen will display a live visual depth map of the scene captured by the camera.  
- Depth variations will appear as intensity or color differences corresponding to object proximity.  
- Smoother and more accurate depth transitions will be observed under stable lighting and slow camera motion.
