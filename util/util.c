#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "util.h"

double
rand_double()
{
  return ((double)rand()/(double)RAND_MAX);
}


void
exit_if_null (void* p)
{
  if (p == NULL)
  {
    perror("Error");
    exit(-1);
  }
}

void
exit_if_not_zero (int n)
{
  if (n != 0)
  {
    perror("Error");
    exit(n);
  }
}

int
printferr (const char *format, ...)
{
  va_list args;
  va_start(args, format);
  int retr = vfprintf(stderr, format, args);
  va_end(args);
  return retr;
}

void
printferr_and_exit (const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printferr(format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}

void
printferr_and_exitno (int exit_value, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  printferr(format, args);
  va_end(args);
  exit(exit_value);
}

