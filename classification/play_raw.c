#include "config.h"
#include "ff.h"
#include "stdio.h"
#define MUL 8
extern DIR dir;
extern FIL file;
extern AudioRemSize;
static __IO int32_t size_left; //in uint16;
static __IO uint8_t Xfer_status;

//play raw audio file using polling
void playback(uint16_t _result)
{
  AudioRemSize = 0;
  size_left = RECORD_MAX_TIME*SAMPLING_FREQZ; //in uint16
  Xfer_status = 0;
  uint8_t buf_idx = 0;
  UINT br;
  uint16_t buffer[2][_MAX_SS*MUL/2] = {{0}};
  char path[] = "0:/";
  char filename[16] = {'0',':','0','1','.','w','a','v',0};
  //sprintf(filename, "%02d.wav", _result+1); <=gives sbrk error?
  filename[3] += _result;
  if(f_opendir(&dir, path) != FR_OK)
    while(1);
  if(f_open(&file, filename, FA_READ) != FR_OK)
    while(1);
  
  I2S_Cmd(SPI2,DISABLE);
  if(EVAL_AUDIO_Init(OUTPUT_DEVICE_BOTH, 80, SAMPLING_FREQZ/2))
    while(1);
  //f_read(&file,buffer[0],MUL*_MAX_SS, &br);
  //f_read(&file,buffer[1],MUL*_MAX_SS, &br);
  Audio_MAL_Play((uint32_t)buffer[0], _MAX_SS*MUL); //buffer1 pointer is sent as a NUMBER. 
  EVAL_AUDIO_PauseResume(1);
  while(size_left)
  {
    while(Xfer_status == 0 && size_left);
    Xfer_status = 0;
    buf_idx = !buf_idx;
    Audio_MAL_Play((uint32_t)buffer[buf_idx], _MAX_SS*MUL); //buffer1 pointer is sent as a NUMBER.
    f_read(&file,buffer[!buf_idx],MUL*_MAX_SS, &br);
    if(br == 0)
      break;
  }

  EVAL_AUDIO_Stop(CODEC_PDWN_SW);
}

//User implemented callbacks
uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
  return 0;
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t _ptr_buffer, uint32_t _size)
{ 
  if(size_left <= _MAX_SS*MUL/2)
  {
    size_left = 0;
    return;
  }
  size_left -= _MAX_SS*MUL/2;
  Xfer_status = 1;
}

uint32_t Codec_TIMEOUT_UserCallback(void)
{
  //timeout
  while(1);
  return 0;
}
