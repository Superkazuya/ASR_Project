#pragma once
#include "config.h"
#include "arm_math.h"
#include "pdm_filter.h"
PDMFilter_InitStruct pdm;

inline void hamming_init();
inline void filter_init();
void sample_proc(int16_t _sample);
