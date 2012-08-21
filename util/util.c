#include <stdlib.h>
#include <stdio.h>
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

