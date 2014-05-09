#include "config.h"
#include "mfcc.h"
#include "sample_proc.h"
#include "record.h"
#include "codec.h"

static void event_handler();
static void raw_buffull_handler();
void exit_handler();


static void event_handler()
{
  //static event handler 
  static void (*handler[OP_LAST])() = {
    [OP_IDLE] = exit_handler,
    [OP_EXIT] = exit_handler,
    [OP_PROC] = raw_buffull_handler
  };
  (*handler[status])();
}

void exit_handler()
{
}

static void raw_buffull_handler()
{
  status = 0;
  uint16_t i = 0;
  uint16_t* header;
  PDM_Filter_64_LSB((uint8_t*)buffer[0], (uint16_t*)buffer[0], VOLUME, &pdm);
  header=(uint16_t*)buffer[0];
  while(i <= OUT_BUFSIZE)
  {
    while(SPI_I2S_GetFlagStatus(CODEC_I2S, SPI_I2S_FLAG_TXE) != SET);
    SPI_I2S_SendData(CODEC_I2S, *(header+i));
    while(SPI_I2S_GetFlagStatus(CODEC_I2S, SPI_I2S_FLAG_TXE) != SET);
    SPI_I2S_SendData(CODEC_I2S, *(header+i++));
  }
}

int main()
{
  hamming_init();
  filter_init();
  mfcc_init(32000);
  record_init(32000);
  codec_init();
  I2S_Cmd(CODEC_I2S, ENABLE);
  //record start
  while(1)
    event_handler();
}
