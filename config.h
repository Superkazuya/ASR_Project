#pragma once
#include "stm32f4xx_conf.h"
#include "arm_math.h"

//#define CEILING(x, y) (((x)+(y)-1)/(y))

#define SAMPLING_FREQZ 16000
#define VOLUME 40

#define NUM_SAMPLES 512
#define DATA_COL NUM_SAMPLES
#define DATA_ROW 16
#define FRAME_OVERLAP 100
#define RECI_DECIMATION 64
#define RECORD_I2S_FS (SAMPLING_FREQZ*RECI_DECIMATION/16/2)
#define OUT_BUFSIZE (SAMPLING_FREQZ/1000) //uint16_t
#define RAW_BUFSIZE (SAMPLING_FREQZ*RECI_DECIMATION/8000/2) //uint16_t
//OVERLAP < DATA_COL, compile time check
uint8_t check[FRAME_OVERLAP < DATA_COL ? 1:-1];
uint8_t check1[OUT_BUFSIZE < DATA_COL ? 1:-1];
//#define COL_PROC CEILING(DATA_COL, (DATA_COL-FRAME_OVERLAP)) //This many 1d arrays(frames) will be processed at most, for given FRAME_OVERLAP


enum STATUS
{
  STATUS_RAW_BUF_FULL,
  STATUS_RAW_BUF_UNDERRUN,
  STATUS_LAST
};

enum OPERATIONS
{
  OP_IDLE = 0,
  OP_RAWBUF_FULL = 1 << STATUS_RAW_BUF_FULL,
  OP_RAWBUF_UR = 1 << STATUS_RAW_BUF_UNDERRUN,
  OP_EXIT,
  OP_LAST
};

//global variables
extern float32_t buffer[DATA_ROW][DATA_COL];
extern __IO uint16_t status;

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



