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

void
exit_if_null (const void* const p)
{
  if (p == NULL)
  {
    perror("Error");
    exit(-1);
  }
}

void
exit_if_not_zero (const int n)
{
  if (n != 0)
  {
    perror("Error");
    exit(n);
  }
}

int
putserr (const char* const str)
{
  int retr = fputs(str, stderr);
  return retr;
}

void
putserr_and_exit (const char* const str)
{
  fputs(str, stderr);
  exit(EXIT_FAILURE);
}

void
putserr_and_exitno (const int         exit_value,
                    const char* const str)
{
  fputs(str, stderr);
  exit(exit_value);
}

int
printferr (const char* const format,
           ...)
{
  va_list args;
  va_start(args, format);
  int retr = vfprintf(stderr, format, args);
  va_end(args);
  return retr;
}

void
printferr_and_exit (const char * const format,
                    ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

void
printferr_and_exitno (const int         exit_value,
                      const char* const format,
                      ...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(exit_value);
}

