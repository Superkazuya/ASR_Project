#pragma once
#include "stm32f4xx_conf.h"
#include "arm_math.h"

#define CEILING(x, y) (((x)+(y)-1)/(y))

#define NUM_SAMPLES 256
#define DATA_COL NUM_SAMPLES
#define DATA_ROW 100

enum STATUS
{
  BUFFER_FULL = 0,
  DATA_RDY
};

enum EVENT
{
  PROCESSED
};

//global variables
extern float32_t buffer[DATA_ROW][DATA_COL];
extern __IO uint16_t status;
