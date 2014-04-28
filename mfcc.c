#include "mfcc.h"

#define FFT_SIZE 512

#define NUM_FILTER_BANKS 26
#define FREQ_LB 20
#define FREQ_HB 20000

static float32_t mel_filterbanks[NUM_FILTER_BANKS];
static arm_dct4_instance_f32 dct;
static arm_rfft_instance_f32 rfft;
static arm_cfft_radix4_instance_f32 cfft;


static void powerspectrum(float32_t *_samples, float32_t *_powerspectrum)
{
  arm_rfft_f32(&rfft, _samples, _samples);
  arm_cmplx_mag_squared_f32(_samples, _powerspectrum, FFT_SIZE);
}

static inline float32_t mel2freq(float32_t _mel)
{
  return(700*(pow(10, (_mel/2595))-1));
}

static void mel_filterbanks_init(uint16_t _sample_rate)
{
  float32_t freq_lb = FREQ_LB;
  float32_t freq_hb = FREQ_HB < (_sample_rate/2) ? FREQ_HB:(_sample_rate/2);
  float32_t freq_delta = (freq_hb - freq_lb)/2;
  uint8_t i;
  for(i = 0; i < NUM_FILTER_BANKS; ++i)
  {
    mel_filterbanks[i] = mel2freq(freq_lb);
    freq_lb += freq_delta;
  }
}

static void log_mel_spectrum(float32_t *_input, float32_t* _output)
{
  uint16_t i,j;
  for(i = 0; i < NUM_FILTER_BANKS; ++i)
  {
    for(j = 0; j < FFT_SIZE; ++j)
      if(j < mel_filterbanks[i+1] && j >= mel_filterbanks[i])
	_output[i] += _input[j]*(j-mel_filterbanks[i])/(mel_filterbanks[i+1]-mel_filterbanks[i]);
      else if(j < mel_filterbanks[i+2] && j >= mel_filterbanks[i+1])
	_output[i] += _input[j]*(mel_filterbanks[i+2]-j)/(mel_filterbanks[i+2]-mel_filterbanks[i+1]);
    _output[i] = log(_output[i]);
  }
}

int8_t mfcc_init(uint16_t _sample_rate)
{
  mel_filterbanks_init(_sample_rate);
  if(arm_dct4_init_f32(&dct, &rfft, &cfft, NUM_FILTER_BANKS, NUM_FILTER_BANKS/2, sqrt(2/NUM_FILTER_BANKS)) != ARM_MATH_SUCCESS)
    return 1;
  if(arm_rfft_init_f32(&rfft, &cfft, FFT_SIZE, 0, 0) != ARM_MATH_SUCCESS)
    return -1;
  return 0;
}

void mfcc(float32_t* _samples, float32_t* _mfcc)
{
  float32_t state[NUM_FILTER_BANKS];
  powerspectrum(_samples, _samples);
  log_mel_spectrum(_samples, _mfcc);
  arm_dct4_f32(&dct, state, _mfcc);
}
