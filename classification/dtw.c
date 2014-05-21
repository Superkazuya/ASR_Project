#include "config.h"
#define MIN3(x,y,z) ((x) < MIN2(y,z) ? (x): MIN2(y,z))
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
/*
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
    for(j = MAX(0,i-_w_size); j < MIN2(_len2, i+_w_size+1); ++j)
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

*/
uint32_t dtw_calc(float32_t *_vect1, uint16_t _num_frame, float32_t *_vect2, uint16_t _len2, uint32_t* _dtw)
{
  uint16_t _w_size = MAX(W_SIZE, ABS(NUM_FRAME-_len2));//_len1 now == NUM_FRAME , _len2=NUM_FRAME
  uint16_t j;
  float32_t *si, *tj;
  uint32_t tl = (_num_frame == 0)?0:9999999; //topleft = dtw[0]
  uint32_t top;
  uint32_t left = 9999999; //left = dtw[0];
  si = _vect1+_num_frame*DCT_DIGIT;
  for(j = MAX(0,_num_frame-_w_size); j < MIN2(_len2, _num_frame+_w_size+1); ++j)
  {
    tj = _vect2+j*DCT_DIGIT;
    top = _dtw[j+1];
    _dtw[j+1] = dist(si,tj)+MIN3(top, tl, left);
    tl = top;
    left = _dtw[j+1];
  }
  return(_dtw[_len2]);
}

/*
 * def dtw_calc(TEMPLATE[0 ... m], MFCC):
 *    topleft = INFINITY
 *    if(first frame):
 *	topleft = 0
 *    left = INFINITY
 *    for i in Range(left window boundary ... right window boundary):
 *	top = _dtw[i+1]
 *	_dtw[i+1] = euclidean_distance(TEMPLATE[i], MFCC) + MIN(topleft, left, top)
 *	topleft = top
 *	left=_dtw[i+1]
 *    return _dtw;
 */
 
