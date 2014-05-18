#include "sample_proc.h"
#include "mfcc.h"

uint16_t frame[DATA_ROW][DATA_COL] = { {0} };
float32_t feature_vec[NUM_FRAME*DCT_DIGIT] = {0};
static float32_t Hamming[FRAME_SIZE];

//static uint16_t zero_cross[DATA_COL] = {0};



void SPI2_IRQHandler(void)
{
  if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) == SET)
    sample_proc(SPI_I2S_ReceiveData(SPI2));
}

//called in the interrupt
void sample_proc(int16_t _sample)
{
  static uint16_t i = 0;
  raw_buffer[i++] = HTONS(_sample);

  if(i < RAW_BUFSIZE)
    return;
  i = 0;
  if(raw_buffer == raw_buffer1)
  {
    rawbuf_status |= RAWBUF_FULL1;
    raw_buffer = raw_buffer2;
  }
  else
  {
    rawbuf_status |= RAWBUF_FULL2;
    raw_buffer = raw_buffer2;
  }
}

void filter_init()
{
  pdm.LP_HZ = 8000;
  pdm.HP_HZ = 10;
  pdm.Fs = SAMPLING_FREQZ;
  pdm.Out_MicChannels = 1;
  pdm.In_MicChannels = 1;
  PDM_Filter_Init(&pdm);
}

inline void hamming_init()
{
  uint16_t i;
  for(i = 0; i < FRAME_SIZE; ++i)
    Hamming[i] = 0.54-0.46*arm_cos_f32((2*PI*i)/(FRAME_SIZE-1));
}

void enframe(uint16_t* _data, uint16_t _frame_num)
{
  static float32_t fdata[FFT_SIZE];//static to prevent overflow. FFT_SIZE>FRAME_SIZE
  if(_frame_num >= NUM_FRAME)
  {
    STM_EVAL_LEDOn(LED5);
    return;
  }
  uint16_t i;
  for(i = 0; i < FRAME_SIZE; ++i)
    fdata[i] = Hamming[i]*_data[i];
  memset(fdata+FRAME_SIZE, 0, (FFT_SIZE-FRAME_SIZE+1)*sizeof(float32_t));//zero padding

  mfcc(fdata, feature_vec+_frame_num*DCT_DIGIT);
  enframe(_data+FRAME_SHIFT, _frame_num+1);
}


