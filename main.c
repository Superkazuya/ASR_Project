#include "config.h"
#include "mfcc.h"
#include "sample_proc.h"
#include "record.h"
#include "stm32f4_discovery_audio_codec.h"


uint16_t raw_buffer1[RAW_BUFSIZE];
uint16_t raw_buffer2[RAW_BUFSIZE];
uint16_t* raw_buffer=raw_buffer1;

uint16_t buff1[OUT_BUFSIZE];
uint16_t buff2[OUT_BUFSIZE];
uint16_t* buff=buff1;

static void event_handler();
static void raw_buffull_handler();
static void playback();
static void exit_handler();
static void idle_handler();
static void underrun_handler()


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
  while(1);
}

inline void idle_handler()
{
}

inline void exit_handler()
{
  while(1);
}

static void playback()
{
  uint32_t i = 0;
  while(i <= DATA_COL)
  {
    while(SPI_I2S_GetFlagStatus(CODEC_I2S, SPI_I2S_FLAG_TXE) != SET);
    SPI_I2S_SendData(CODEC_I2S, buff[i]);
    while(SPI_I2S_GetFlagStatus(CODEC_I2S, SPI_I2S_FLAG_TXE) != SET);
    SPI_I2S_SendData(CODEC_I2S, buff[i++]);
  }
}

static void raw_buffull_handler()
{
  if(status == STATUS_RAWBUF1_FULL)
    raw_buffer = raw_buffer1;
  else if(status == STATUS_RAWBUF2_FULL)
    raw_buffer = raw_buffer2;

  PDM_Filter_64_LSB((uint8_t *)raw_buffer, buff, VOLUME, &pdm);
  EVAL_AUDIO_Play(buff, OUT_BUFSIZE);
}

int main()
{
  status = STATUS_IDLE;
  //hamming_init();
  mfcc_init(SAMPLING_FREQZ);
  record_init(RECORD_I2S_FS);
  EVAL_AUDIO_Init(0, 80, SAMPLING_FREQZ);
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
  if(_size)
    return;
}

uint32_t Codec_TIMEOUT_UserCallback(void)
{
  //timeout
  while(1);
  return 0;
}
