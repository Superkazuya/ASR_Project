#include "sample_proc.h"
#include "mfcc.h"

static float32_t Hamming[NUM_SAMPLES];
//extern float32_t buffer[DATA_ROW][DATA_COL];
uint16_t row_idx = 0;
uint16_t col_idx = 0;


/*
void filter_init()
{
  filter.LP_HZ = 8000;
  filter.HP_HZ = 10;
  filter.Fs = 16000;
  filter.Out_MicChannels = 1;
  filter.In_MicChannels = 1;
  PDM_Filter_Init(&filter);
}
*/

inline void hamming_init()
{
  uint16_t i;
  for(i = 0; i < NUM_SAMPLES; ++i)
    Hamming[i] = 0.54-0.46*arm_cos_f32((2*PI*i)/(NUM_SAMPLES-1));
}

void sample_proc(int16_t _sample)
{
  if(col_idx >= DATA_COL)
  {
    mfcc(buffer[row_idx], buffer[row_idx]);
    col_idx -= FRAME_OVERLAP;
    row_idx++;
  }
  if(row_idx >= DATA_ROW)
  {
    row_idx = 0;
    col_idx = 0;
    //signal ready
  }

  uint16_t row = row_idx;
  uint16_t col = col_idx;
  while(col >= 0 && row <= DATA_ROW)
  {
    buffer[row][col] = _sample*Hamming[col];
    col -= FRAME_OVERLAP;
    row++;
  }
  col_idx++;
}
