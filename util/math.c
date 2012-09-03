#include "math.h"

#if !(defined __GNU_LIBRARY__ || __GLIBC__ || __GLIBC_MINOR__)
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
#endif
