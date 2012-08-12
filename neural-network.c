#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
/****************************** BEGIN DECLS *******************************/
struct neural_network_t {
  int* config;
  size_t config_size;

  double* weights;
  size_t weights_size;
  double weights_min;
  double weights_max;
};

typedef struct neural_network_t neural_network_t;

neural_network_t*
construct_neural_network (const int*    config,
                          const size_t  config_size);

neural_network_t*
construct_neural_network_full (const int*   config,
                               const size_t config_size,
                               const double weights_min,
                               const double weights_max);

void
destruct_neural_network (neural_network_t* nn);

size_t
set_weight (neural_network_t* nn,
            const size_t      row_index,
            const size_t      neuron_index_in_row,
            const size_t      neuron_index_in_next_row,
            const double      value);

size_t
print_weight (const neural_network_t* nn,
              const size_t            row_index,
              const size_t            neuron_index_in_row,
              const size_t            neuron_index_in_next_row);

size_t
get_weight_index (const neural_network_t* nn,
                  const size_t            row_index,
                  const size_t            neuron_index_in_row,
                  const size_t            neuron_index_in_next_row);

size_t
_2d_to_1d (const size_t i,
           const size_t j,
           const size_t j_max);

double
rand_double();

/***************************** END DECLS ********************************/


/***************************** BEGIN IMPL *******************************/
neural_network_t*
construct_neural_network (const int*    config,
                          const size_t  config_size)
{
  return construct_neural_network_full(config, config_size, -1.0, 1.0);
}


neural_network_t*
construct_neural_network_full (const int*   config,
                               const size_t config_size,
                               const double weights_min,
                               const double weights_max)
{
  // MALLOC: nn
  neural_network_t* nn = malloc(sizeof(neural_network_t));
  if (nn == NULL)
    return NULL;

  // INIT: nn->config_size
  nn->config_size = config_size;

  // MALLOC: nn->config
  nn->config = malloc(sizeof(int) * config_size);
  if (nn->config == NULL)
    return NULL;

  // INIT: nn->config
  memcpy(nn->config, config, sizeof(int) * config_size);

  // INIT: nn->weights_size
  nn->weights_size = 0;

  // INIT: nn->weights_min
  nn->weights_min = weights_min;

  // INIT: nn->weights_max
  nn->weights_max = weights_max;

  // MALLOC: nn->weights
  size_t i;
  for (i = 0; i < nn->config_size - 1; ++i) {
    nn->weights_size += nn->config[i] * nn->config[i + 1];
  }
  nn->weights = malloc(sizeof(double) * nn->weights_size);
  if (nn->weights == NULL)
    return NULL;

  // INIT: nn->weights
  srand((unsigned)time(NULL));
  for (i = 0; i < nn->weights_size; ++i) {
    nn->weights[i] = rand_double() * (nn->weights_max - nn->weights_min) + nn->weights_min ;
  }
  int j, k;
  for (i = 0; i < nn->config_size - 1; ++i) {
    for (j = 0; j < nn->config[i]; ++j) {
      for (k = 0; k < nn->config[i + 1]; ++k) {
        print_weight(nn, i, j, k);
      }
    }
  }
  return nn;
}


void
destruct_neural_network (neural_network_t* nn)
{
  // FREE: config
  free(nn->config);

  // FREE: weights
  free(nn->weights);

  // FREE: nn
  free(nn);
}


size_t
set_weight (neural_network_t* nn,
            const size_t      row_index,
            const size_t      neuron_index_in_row,
            const size_t      neuron_index_in_next_row,
            const double      value)
{
  size_t index = get_weight_index(nn, row_index, neuron_index_in_row, neuron_index_in_next_row);
  nn->weights[index] = value;
  return index;
}


size_t
print_weight (const neural_network_t* nn,
              const size_t            row_index,
              const size_t            neuron_index_in_row,
              const size_t            neuron_index_in_next_row)
{
  size_t index = get_weight_index(nn, row_index, neuron_index_in_row, neuron_index_in_next_row);
  printf("%f\n",
         nn->weights[retr]);
  return index;
}


size_t
get_weight_index (const neural_network_t* nn,
                  const size_t            row_index,
                  const size_t            neuron_index_in_row,
                  const size_t            neuron_index_in_next_row)
{
  size_t previous_weight_row_size = (row_index == 0) ? 0 : nn->config[row_index - 1] * nn->config[row_index];
  return _2d_to_1d(neuron_index_in_row, neuron_index_in_next_row, nn->config[row_index + 1]) + previous_weight_row_size;
}


size_t
_2d_to_1d (const size_t i,
           const size_t j,
           const size_t j_max)
{
  return i * j_max + j;
}


double
rand_double()
{
  return ((double)rand()/(double)RAND_MAX);
}
/***************************** END IMPL *********************************/


int main (int argc, char** argv)
{
  int config[] = {2,4,1};
  neural_network_t* nn = construct_neural_network(config, 3);
  destruct_neural_network(nn);
  return 0;
}
