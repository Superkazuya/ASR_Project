#include "sample_proc.h"
#include "mfcc.h"

uint16_t frame[DATA_ROW][DATA_COL] = { {0} };
float32_t feature_vec[NUM_FRAME][DCT_DIGIT] = {{0}};
static float32_t Hamming[FRAME_SIZE];
extern void post_process();
extern void post_proc_callback(float32_t *_feature_vec, uint16_t _frame_num);

typedef struct {
  uint16_t header;
  uint16_t tail; //tail will always be empty
  int16_t data[MAX_BUF_SIZE];
  uint16_t count;
}circular_buffer;

static circular_buffer cbuffer = {0, 0, {0}, 0};


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
  EXTI_GenerateSWInterrupt(EXTI_Line0);
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

void enframe()
{
  static uint16_t frame_num = 0;
  if(cbuffer.count < FRAME_SIZE)
    return; //nothing to do
  float32_t fdata[FFT_SIZE] = {0};//FFT_SIZE>FRAME_SIZE !!!!! todo
  if(frame_num >= NUM_FRAME)
  {
    /*
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    STM_EVAL_LEDOn(LED5);
    while(1)
      usb_process();
      */
    //enframing done
    post_process();
  }

  uint16_t i;
  uint16_t head_length = MAX_BUF_SIZE-cbuffer.header;
  if(head_length < FRAME_SIZE)//complex case
  {
    for(i = 0; i < head_length; i++)
      fdata[i] = Hamming[i]*cbuffer.data[cbuffer.header+i];
    for(i = 0; i < FRAME_SIZE-head_length; i++)
      fdata[head_length+i] = Hamming[head_length+i]*cbuffer.data[i];
  }
  else //simple case
    for(i = 0; i < FRAME_SIZE; ++i) //SIMD not available
      fdata[i] = Hamming[i]*cbuffer.data[cbuffer.header+i];
  memset(fdata+FRAME_SIZE, 0, (FFT_SIZE-FRAME_SIZE+1)*sizeof(float32_t));//zero padding
  cbuffer.count -= FRAME_SHIFT; //counter modification
  cbuffer.header += FRAME_SHIFT;
  if(cbuffer.header >= MAX_BUF_SIZE)
    cbuffer.header -= MAX_BUF_SIZE;

  mfcc(fdata, feature_vec[frame_num]);
  post_proc_callback(&feature_vec[0][0], frame_num);
  frame_num++;
}

//rawbuf full handler, appending data
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) == RESET)
    return;
  static int16_t buff[OUT_BUFSIZE] ;
  if(cbuffer.count + OUT_BUFSIZE > MAX_BUF_SIZE)
    while(1);//todo
  if(rawbuf_status == RAWBUF_UNDERRUN)
  {
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    STM_EVAL_LEDOff(LED3);
    while(1);
  }
  uint16_t tail_length = MAX_BUF_SIZE-cbuffer.tail;
  if(tail_length < OUT_BUFSIZE) //complex case
  {
    if(rawbuf_status == RAWBUF_FULL1)
    {
      PDM_Filter_64_LSB((uint8_t *)raw_buffer1, (uint16_t*)buff, VOLUME, &pdm);
      rawbuf_status &= ~RAWBUF_FULL1;
    }
    else if(rawbuf_status == RAWBUF_FULL2)
    {
      PDM_Filter_64_LSB((uint8_t *)raw_buffer2, (uint16_t*)buff, VOLUME, &pdm);
      rawbuf_status &= ~RAWBUF_FULL2;
    }//underrun ????
    memcpy(buff, cbuffer.data+cbuffer.tail, sizeof(uint16_t)*tail_length);
    memcpy(buff+tail_length, cbuffer.data, sizeof(uint16_t)*(OUT_BUFSIZE-tail_length));
    cbuffer.tail = OUT_BUFSIZE-tail_length;
    cbuffer.count += OUT_BUFSIZE;
  }
  else
  {
    if(rawbuf_status == RAWBUF_FULL1)
    {
      PDM_Filter_64_LSB((uint8_t *)raw_buffer1, (uint16_t*)(cbuffer.data+cbuffer.tail), VOLUME, &pdm);
      rawbuf_status &= ~RAWBUF_FULL1;
    }
    else if(rawbuf_status == RAWBUF_FULL2)
    {
      PDM_Filter_64_LSB((uint8_t *)raw_buffer2, (uint16_t*)(cbuffer.data+cbuffer.tail), VOLUME, &pdm);
      rawbuf_status &= ~RAWBUF_FULL2;
    }//underrun ????
    cbuffer.tail += OUT_BUFSIZE;
    cbuffer.count += OUT_BUFSIZE;

    EXTI_ClearITPendingBit(EXTI_Line0);
  }
  /*
  enframe(buff, 0);
  buff += OUT_BUFSIZE;
  if(buff >= data+MAX_BUF_SIZE)
  {
      */
}
