#include "mfcc.h"

static uint32_t mel_filterbanks[NUM_FILTER_BANKS+2]; //It is not NUM_FILTER_BANKS+1, since filter banks are overlapping
static arm_rfft_instance_f32 rfft;
static arm_cfft_radix4_instance_f32 cfft;
#ifdef ARM_DCT
static arm_dct4_instance_f32 dct;
static arm_rfft_instance_f32 dct_rfft;
static arm_cfft_radix4_instance_f32 dct_cfft;
#endif


/**
 * @brief calculate power spectrum. NOT IN-PLACE
 *
 * @param _samples length = FFT_SIZE
 * @param _powerspectrum length = FFT_SIZE/2
 */
static void powerspectrum(float32_t *_samples, float32_t *_powerspectrum)
{
  float32_t tmp[FFT_SIZE*2]; //FFT_SIZE*2, with complex conjugate. CMSIS document never mentioned this.
  arm_rfft_f32(&rfft, _samples, tmp);
  arm_cmplx_mag_squared_f32(tmp, _powerspectrum, FFT_SIZE/2);
}

static inline float32_t mel2freq(float32_t _mel)
{
  return(700*(pow(10, (_mel/2595))-1));
}

static inline float32_t freq2mel(float32_t _freq)
{
  return(1125*log(1+(_freq/700)));
}

static void mel_filterbanks_init(uint16_t _sample_rate)
{
  float32_t mel_lb = freq2mel(FREQ_LB);
  float32_t mel_hb = freq2mel(FREQ_HB < (_sample_rate/2) ? FREQ_HB:(_sample_rate/2));
  float32_t mel_delta = (mel_hb - mel_lb)/(NUM_FILTER_BANKS+1);
  uint8_t i;
  for(i = 0; i <= (NUM_FILTER_BANKS+1); ++i)
  {
    mel_filterbanks[i] = FFT_SIZE*mel2freq(mel_lb)/_sample_rate;
    mel_lb += mel_delta;
  }
}

static void log_mel_spectrum(float32_t *_input, float32_t* _output) 
{
  uint16_t bin,i;
  //_output must be initialize to 0 first. 
  //memset(_output, 0, NUM_FILTER_BANKS*sizeof(float32_t));

  for(bin = mel_filterbanks[0]; bin < mel_filterbanks[1]; ++bin)
    _output[0] += _input[bin]*(mel_filterbanks[1]-bin)/(mel_filterbanks[1]-mel_filterbanks[0]);
  for(bin = mel_filterbanks[NUM_FILTER_BANKS]; bin < mel_filterbanks[NUM_FILTER_BANKS+1]; ++bin)
    _output[NUM_FILTER_BANKS-1] += _input[bin]*(mel_filterbanks[NUM_FILTER_BANKS+1]-bin)/(mel_filterbanks[NUM_FILTER_BANKS+1]-mel_filterbanks[NUM_FILTER_BANKS]);

  for(i = 1; i < NUM_FILTER_BANKS; ++i)
  {
    uint32_t lowerb = mel_filterbanks[i];
    uint32_t upperb = mel_filterbanks[i+1];
    uint32_t delta = upperb -lowerb;
    float32_t tmp;
    _output[i] += 0;
    _output[i-1] += _input[lowerb];
    for(bin = lowerb+1; bin < upperb; ++bin)
    {
      tmp = (bin-lowerb)*_input[bin]/delta;
      _output[i] += tmp;
      _output[i-1] += _input[bin] - tmp;
    }
    _output[i-1] = (float32_t)log((double)_output[i-1]);
  }
  _output[NUM_FILTER_BANKS-1] = (float32_t)log((double)_output[NUM_FILTER_BANKS-1]);
}

/**
 * @brief MFCC init. Need to initialize only once
 *
 * @param _sample_rate maximum sampling rate
 *
 * @return  0 if sucessful, 1 dct init err, -1 rfft init err
 */
int8_t mfcc_init(uint16_t _sample_rate)
{
  mel_filterbanks_init(_sample_rate);
#ifdef ARM_DCT
  if(arm_dct4_init_f32(&dct, &dct_rfft, &dct_cfft, DCT_SIZE, DCT_SIZE/2, 600) != ARM_MATH_SUCCESS)
    return 1;
#endif
  if(arm_rfft_init_f32(&rfft, &cfft, FFT_SIZE, 0, 1) != ARM_MATH_SUCCESS)
    return -1;
  return 0;
}

/**
 * @brief Calculate MFCC, NOT IN PLACE
 *
 * @param _samples audio sample length == FRAME_SIZE
 * @param _mfcc MFCC array, length == DCT_HIGH-DCT_LOW+1 == DCT_DIGIT
 */
void mfcc(float32_t* _samples, float32_t* _mfcc)
{
  float32_t ps[FFT_SIZE/2];
  float32_t mfcc[DCT_SIZE]={0};
  powerspectrum(_samples, ps);
  log_mel_spectrum(ps, mfcc);
#ifdef ARM_DCT
  float32_t state[2*DCT_SIZE]={0};
  arm_dct4_f32(&dct, state, mfcc); //dct can be in place
  memcpy(mfcc+DCT_LOW, _mfcc, DCT_DIGIT*sizeof(float32_t));
#else
  uint16_t i, j;
  for(i = DCT_LOW; i <= DCT_HIGH; ++i)
    for(j = 0; j < NUM_FILTER_BANKS; ++j)
      _mfcc[i-DCT_LOW] += mfcc[j]*arm_cos_f32(3.1415926*i*(j-0.5)/NUM_FILTER_BANKS);//todo
#endif
}
