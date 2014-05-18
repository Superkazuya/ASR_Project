#include "detection.h"

#define INT16_ABS_MASK (~(1 << 15))
#define INT16_ABS(x) (INT16_ABS_MASK & (x))
#define IS_OPPOSITE_SIGN(x,y) (((x)^(y)) < 0)

inline uint16_t zero_crossing_count(int16_t _sample_last, uint16_t _sample);
inline uint32_t frame_energy(uint16_t _sample);

inline uint16_t zero_crossing_count(int16_t _sample_last, uint16_t _sample)
{
    return IS_OPPOSITE_SIGN(_sample_last, _sample)*(INT16_ABS(_sample_last-_sample) > DIFF);
}

inline uint32_t frame_energy(uint16_t _sample)
{
    return INT16_ABS(_sample);
}
