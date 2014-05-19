#include "detection.h"

#define IS_OPPOSITE_SIGN(x,y) (((x)^(y)) < 0)

enum SIG_STATE
{
  SIG_NONE,
  SIG_TRANS,
  SIG_SPEECH,
  SIG_LAST
};

static __IO uint8_t state = SIG_NONE;

inline void zcr_energy_calc(int16_t* _sample, uint32_t* _result)
{
  int16_t last, sample;
  uint32_t zcr = 0;
  uint32_t energy = 0;
  last = _sample[0];
  uint16_t i;
  for(i = 1; i < DETECTION_LENGTH; ++i)
  {
    sample = _sample[i];
    zcr += IS_OPPOSITE_SIGN(last, sample)*(ABS(last-sample) > DIFF);
    energy += ABS(sample);
    last = sample;
  }
  _result[0] = zcr;
  _result[1] = energy;
}

uint8_t detection(int16_t* _data)
{
  /*
  static uint32_t energy_low = ENG_THRESHOLD_LOW;
  static uint32_t energy_hi = ENG_THRESHOLD_HIGH;
  static uint32_t energy_max = 0;
  */
  uint32_t criteria[2];
  zcr_energy_calc(_data, criteria);
  switch(state)
  {
    case SIG_NONE:
      if(!(criteria[0] < ZCR_THRESHOLD_LOW && criteria[1] <  ENG_THRESHOLD_LOW))
	state = SIG_TRANS;
      break;
    case SIG_TRANS:
      if(criteria[0] < ZCR_THRESHOLD_LOW && criteria[1] <  ENG_THRESHOLD_LOW)
	state = SIG_NONE;
      else if(!(criteria[0] < ZCR_THRESHOLD_HIGH && criteria[1] <  ENG_THRESHOLD_HIGH))
	state = SIG_SPEECH;
      break;
    default:
      break;
  }
  if(state == SIG_SPEECH)
    return 1;
  return 0;

  /*
  if(criteria[1] > energy_max)
    energy_max = criteria[1];
  energy_low = MIN2(energy_low, energy_max/8);
  energy_hi = MIN2(energy_hi, energy_max/4);
  */
}
