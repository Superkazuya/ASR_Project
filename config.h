#pragma once
#include "stm32f4xx_conf.h"
#include "arm_math.h"

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

//#define CEILING(x, y) (((x)+(y)-1)/(y))
//#define COL_PROC CEILING(DATA_COL, (DATA_COL-FRAME_OVERLAP)) //This many 1d arrays(frames) will be processed at most, for given FRAME_OVERLAP

#define SAMPLING_FREQZ 8000
#define VOLUME 40

#define NUM_SAMPLES 256
#define DATA_COL NUM_SAMPLES //must be multiple of OUT_BUFSIZE
#define DATA_ROW 50
#define FRAME_OVERLAP 100
#define RECI_DECIMATION 64
#define RECORD_I2S_FS (SAMPLING_FREQZ*RECI_DECIMATION/16/2)
#define OUT_BUFSIZE (SAMPLING_FREQZ/1000) //uint16_t
#define RAW_BUFSIZE (SAMPLING_FREQZ*RECI_DECIMATION/8000/2) //uint16_t
//OVERLAP < DATA_COL, compile time check
uint8_t check[FRAME_OVERLAP < DATA_COL ? 1:-1];
uint8_t check1[OUT_BUFSIZE < DATA_COL ? 1:-1];


enum STATUS
{
  STATUS_IDLE = 0,
  STATUS_RAWBUF1_FULL = 1,
  STATUS_RAWBUF2_FULL = 1 << 1,
  STATUS_RAWBUF_UNDERRUN = STATUS_RAWBUF1_FULL | STATUS_RAWBUF2_FULL,
  STATUS_LAST
};

//global variables
extern float32_t buffer[DATA_ROW][DATA_COL];
extern uint16_t raw_buffer1[RAW_BUFSIZE];
extern uint16_t raw_buffer2[RAW_BUFSIZE];
extern uint16_t* raw_buffer;

extern uint16_t buff[OUT_BUFSIZE];
__IO uint8_t status;
