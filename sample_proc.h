#pragma once
#include "config.h"
#include "arm_math.h"

#define FRAME_OVERLAP 100
//OVERLAP < DATA_COL, compile time check
uint8_t check[FRAME_OVERLAP < DATA_COL ? 1:-1];
#define COL_PROC CEILING(DATA_COL, (DATA_COL-FRAME_OVERLAP)) //This many 1d arrays(frames) will be processed at most, for given FRAME_OVERLAP

inline void hamming_init();
void sample_proc(int16_t _sample);
