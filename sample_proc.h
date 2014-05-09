#pragma once
#include "config.h"
#include "arm_math.h"
#include "pdm_filter.h"

#define FRAME_OVERLAP 100
#define RECI_DECIMATION 64
#define OUT_FREQZ 16000
#define OUT_BUFSIZE (OUT_FREQZ/1000) //uint16_t
#define RAW_BUFSIZE (OUT_FREQZ*RECI_DECIMATION/8000/2) //uint16_t
//OVERLAP < DATA_COL, compile time check
uint8_t check[FRAME_OVERLAP < DATA_COL ? 1:-1];
#define COL_PROC CEILING(DATA_COL, (DATA_COL-FRAME_OVERLAP)) //This many 1d arrays(frames) will be processed at most, for given FRAME_OVERLAP

PDMFilter_InitStruct pdm;

inline void hamming_init();
inline void filter_init();
void sample_proc(int16_t _sample);
