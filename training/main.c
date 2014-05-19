#include "config.h"
#include "mfcc.h"
#include "sample_proc.h"
#include "record.h"
#include "usb.h"
#include "detection.h"

uint16_t raw_buffer1[RAW_BUFSIZE];
uint16_t raw_buffer2[RAW_BUFSIZE];
uint16_t* raw_buffer=raw_buffer1;
uint16_t data[MAX_BUF_SIZE];


static void event_handler();
static void raw_buffull_handler();
static void exit_handler();
static void idle_handler();
static void underrun_handler();


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
  if(detection((int16_t*)buff))
    STM_EVAL_LEDOn(LED4);
  else
    STM_EVAL_LEDOff(LED4);
  buff += OUT_BUFSIZE;
  if(buff >= data+MAX_BUF_SIZE)
  {
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    enframe((int16_t*)data, 0);
    STM_EVAL_LEDOn(LED5);
    while(1)
      usb_process();
  }

  //EVAL_AUDIO_Play(buff, sizeof(uint16_t)*OUT_BUFSIZE);
  /*
  uint16_t i;
  for(i = 0; i < OUT_BUFSIZE; ++i)
    enframe(buff[i]);
    */
}


int main()
{
  rawbuf_status = RAWBUF_IDLE;
  hamming_init();
  usb_init();
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
  //record start
  while(1)
    event_handler();
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
