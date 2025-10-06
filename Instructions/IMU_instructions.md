# IMU Dead Reckoning Test (imu.bin)

## 1. Flashing Instructions
1. Flash the `imu.bin` file to the STM32 board using **STM32CubeProgrammer** after signing it as mentioned below.

(Flash Application Command):

```bash
STM32_Programmer_CLI -c port=SWD mode=HOTPLUG -el MX66UW1G45G_STM32N6570-DK.stldr -hardRst -w imu_int.bin 0x70100000

```

2. Connect the board to your PC via USB and ensure power is supplied.  
3. Once flashing is complete, reset or power-cycle the board to begin execution.

---

## 2. Objective
This binary file demonstrates the real-time dead reckoning capability of the Inertial Measurement Unit (IMU) mounted on the BCAMS-IMX module (ISM330DLC). This application mainly shows the independent progress in the localization algorithm that we wanted to integrate into our final project, which we weren't able to do due to time constraints. It outputs the estimated 2D position (x, y) and yaw angle of the device based on the fused accelerometer and gyroscope data, including the custom Zero-Velocity Update (ZUPT) implementation for drift correction.

---

## 3. Viewing the Serial Output
To observe the data stream, you will need a serial terminal application like PuTTY.

1. Connect the board to your PC via USB.  
2. Identify the COM port assigned to the board (you can find this in your system's Device Manager).  
3. Launch PuTTY and configure the session with the following settings:  
   - **Session → Connection type:** Serial  
   - **Serial → Serial line:** Your board's COM port (e.g., COM3)  
   - **Serial → Speed (baud):** 115200  
4. Click **Open** to begin monitoring the data stream.

---

## 4. Test Procedure
Follow these steps to test the IMU functionality correctly. It is important to keep the device as horizontal as possible throughout the test.

### 4.1. Initialization
1. Place the device on a flat surface with the camera facing forward.  
2. To engage the sensor fusion algorithm, perform small, slow rotations (wiggles of about 1 degree) to the left and right.  
3. You will see the yaw angle value on the serial monitor begin to respond to the movement.

### 4.2. Motion Tracking
4. Move the device around the horizontal plane.  
5. For optimal tracking, primarily rotate the device around its vertical (yaw) axis, keeping the camera facing forward as it turns.

### 4.3. Zero-Velocity Update (ZUPT)
6. Periodically, bring the device to a complete stop for a few seconds.  
7. This action triggers the custom ZUPT algorithm, which automatically resets accumulated velocity and acceleration biases to zero. This is a key feature for reducing long-term drift.

---

## 5. Expected Output
The serial terminal will display a continuous stream of comma-separated values in the following format:
```
position_x, position_y, euler.angle.yaw
```

```
### Example Output:
10.52,-5.18,25.3  
10.61,-5.20,25.4  
10.70,-5.22,25.4
```
