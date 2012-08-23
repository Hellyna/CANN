#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "util.h"

inline double
rand_double()
{
  return ((double)rand()/(double)RAND_MAX);
}

inline void*
malloc_exit_if_null(const size_t size)
{
  void* p = malloc(size);
  exit_if_null(p);
  return p;
}

inline void*
calloc_exit_if_null(const size_t num,
                    const size_t size)
{
  void* p = calloc(num, size);
  exit_if_null(p);
  return p;
}

inline void
free_and_null (void* p)
{
  free(p);
  p = NULL;
}

inline void
exit_if_null (void* p)
{
  if (p == NULL)
  {
    perror("Error");
    exit(-1);
  }
}

inline void
exit_if_not_zero (int n)
{
  if (n != 0)
  {
    perror("Error");
    exit(n);
  }
}

inline int
printferr (const char *format, ...)
{
  va_list args;
  va_start(args, format);
  int retr = vfprintf(stderr, format, args);
  va_end(args);
  return retr;
}

inline void
printferr_and_exit (const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printferr(format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

inline void
printferr_and_exitno (int exit_value, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printferr(format, args);
  va_end(args);
  exit(exit_value);
}

