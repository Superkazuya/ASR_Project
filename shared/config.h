#pragma once
#include "stm32f4xx_conf.h"
#include "arm_math.h"
#include "stm32f4_discovery_audio_codec.h"
#include "stm32f4_discovery.h"

//gpio pin settings
#define SPI_SCK_GPIO_CLK RCC_AHB1Periph_GPIOB
#define SPI_SCK_GPIO_PORT GPIOB
#define SPI_SCK_PIN GPIO_Pin_10
#define SPI_SCK_SOURCE GPIO_PinSource10
#define SPI_SCK_AF GPIO_AF_SPI2

#define SPI_MOSI_GPIO_CLK RCC_AHB1Periph_GPIOC
#define SPI_MOSI_GPIO_PORT GPIOC
#define SPI_MOSI_PIN GPIO_Pin_3
#define SPI_MOSI_SOURCE GPIO_PinSource3
#define SPI_MOSI_AF GPIO_AF_SPI2

#define MAX(x,y) ((x) > (y) ? (x):(y))
#define MIN2(x,y) ((x) < (y) ? (x):(y))
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define CEILING(x, y) (((x)+(y)-1)/(y))

#define ROW_PROC CEILING(DATA_COL-1, FRAME_SHIFT) //This many 1d arrays(frames) will be processed at most, for given FRAME_OVERLAP

#define SAMPLING_FREQZ 16000
#define VOLUME 60


#define FRAME_SIZE 256
#define DATA_COL FRAME_SIZE //must be multiple of OUT_BUFSIZE
#define DATA_ROW ROW_PROC
#define FRAME_OVERLAP 64
#define FRAME_SHIFT (FRAME_SIZE-FRAME_OVERLAP)
#define NUM_FRAME CEILING((MAX_BUF_SIZE-FRAME_OVERLAP), FRAME_SHIFT)//total # of frames of recorded data
#define RECI_DECIMATION 64
#define RECORD_I2S_FS (SAMPLING_FREQZ*RECI_DECIMATION/16/2)
#define OUT_BUFSIZE (SAMPLING_FREQZ/1000) //uint16_t
#define RAW_BUFSIZE (SAMPLING_FREQZ*RECI_DECIMATION/8000/2) //uint16_t
//pre-processed buffer
#define MAX_BUF_SIZE (3*16000)

//FRAM_SHIFT <= DATA_COL, compile time check
uint8_t check[FRAME_SHIFT <= DATA_COL ? 1:-1];
uint8_t check1[OUT_BUFSIZE < DATA_COL ? 1:-1];


enum RAWBUFF_STATUS
{
  RAWBUF_IDLE = 0,
  RAWBUF_FULL1 = 1,
  RAWBUF_FULL2 = 1 << 1,
  RAWBUF_UNDERRUN = RAWBUF_FULL1 | RAWBUF_FULL2,
  RAWBUF_LAST
};
//global variables
//raw PDM data
extern uint16_t raw_buffer1[RAW_BUFSIZE];
extern uint16_t raw_buffer2[RAW_BUFSIZE];
extern uint16_t* raw_buffer;
__IO uint8_t rawbuf_status;
//raw PCM data
//extern uint16_t buff[OUT_BUFSIZE];

//framed PCM data
extern uint16_t frame[DATA_ROW][DATA_COL]; 
//speech data
extern uint16_t data[MAX_BUF_SIZE];

//mfcc configuration
#define FFT_SIZE 512 //FFT_SIZE >= FRAME_SIZE is recommanded
#define DCT_SIZE 128

#define NUM_FILTER_BANKS 26
#define FREQ_LB 300
#define FREQ_HB 20000
#define DCT_LOW 2
#define DCT_HIGH 13
#define DCT_DIGIT (DCT_HIGH-DCT_LOW+1)

//dual-threshold end point detection settings
#define DIFF 655
#define DETECTION_LENGTH OUT_BUFSIZE
#define ZCR_THRESHOLD_LOW (DETECTION_LENGTH*0.05)
#define ZCR_THRESHOLD_HIGH (DETECTION_LENGTH*0.10)
#define ENG_THRESHOLD_LOW (DETECTION_LENGTH*6000)
#define ENG_THRESHOLD_HIGH (DETECTION_LENGTH*12000)

/*
#include "usb_hcd_int.h"
#include "usbh_usr.h"
#include "usbh_core.h"
#include "usbh_msc_core.h"
*/

float32_t feature_vec[NUM_FRAME][DCT_DIGIT];
