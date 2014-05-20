#include "config.h"
#include "mfcc.h"
#include "sample_proc.h"
#include "record.h"
#include "data.h"

uint16_t raw_buffer1[RAW_BUFSIZE];
uint16_t raw_buffer2[RAW_BUFSIZE];
uint16_t* raw_buffer=raw_buffer1;
//uint16_t data[MAX_BUF_SIZE];
uint32_t dtw_mat[NUM_CLASS][NUM_FRAME+1] = {[0 ... NUM_CLASS-1][0 ... NUM_FRAME] = 9999999};
uint16_t result = 0;

extern uint32_t dtw_calc(float32_t *_vect1, uint16_t _num_frame, float32_t *_vect2, uint16_t _len2, uint32_t* _dtw);
static void event_handler();
static void raw_buffull_handler();
static void exit_handler();
static void idle_handler();
static void underrun_handler();

uint16_t max_loc(uint32_t *_val, uint16_t _length)
{
  uint32_t max = _val[0];
  uint16_t i;
  uint16_t loc = 0;
  for(i = 1; i < _length; ++i)
    if(_val[i] > max)
    {
      max = _val[i];
      loc = i;
    }
  return loc;
}

void post_proc_callback(float32_t *_feature_vec, uint16_t _frame_num)
{
  uint32_t val[NUM_CLASS];
  uint16_t i;
  for(i = 0; i < NUM_CLASS; i++)
    val[i] = dtw_calc(_feature_vec, _frame_num, (float32_t*)&fvector[i][0][0], NUM_FRAME, dtw_mat[i]);
  if(_frame_num == (NUM_FRAME-1))
    result = max_loc(val, NUM_CLASS);
}

/*
uint16_t recognition()
{
  uint16_t min_loc = 1;
  uint32_t min_val = dtw_calc(&feature_vec[0][0], NUM_FRAME, &fvector[0][0][0], NUM_FRAME);
  test[0] = min_val;
  uint32_t val;
  uint16_t i;
  for(i = 1; i < 5; ++i)
  {
    //val = dtw_calc(feature_vec, NUM_FRAME, &fvector[i][0], NUM_FRAME);
    val = dtw_calc(&feature_vec[0][0], NUM_FRAME, &fvector[i][0][0], NUM_FRAME);
    test[i]=val;
    if(val < min_val)
    {
      min_val = val;
      min_loc = i+1;
    }
  }
  return min_loc;
  test[5] = dtw_calc(&feature_vec[0][0], NUM_FRAME, &feature_vec[0][0], NUM_FRAME);
}
*/


static void event_handler()
{
  //static event handler 
  static void (*handler[RAWBUF_LAST])() = {
    [RAWBUF_IDLE] = idle_handler,
    [RAWBUF_FULL1] = raw_buffull_handler,
    [RAWBUF_FULL2] = raw_buffull_handler,
    [RAWBUF_UNDERRUN] = underrun_handler
  };
  (*handler[rawbuf_status])();
}

static void underrun_handler()
{
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
  while(1);
}

inline void idle_handler()
{
}

inline void exit_handler()
{
  while(1);
}

static void raw_buffull_handler()
{
  /*
  STM_EVAL_LEDOn(LED4);
  static uint16_t* buff = data;
  if(rawbuf_status == RAWBUF_FULL1)
  {
    PDM_Filter_64_LSB((uint8_t *)raw_buffer1, buff, VOLUME, &pdm);
    rawbuf_status &= ~RAWBUF_FULL1;
  }
  else if(rawbuf_status == RAWBUF_FULL2)
  {
    PDM_Filter_64_LSB((uint8_t *)raw_buffer2, buff, VOLUME, &pdm);
    rawbuf_status &= ~RAWBUF_FULL2;
  }
  buff += OUT_BUFSIZE;
  if(buff >= data+MAX_BUF_SIZE)
  {
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    STM_EVAL_LEDOff(LED4);
    enframe((int16_t*)data, 0);
    result = recognition();
    STM_EVAL_LEDOn(LED6);
    //EVAL_AUDIO_Play(data, sizeof(uint16_t)*MAX_BUF_SIZE);
  }
  */
}

void post_process()
{
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
  STM_EVAL_LEDOff(LED3);
  //result = recognition();
  STM_EVAL_LEDOn(LED6);
  while(1);
}

int main()
{
  rawbuf_status = RAWBUF_IDLE;
  hamming_init();
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDOff(LED3);
  STM_EVAL_LEDOff(LED4);
  STM_EVAL_LEDOff(LED5);
  STM_EVAL_LEDOff(LED6);
  if(mfcc_init(SAMPLING_FREQZ) !=0)
    while(1);
  EVAL_AUDIO_Init(0, 80, SAMPLING_FREQZ);
  record_init(RECORD_I2S_FS);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
  STM_EVAL_LEDOn(LED3);
  //record start
  while(1)
    enframe();
}


//User implemented callbacks
uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
  return 0;
}

void EVAL_AUDIO_TransferComplete_CallBack(uint16_t* _ptr_buffer, uint32_t _size)
{
  //_size is always 0
}

uint32_t Codec_TIMEOUT_UserCallback(void)
{
  //timeout
  while(1);
  return 0;
}
