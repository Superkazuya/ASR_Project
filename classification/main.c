#include "config.h"
#include "mfcc.h"
#include "sample_proc.h"
#include "record.h"
#include "data.h"
#include "usb.h"

uint16_t raw_buffer1[RAW_BUFSIZE];
uint16_t raw_buffer2[RAW_BUFSIZE];
uint16_t* raw_buffer=raw_buffer1;
uint32_t dtw_mat[NUM_CLASS][NUM_FRAME+1] = {[0 ... NUM_CLASS-1][0 ... NUM_FRAME] = 9999999};
__IO uint16_t result = 0;
//uint32_t test[NUM_CLASS];

//extern uint32_t dtw_calc(float32_t *_vect1, uint16_t _len1, float32_t *_vect2, uint16_t _len2);
extern uint32_t dtw_calc(float32_t *_vect1, uint16_t _num_frame, float32_t *_vect2, uint16_t _len2, uint32_t* _dtw);
//static void event_handler();
//static void raw_buffull_handler();
//static void exit_handler();
//static void idle_handler();
//static void underrun_handler();

void post_proc_callback(float32_t *_feature_vec, uint16_t _frame_num)
{
  uint16_t i;
  for(i = 0; i < NUM_CLASS; i++)
    dtw_calc(_feature_vec, _frame_num, (float32_t*)&fvector[i][0][0], NUM_FRAME, dtw_mat[i]);
}

/*
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
}
*/

void post_process()
{
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
  STM_EVAL_LEDOff(LED3);
  uint32_t min = dtw_mat[0][NUM_FRAME];
  uint16_t i;
  uint16_t min_loc = 0;
  for(i = 1; i < NUM_CLASS; ++i)
    if(dtw_mat[i][NUM_FRAME] < min)
    {
      min = dtw_mat[i][NUM_FRAME];
      min_loc = i;
    }
  result = min_loc;
  STM_EVAL_LEDOn(LED6);
  while(1)
    usb_process();
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
  usb_init();
  if(mfcc_init(SAMPLING_FREQZ) !=0)
    while(1);

  //EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
  record_init(RECORD_I2S_FS);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
  STM_EVAL_LEDOn(LED3);
  //record start
  while(1)
    enframe();
}

