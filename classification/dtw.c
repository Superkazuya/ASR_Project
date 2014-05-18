#include "config.h"
#define MAX(x,y) ((x) > (y) ? (x):(y))
#define MIN(x,y) ((x) < (y) ? (x):(y))
#define MIN3(x,y,z) ((x) < MIN(y,z) ? (x): MIN(y,z))
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define W_SIZE 60

static inline uint32_t dist(float32_t* _vect1, float32_t* _vect2)
{
  static float32_t tmp[DCT_DIGIT];
  float32_t sum;
  arm_sub_f32(_vect1, _vect2, tmp, DCT_DIGIT);
  arm_dot_prod_f32(tmp, tmp, DCT_DIGIT, &sum);
  return sqrt((uint32_t)sum);
}

//linear space , dtw with constraint
uint32_t dtw_calc(float32_t *_vect1, uint16_t _len1, float32_t *_vect2, uint16_t _len2)
{
  uint16_t _w_size = MAX(W_SIZE, ABS(_len1-_len2));
  uint32_t dtw[NUM_FRAME+1] = {[0 ... NUM_FRAME] = 9999999};
  uint16_t i,j;
  float32_t *si, *tj;
  uint32_t tl = 0; //topleft = dtw[0]
  uint32_t top;
  for(i = 0; i < _len1; ++i)
  {
    uint32_t left = 9999999; //left = dtw[0];
    si = _vect1+i*DCT_DIGIT;
    for(j = MAX(0,i-_w_size); j < MIN(_len2, i+_w_size+1); ++j)
    {
      tj = _vect2+j*DCT_DIGIT;
      top = dtw[j+1];
      dtw[j+1] = dist(si,tj)+MIN3(top, tl, left);
      tl = top;
      left = dtw[j+1];
    }
  }
  return(dtw[_len2]);
}

