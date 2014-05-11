#include "config.h"
#include "mfcc.h"
#include "sample_proc.h"
#include "record.h"
#include "stm32f4_discovery_audio_codec.h"
#include "stm32f4_discovery.h"


uint16_t raw_buffer1[RAW_BUFSIZE];
uint16_t raw_buffer2[RAW_BUFSIZE];
uint16_t* raw_buffer=raw_buffer1;

uint16_t buff[OUT_BUFSIZE];

static void event_handler();
static void raw_buffull_handler();
static void exit_handler();
static void idle_handler();
static void underrun_handler();


static void event_handler()
{
  //static event handler 
  static void (*handler[STATUS_LAST])() = {
    [STATUS_IDLE] = idle_handler,
    [STATUS_RAWBUF1_FULL] = raw_buffull_handler,
    [STATUS_RAWBUF2_FULL] = raw_buffull_handler,
    [STATUS_RAWBUF_UNDERRUN] = underrun_handler
  };
  (*handler[status])();
}

static void underrun_handler()
{
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
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
  if(status == STATUS_RAWBUF1_FULL)
    raw_buffer = raw_buffer1;
  else if(status == STATUS_RAWBUF2_FULL)
    raw_buffer = raw_buffer2;

  PDM_Filter_64_LSB((uint8_t *)raw_buffer, buff, VOLUME, &pdm);
  status = STATUS_IDLE;
  //EVAL_AUDIO_Play(buff, sizeof(uint16_t)*OUT_BUFSIZE);
  uint16_t i;
  for(i = 0; i < OUT_BUFSIZE; ++i)
    store(buff[i]);
}

int main()
{
  hamming_init();
  STM_EVAL_LEDInit(LED3);
  mfcc_init(SAMPLING_FREQZ);
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
