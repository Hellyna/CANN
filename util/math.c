#include "math.h"

inline double fmin(const double a,
                   const double b)
{
  return (a <= b) ? a : b;
}

inline double fmax(const double a,
                   const double b)
{
  return (a >= b) ? a : b;
}
