#include <tk/tkernel.h>
#include <tm/tmonitor.h>

#include "nnlib.h"
#include "imu_int.h"
#include "data_structs.h"
#include "intrinsics.h"
#include "init_map/init_map.h"
#include "track_frames.h"

#define MAX_WORLD_POINTS 1000

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB fr1[224 * 224 *3];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB fr2[224 * 224 *3];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB d1[224 * 224];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB d2[224 * 224];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB gray_f1[224 * 224];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB gray_f2[224 * 224];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB nn_gray[224 * 224];

__attribute__ ((section (".psram_bss")))
__attribute__ ((aligned (32)))
UB nn_prev_gray[224 * 224];


volatile VW cameraFrameReceived;
Img frame1, frame2;
Img nn_gray_frame, nn_prev_gray_frame;
Pose currPose;
B world_points_count;
UB status;


LOCAL void task_1(INT stacd, void *exinf);	// task execution function
LOCAL ID	tskid_1;			// Task ID number
LOCAL T_CTSK ctsk_1 = {				// Task creation information
	.itskpri	= 10,
	.stksz		= 8192,
	.task		= task_1,
	.tskatr		= TA_HLNG | TA_RNG3,
};

LOCAL void task_1(INT stacd, void *exinf)
{

	Mem_init();

	LCD_init();

	IMU_Init();

	/* NN Init */
	UW pitch_nn = 0;
	UW nn_in_len = 0;
	B number_output = 0;
	uint8_t *nn_out[MAX_NUMBER_OUTPUT] = {0};
	VW nn_out_len[MAX_NUMBER_OUTPUT] = {0};
	NeuralNetwork_init(&nn_in_len, nn_out, &number_output, nn_out_len);

	/* Post Processing Init */
	pp_input = (UB *) nn_out[0];

	/* Camera Init */
	CameraPipeline_Init(&lcd_bg_area.XSize, &lcd_bg_area.YSize, &pitch_nn);

	B number_cam_recv = 0;
	vec3 CURRENT_WORLD_POINTS[MAX_WORLD_POINTS];
	int num_matches_1 = 0;
	B flg = 0;


	/* Not used */
	CameraPipeline_DisplayPipe_Start(lcd_bg_buffer, CMW_MODE_CONTINUOUS);

	while (1){
		CameraPipeline_IspUpdate();


		if (pitch_nn != (NN_WIDTH * NN_BPP)){
			/* Start NN camera single capture Snapshot */
			CameraPipeline_NNPipe_Start(dcmipp_out_nn, CMW_MODE_SNAPSHOT);
		}
		else{
			/* Start NN camera single capture Snapshot */
			CameraPipeline_NNPipe_Start(nn_in, CMW_MODE_SNAPSHOT);
		}


		while (cameraFrameReceived == 0) {};
		cameraFrameReceived = 0;


		for (W i = 0; i < 224*224; i++) {
			 UB r = nn_in[i*3 + 0];
			 UB g = nn_in[i*3 + 1];
			 UB b = nn_in[i*3 + 2];
			 nn_gray[i] = (uint8_t)(0.299*r + 0.587*g + 0.114*b);
		}
		nn_gray_frame.pixels = nn_gray;
		nn_gray_frame.w = 224;
		nn_gray_frame.h = 224;

		//UW ts[2] = { 0 };

		if (pitch_nn != (NN_WIDTH * NN_BPP))
		{
			SCB_InvalidateDCache_by_Addr(dcmipp_out_nn, sizeof(dcmipp_out_nn));
			img_crop(dcmipp_out_nn, nn_in, pitch_nn, NN_WIDTH, NN_HEIGHT, NN_BPP);
			SCB_CleanInvalidateDCache_by_Addr(nn_in, nn_in_len);
		}


		/* run ATON inference */
		Run_Inference();

		if(number_cam_recv == 55){
			UB *pfr1 = (UB *)&fr1;
			if (pfr1 != NULL) {
				memcpy(pfr1, nn_in, 224*224*3);
			}

			int total_pixels = 224 * 224;
			for (W i = 0; i < total_pixels; i++) {
			     UB r = pfr1[i*3 + 0];
			     UB g = pfr1[i*3 + 1];
			     UB b = pfr1[i*3 + 2];
			     gray_f1[i] = (uint8_t)(0.299*r + 0.587*g + 0.114*b);
			}

			frame1.pixels = gray_f1;
			frame1.w = 224;
			frame1.h = 224;

			UB *pd1 = (UB *)&d1;
			if (pd1 != NULL) {
				memcpy(pd1, pp_input, 224*224);
			}
		}


		if(number_cam_recv == 56){
			UB *pfr2 = (UB *)&fr2;
			if (pfr2 != NULL) {
				memcpy(pfr2, nn_in, 224*224*3);
			}

			int total_pixels = 224 * 224;
			for (W i = 0; i < total_pixels; i++) {
				 UB r = pfr2[i*3 + 0];
				 UB g = pfr2[i*3 + 1];
				 UB b = pfr2[i*3 + 2];
				 gray_f2[i] = (uint8_t)(0.299*r + 0.587*g + 0.114*b);
			}

			frame2.pixels = gray_f2;
			frame2.w = 224;
			frame2.h = 224;

			UB *pd2 = (UB *)&d2;
			if (pd2 != NULL) {
				memcpy(pd2, pp_input, 224*224);
			}

			init_map(&frame1, &frame2, K_MAT, &num_matches_1, CURRENT_WORLD_POINTS, (int) MAX_WORLD_POINTS, &currPose, "no", NULL, NULL, NULL, NULL);
			world_points_count = num_matches_1;

			flg = 1;
		}

		for(UW i=0; i < MAX_WORLD_POINTS; i++){
			if(!(CURRENT_WORLD_POINTS->x < 1e-6 && CURRENT_WORLD_POINTS->y < 1e-6 && CURRENT_WORLD_POINTS->z < 1e-6)){
				world_points_count++;
			}
		}


		if(flg){
			tracking_thread(CURRENT_WORLD_POINTS, K_MAT, &nn_gray_frame, &nn_prev_gray_frame, &currOptimalPose, world_points_count, &status);
		}

		number_cam_recv = (number_cam_recv > 57) ? 58 : (number_cam_recv+1);

		//UTIL_LCDEx_PrintfAtLine(0, "%d", num_matches_1);
		//UTIL_LCDEx_PrintfAtLine(2, "%d", k);

		process_depth_map();

		display_depth_map();
		//BSP_LCD_FillRGBRect(0, 0, 0, lcd_bg_buffer, 640, 480);


		for (W i = 0; i < 224*224; i++) {
			 nn_prev_gray[i] = nn_gray[i];
		}
		nn_prev_gray_frame.pixels = nn_prev_gray;
		nn_prev_gray_frame.w = 224;
		nn_prev_gray_frame.h = 224;



		for (B i=0; i<number_output; i++)
		{
			uint8_t *tmp = nn_out[i];
			SCB_InvalidateDCache_by_Addr(tmp, nn_out_len[i]);
		}
  }

}

EXPORT INT usermain(void)
{
	/* Create & Start Tasks */
	tskid_1 = tk_cre_tsk(&ctsk_1);
	tk_sta_tsk(tskid_1, 0);

	tk_slp_tsk(TMO_FEVR);

	return 0;
}
