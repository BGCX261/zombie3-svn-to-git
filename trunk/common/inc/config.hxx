#pragma once

#define BITS_PER_SAMPLE		(16)		//To minimize processor usage, capture sounds in a 16-bit mono format.
#define SAMPLE_RATE			(8000)
#define	CHANNEL_NB			(1)
#define DURATION			(20)		// in ms
#define BLOCK_SIZE			(BITS_PER_SAMPLE*SAMPLE_RATE*CHANNEL_NB*DURATION/(1000*8))
#define FRAME_SIZE			(BLOCK_SIZE/2)

#define PLAYER_BLOCK_NB		(10)
#define TOPLAY_BLOCK_NB		(10)
#define RECORDER_BLOCK_NB	(10)
#define RECORDED_BLOCK_NB	(10)

//////////////////////////////////////////speex///////////
#define SPEEX_QUALITY		(8)
#define SPEEX_AGC_LEVEL		(16000)		// actually default is 8000(0,32768)
//#define SPEEX_AEC
#define SPEEX_CODE
#define SPEEX_PREPROCESS
//#define USING_JITTER

////////////////////////////////////////AXM converter////////
#define ENCODE_FORMAT		WAVE_FORMAT_GSM610

////////////////////////////////////////DSound Effects//////
#define DSOUND_AEC


///////////////////////////////////////Video////////////////
#define VIDEO_INTERVAL 1000
#define TIMER_ID		1

#define CAP_BLOCK_NUM		4
#define RENDER_BLOCK_NUM	4
//#define CAM_WIDTH  320
//#define CAM_HEIGHT 240
//#define CAM_BITS   24
#define CAM_WIDTH		160
#define CAM_HEIGHT		120
#define CAM_IMAGEBITS	3
#define CAM_BITS		24
#define IMAGE_LEN		(CAM_WIDTH * CAM_HEIGHT * CAM_IMAGEBITS)