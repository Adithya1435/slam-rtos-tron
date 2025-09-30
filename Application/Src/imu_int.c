
#include "imu_int.h"


extern TIM_HandleTypeDef htim6;

int16_t accel_x=0, accel_y=0, accel_z=0;
int16_t gyro_x=0, gyro_y=0, gyro_z =0;
volatile float ax=0 , ay=0 , az=0 ;

volatile float axe=0 , aye=0;
volatile float gx=0 , gy=0, gz= 0;
volatile float temp;
static float position_x = 0.0f;
static float position_y = 0.0f;
static float velocity_x = 0.0f;
static float velocity_y = 0.0f;

// Create a persistent counter outside your main loop or as a static variable (for quaternion)
static int stationary_counter = 0;
// Create a persistent counter outside your main loop or as a static variable (for  acc and vel)
static int stationary_counter1 = 0;
static int stationary_counter2 = 0;

// --- Fusion-related global variables ---
FusionAhrs ahrs;
FusionEuler euler;

const float deltaTime = (0.01f); // Corresponds to 100Hz
const float ACCEL_DRIFT_THRESHOLD = 0.5f; // Threshold in Gs. Tune this value.

void lsm6_init();
void lsm6_Read_acc(int16_t *x, int16_t *y, int16_t *z) ;
void lsm6_Read_gyro(int16_t *x, int16_t *y, int16_t *z) ;

FusionQuaternion FusionEulerToQuaternion(FusionEuler euler);

uint8_t who_am_i[1];


I2C_HandleTypeDef hi2c1;

const float gyro_offset_x = -2.758125f;
const float gyro_offset_y = -1.2890625f;
const float gyro_offset_z = 0.43625f;
const float G_TO_MS2 = 9.81f;

#define SAMPLE_RATE (100)

// Gyroscope threshold in degrees/s. If the rotation rate is below this, treat it as zero.
#define GYRO_DEADZONE 10.0f
#define ACCEL_DEADZONE 0.015f

#define ACCEL_BIAS_X 0.05f
#define ACCEL_BIAS_Y 0.02f

// Define a threshold for what you consider "stationary"
#define GYRO_STATIONARY_THRESHOLD 18.0f

// A value of 0.02g to 0.03g is a good starting point based on your data.
#define EARTH_ACCEL_DEADZONE 0.07f

// Define how many consecutive stationary readings are needed to trigger a reset
// At 100Hz (deltaTime = 0.01), 700 readings is 3.5 seconds.
#define STATIONARY_DURATION_COUNT 700

#define VELOCITY_CHECK (200/SAMPLE_RATE)

int16_t x, y, z = 0;

mat3 R_imu;
vec3 t_imu;

void TIM6_IRQHandler(void)
{

    static uint32_t tick_count = 0;


	// --- This code now runs at 100Hz (every 10ms) ---

	lsm6_Read_acc(&accel_x, &accel_y, &accel_z);
	lsm6_Read_gyro(&gyro_x, &gyro_y, &gyro_z);

	ax = (accel_x) * 0.244 / 1000;  // Sensitivity = 0.244 mg/LSB
	ay = (accel_y) * 0.244 / 1000;
	az = (accel_z) * 0.244 / 1000;
	gx = (gyro_x) * 35 / 1000;
	gy = (gyro_y) * 35 / 1000;
	gz = (gyro_z) * 35 / 1000;

	// Store original values before remapping to avoid overwriting them
	float original_ax = ax;
	float original_ay = ay;
	float original_az = az;

	// Remap accelerometer axes
	ay = original_az;  // New Y (Forward) is original Z
	ax = -original_ay; // New X (Right) is inverted original Y (Left)
	az = -original_ax; // New Z (Up) is inverted original X (Down)

	// Do the exact same remapping for the gyroscope
	float original_gx = gx;
	float original_gy = gy;
	float original_gz = gz;

	gy = original_gz;
	gx = -original_gy;
	gz = -original_gx;

	// Gyroscope Deadzone
	if (fabsf(gx) < GYRO_DEADZONE) {
		gx = 0.0f;
	}
	if (fabsf(gy) < GYRO_DEADZONE) {
		gy = 0.0f;
	}
	if (fabsf(gz) < GYRO_DEADZONE) {
		gz = 0.0f;
	}

	/*
	 * Fusion Library
	 */
	const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
	const FusionVector gyroscopeOffset = {-1.000000f, -1.000000f, 0.000000f};
	const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
	const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};

	FusionVector gyroscope = {gx, gy, gz};
	FusionVector accelerometer = {ax, ay, az};

	gyroscope = FusionCalibrationInertial(gyroscope, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
	accelerometer = FusionCalibrationInertial(accelerometer, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);

	// Perform the AHRS algorithm update
	FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, deltaTime);

	// Get the quaternion representing the sensor's orientation
	FusionQuaternion quaternion = FusionAhrsGetQuaternion(&ahrs);

	// Get the orientation as Euler angles
	euler = FusionQuaternionToEuler(quaternion);

	//Gyroscope stationary detection
	if ((fabsf(gx) < GYRO_STATIONARY_THRESHOLD) &&
		(fabsf(gy) < GYRO_STATIONARY_THRESHOLD) &&
		(fabsf(gz) < GYRO_STATIONARY_THRESHOLD))
	{
		stationary_counter++;
	}
	else
	{
		stationary_counter = 0;
	}

	if (stationary_counter >= STATIONARY_DURATION_COUNT)
	{
		euler.angle.yaw = 0.0f;
		FusionAhrsReset(&ahrs);
		stationary_counter = 0;
	}

	FusionVector earthAcceleration = FusionAhrsGetEarthAcceleration(&ahrs);


	if (fabsf(earthAcceleration.axis.x - (-original_ay)) > ACCEL_DRIFT_THRESHOLD) {
		axe = 0.0f;
	} else {
		// The value seems reasonable, use it.
		axe = earthAcceleration.axis.x;
	}

	if (fabsf(earthAcceleration.axis.y - (-original_az)) > ACCEL_DRIFT_THRESHOLD) {
		aye = 0.0f;
	} else {
		// The value seems reasonable, use it.
		aye = earthAcceleration.axis.y;
	}

	// Acceleration Deadzone
	if (fabsf(ax) < EARTH_ACCEL_DEADZONE) {
	  ax = 0.0f;
	}
	if (fabsf(ay) < EARTH_ACCEL_DEADZONE) {
	  ay = 0.0f;
	}

	// Earth acceleration deadzone
	if (fabsf(az) < EARTH_ACCEL_DEADZONE) {
	  az = 0.0f;
	}

	//Angle correction w.r.t earth frame

	if ((az < 1.015f) && ((fabsf(ax) > 0.1f) || (fabsf(ay) > 0.1f)))
	{
		ax = 0.0f;
		ay = 0.0f;
	}
	float sign_of_ax = copysignf(1.0f, ax);
	float sign_of_ay = copysignf(1.0f, ay);
	ax = sign_of_ax* sqrt(fabsf(ax)*fabsf(axe));
	ay = sign_of_ay* sqrt(fabsf(ay)*fabsf(aye));


	//ZUPT Creation
	if (fabsf(ax) < EARTH_ACCEL_DEADZONE) {
		stationary_counter1++;
	} else {
		stationary_counter1 = 0;
	}

	if (stationary_counter1 > VELOCITY_CHECK) {
		velocity_x = 0.0f;
	}

	if (fabsf(ay) < EARTH_ACCEL_DEADZONE) {
		stationary_counter2++;
	} else {
		stationary_counter2 = 0;
	}

	if (stationary_counter2 > VELOCITY_CHECK) {
		velocity_y = 0.0f;
	}

	float yaw_rad = euler.angle.yaw * (float)M_PI / 180.0f;
	float cos_yaw = cosf(yaw_rad);
	float sin_yaw = sinf(yaw_rad);



	float world_ax = (ax * cos_yaw) - (ay * sin_yaw);
	float world_ay = (ax * sin_yaw) + (ay * cos_yaw);

	 R_imu.m[0][0] = cos_yaw;
	 R_imu.m[0][2] = -1.0 * sin_yaw;
	 R_imu.m[1][0] = sin_yaw;
	 R_imu.m[1][2] = cos_yaw;

	float world_ax_ms2 = world_ax * G_TO_MS2;
	float world_ay_ms2 = world_ay * G_TO_MS2;

	velocity_x += world_ax_ms2 * deltaTime;
	velocity_y += world_ay_ms2 * deltaTime;

	position_x += velocity_x * deltaTime;
	position_y += velocity_y * deltaTime;

	t_imu.x = position_x;
	t_imu.z = position_y;

	HAL_TIM_IRQHandler(&htim6);

}

// This is the helper function that converts Euler angles back to a Quaternion.
FusionQuaternion FusionEulerToQuaternion(FusionEuler euler) {
    // 1. Convert degrees to radians and halve the angles
    const float halfRoll = euler.angle.roll * 0.5f * (float)M_PI / 180.0f;
    const float halfPitch = euler.angle.pitch * 0.5f * (float)M_PI / 180.0f;
    const float halfYaw = euler.angle.yaw * 0.5f * (float)M_PI / 180.0f;

    // 2. Pre-calculate the sine and cosine of the halved angles
    const float cosHalfRoll = cosf(halfRoll);
    const float sinHalfRoll = sinf(halfRoll);
    const float cosHalfPitch = cosf(halfPitch);
    const float sinHalfPitch = sinf(halfPitch);
    const float cosHalfYaw = cosf(halfYaw);
    const float sinHalfYaw = sinf(halfYaw);

    // 3. Apply the standard conversion formula
    FusionQuaternion result;
    result.element.w = cosHalfRoll * cosHalfPitch * cosHalfYaw + sinHalfRoll * sinHalfPitch * sinHalfYaw;
    result.element.x = sinHalfRoll * cosHalfPitch * cosHalfYaw - cosHalfRoll * sinHalfPitch * sinHalfYaw;
    result.element.y = cosHalfRoll * sinHalfPitch * cosHalfYaw + sinHalfRoll * cosHalfPitch * sinHalfYaw;
    result.element.z = cosHalfRoll * cosHalfPitch * sinHalfYaw - sinHalfRoll * sinHalfPitch * cosHalfYaw;

    return result;
}


void lsm6_init()
{
//	CUSTOM_ISM330DLC_0_I2C_Init();
//	  * @param  DevAddr Device address on Bus.
//	  * @param  Reg    The target register address to write
//	  * @param  pData  The target register value to be written
//	  * @param  Length buffer size to be written

	BSP_I2C1_ReadReg(0x6A<<1 ,0x0F,(uint8_t *) who_am_i,1);

	if( who_am_i[0] != 0x6A)
	{
		while(1);
	}

	/*Reset the sensor AND   <set auto increment>  */

	uint8_t CTRL3_C = 0x01;
	BSP_I2C1_WriteReg(0x6A<<1,0x12,(uint8_t *) &CTRL3_C,1);

	//control register CTRL_XL

	uint8_t CTRL1_XL = 0x7C ;
	BSP_I2C1_WriteReg(0x6A<<1 ,0x10,(uint8_t *) &CTRL1_XL,1);

	//control register CTRL8_XL

	uint8_t CTRL8_XL = 0xA8 ;
	BSP_I2C1_WriteReg(0x6A<<1 ,0x17,(uint8_t *) &CTRL8_XL,1);

	//control register CTRL4_C

	uint8_t CTRL4_C = 0x02 ;
	BSP_I2C1_WriteReg(0x6A<<1 ,0x13,(uint8_t *) &CTRL4_C,1);

	//control register CTRL6_C

	uint8_t CTRL6_C= 0x02 ;
	BSP_I2C1_WriteReg(0x6A<<1 ,0x15,(uint8_t *) &CTRL6_C,1);

	//control register CTRL_G

	uint8_t CTRL2_G = 0x48 ;
	BSP_I2C1_WriteReg(0x6A<<1 ,0x11,(uint8_t *) &CTRL2_G,1);

}

void lsm6_Read_acc(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data_r[6];  // 6 bytes (X_L, X_H, Y_L, Y_H, Z_L, Z_H)

    for(int16_t b=0x28; b<0x2E; b++) {

    	BSP_I2C1_ReadReg(0x6A<<1,b, (uint8_t *)&data_r[b-0x28], 1);

    }

    *x = (int16_t)(data_r[1] << 8 | data_r[0]);
    *y = (int16_t)(data_r[3] << 8 | data_r[2]);
    *z = (int16_t)(data_r[5] << 8 | data_r[4]);
}

void lsm6_Read_gyro(int16_t *x, int16_t *y, int16_t *z)
{

	uint8_t data_r[6]; // 6 bytes (X_L, X_H, Y_L, Y_H, Z_L, Z_H)


	for(int16_t b=0x22; b<0x28; b++) {
		BSP_I2C1_ReadReg(0x6A<<1,b, (uint8_t *)&data_r[b-0x22], 1);
	}

	// Convert to 16-bit values
    *x = (int16_t)(data_r[1] << 8 | data_r[0]);
	*y = (int16_t)(data_r[3] << 8 | data_r[2]);
	*z = (int16_t)(data_r[5] << 8 | data_r[4]);
}

void IMU_Init(void){
	BSP_I2C1_Init();

    lsm6_init();


	// These values should be determined from a calibration process
	// We are using the gyroscope offsets we calculated from your sample data.
	const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	const FusionVector gyroscopeSensitivity = {2.5f, 2.5f, 2.5f};
	const FusionVector gyroscopeOffset = {-1.000000f, -1.000000f, 0.000000f}; // ** YOUR CALCULATED OFFSETS **

	// For simplicity, we assume the accelerometer is perfectly calibrated.
	const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
	const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};

	// --- 2. INITIALISE THE AHRS ALGORITHM ---

	FusionAhrsInitialise(&ahrs);

	// Set AHRS algorithm settings
	const FusionAhrsSettings settings = {
        .convention = FusionConventionEnu, // North-West-Up coordinate convention
        .gain = 0.5f,                      // Proportional-Integral gain
        .accelerationRejection = 10.0f,    // Rejects acceleration greater than 10 g
        .recoveryTriggerPeriod =  5*SAMPLE_RATE, // Automatically recovers from magnetic errors in 5 seconds
	
    };
	FusionAhrsSetSettings(&ahrs, &settings);
}




void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
