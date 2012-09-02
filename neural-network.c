#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "util/util.h"

#include "neural-network.h"



neural_network_t*
construct_neural_network (const size_t* config,
                          const size_t  config_size,
                          const double  min_weight,
                          const double  max_weight,

                          void          (*weight_initialization_function) (const neural_network_t*))
{
  // MALLOC: nn
  neural_network_t* nn = malloc_exit_if_null(sizeof(neural_network_t));

  // INIT: nn->config_size
  nn->config_size = config_size;

  // MALLOC: nn->config
  nn->config = malloc_exit_if_null(sizeof(size_t) * config_size);

  // INIT: nn->config
  memcpy(nn->config, config, sizeof(size_t) * config_size);

  // INIT: nn->min_weight
  nn->min_weight = min_weight;

  // INIT: nn->max_weight
  nn->max_weight = max_weight;

  size_t num_weight_layers = nn->config_size - 1;
  // MALLOC: nn->weights
  nn->weights = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);

  size_t i, j;
  for (i = 0; i < num_weight_layers; ++i)
  {
    nn->weights[i] = malloc_exit_if_null(SIZEOF_PTR * nn->config[i]);
    for (j = 0; j < nn->config[i]; ++j)
    {
      nn->weights[i][j] = malloc_exit_if_null(sizeof(double) * nn->config[i + 1]);
    }
  }

  // INIT: nn->weights
  (*weight_initialization_function) (nn);

  return nn;
}


void
destruct_neural_network (neural_network_t* nn)
{
  // FREE: nn->weights
  size_t num_weight_layers = nn->config_size - 1;
  size_t i, j;
  for (i = 0; i < num_weight_layers; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(nn->weights[i][j]);
    }
    free_and_null(nn->weights[i]);
  }
  free_and_null(nn->weights);

  // FREE: config
  free_and_null(nn->config);

  // FREE: nn
  free_and_null(nn);
}


void
initialize_nguyen_widrow_weights (const neural_network_t* nn)
{
  initialize_uniform_weights(nn);

  size_t num_input = nn->config[0];
  size_t num_hidden = 0;
  size_t i;
  for (i = 1; i < nn->config_size - 1; ++i)
  {
    num_hidden += nn->config[i];
  }
  double beta = 0.7 * pow(num_hidden, 1.0/num_input);

  size_t j, k;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      double n = 0.0;
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        double x = nn->weights[i][j][k];
        n += x * x;
      }
      n = sqrt(n);

      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        double x = nn->weights[i][j][k];
        nn->weights[i][j][k] = (x * beta) / n;
      }
    }
  }
}


void
initialize_uniform_weights (const neural_network_t* nn)
{
  srand((unsigned)time(NULL));

  size_t i, j, k;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        nn->weights[i][j][k] = rand_double() * (nn->max_weight - nn->min_weight) + nn->min_weight ;
      }
    }
  }
}


