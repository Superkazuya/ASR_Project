#include "record.h"


float32_t buffer[DATA_ROW][DATA_COL];
__IO uint16_t status = 0;

static void gpio_init()
{
  RCC_AHB1PeriphClockCmd(SPI_SCK_GPIO_CLK | SPI_MOSI_GPIO_CLK, ENABLE);
  GPIO_InitTypeDef gpio = {
    .GPIO_Pin = SPI_SCK_PIN,
    .GPIO_Mode = GPIO_Mode_AF,
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_PuPd = GPIO_PuPd_NOPULL,
    .GPIO_Speed = GPIO_Speed_50MHz
  };
  GPIO_Init(SPI_SCK_GPIO_PORT, &gpio);
  GPIO_PinAFConfig(SPI_SCK_GPIO_PORT, SPI_SCK_SOURCE, SPI_SCK_AF);

  gpio.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_GPIO_PORT, &gpio);
  GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT, SPI_MOSI_SOURCE, SPI_MOSI_AF);
}

static void nvic_init()
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  NVIC_InitTypeDef nvic = {
    .NVIC_IRQChannel = SPI2_IRQn,
    .NVIC_IRQChannelPreemptionPriority = 1,
    .NVIC_IRQChannelSubPriority = 0,
    .NVIC_IRQChannelCmd = ENABLE
  };
  NVIC_Init(&nvic);
}
 
static void spi_init(uint16_t _audio_freqz)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  SPI_I2S_DeInit(SPI2);
  I2S_InitTypeDef i2s = {
    .I2S_AudioFreq = _audio_freqz,
    .I2S_Standard = I2S_Standard_LSB,
    .I2S_DataFormat = I2S_DataFormat_16b,
    .I2S_CPOL = I2S_CPOL_High,
    .I2S_Mode = I2S_Mode_MasterRx,
    .I2S_MCLKOutput = I2S_MCLKOutput_Disable
  };
  I2S_Init(SPI2, &i2s);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
  I2S_Cmd(SPI2, ENABLE);
}
 
void record_init(uint16_t _audio_freqz)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
  filter_init();
  gpio_init();
  nvic_init();
  spi_init(_audio_freqz);
}

void SPI2_IRQHandler(void)
{
  if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != SET)
    return;
  // receiver not empty
  if(status & (1 << STATUS_RAW_BUF_FULL)) //raw buff underrun
  {
      status = (1 << STATUS_RAW_BUF_UNDERRUN);
      return;
  }
  sample_proc(SPI_I2S_ReceiveData(SPI2));
}
