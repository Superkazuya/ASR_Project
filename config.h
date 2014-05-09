#pragma once
#include "stm32f4xx_conf.h"
#include "arm_math.h"

//#define CEILING(x, y) (((x)+(y)-1)/(y))

#define NUM_SAMPLES 256
#define DATA_COL NUM_SAMPLES
#define DATA_ROW 16

enum STATUS
{
  STATUS_RAW_BUF_FULL,
  SIG_ON,
  BUF_RDY,
  STATUS_LAST
};

enum OPERATION
{
  OP_IDLE = 0,
  OP_PROC = 1 << STATUS_RAW_BUF_FULL,
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


#define FREQZ 48000
#define VOLUME 50

