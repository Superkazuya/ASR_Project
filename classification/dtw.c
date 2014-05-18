#include "config.h"

static inline uint32_t euc_dist(float32_t* _vect1, float32_t* _vect2)
{
  uint16_t i;
  float32_t sum = 0;
  for(i = 0; i < DCT_DIGIT; ++i)
    sum += _vect1[i]*_vect1[i]+_vect2[i]*_vect2[i];
  return sqrt(sum);
}

static inline uint32_t min_3(uint32_t _val1, uint32_t _val2, uint32_t _val3)
{
}

float32_t dtw_calc(float32_t** _vect1, uint16_t _len1, float32_t** _vect2, uint16_t _len2)
{
  uint32_t dtw[NUM_FRAME][NUM_FRAME] = {{-1}};
}

