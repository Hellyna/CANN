#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
/****************************** BEGIN DECLS *******************************/
struct neural_network_t {
  int* config;
  size_t config_size;

  double*** weights;
  double min_weight;
  double max_weight;
};

typedef struct neural_network_t neural_network_t;

neural_network_t*
construct_neural_network (const int*    config,
                          const size_t  config_size);

neural_network_t*
construct_neural_network_full (const int*   config,
                               const size_t config_size,
                               const double min_weight,
                               const double max_weight);

void
destruct_neural_network (neural_network_t* nn);

void
initialize_nguyen_widrow_weights (const neural_network_t* nn);

void
initialize_uniform_weights (const neural_network_t* nn);

void
print_weights (const neural_network_t* nn);

double
rand_double();

void
exit_if_null (void* p);

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
                               const double min_weight,
                               const double max_weight)
{
  // MALLOC: nn
  neural_network_t* nn = malloc(sizeof(neural_network_t));
  exit_if_null(nn);

  // INIT: nn->config_size
  nn->config_size = config_size;

  // MALLOC: nn->config
  nn->config = malloc(sizeof(int) * config_size);
  exit_if_null(nn->config);

  // INIT: nn->config
  memcpy(nn->config, config, sizeof(int) * config_size);

  // INIT: nn->min_weight
  nn->min_weight = min_weight;

  // INIT: nn->max_weight
  nn->max_weight = max_weight;

  // MALLOC: nn->weights
  nn->weights = malloc(sizeof(double**) * nn->config_size - 1);
  exit_if_null(nn->weights);

  size_t i, j;
  for (i = 0; i < nn->config_size - 1; ++i)
    {
    nn->weights[i] = malloc(sizeof(double*) * nn->config[i]);
    exit_if_null(nn->weights[i]);
    for (j = 0; j < nn->config[i]; ++j)
    {
      nn->weights[i][j] = malloc(sizeof(double) * nn->config[i + 1]);
      exit_if_null(nn->weights[i][j]);
    }
  }
  // INIT: nn->weights
  initialize_nguyen_widrow_weights(nn);
  //initialize_uniform_weights(nn);

  return nn;
}


void
destruct_neural_network (neural_network_t* nn)
{
  // FREE: weights
  size_t i, j;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free(nn->weights[i][j]);
      nn->weights[i][j] = NULL;
    }
    free(nn->weights[i]);
    nn->weights[i] = NULL;
  }
  free(nn->weights);
  nn->weights = NULL;

  // FREE: config
  free(nn->config);
  nn->config = NULL;

  // FREE: nn
  free(nn);
  nn = NULL;
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


void
print_weights (const neural_network_t* nn) {
  size_t i, j, k;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      for(k = 0; k < nn->config[i + 1]; ++k)
      {
        printf("%f ", nn->weights[i][j][k]);
      }
        printf("\n");
    }
      printf("\n");
  }
}


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

/***************************** END IMPL *********************************/


int main (int argc, char** argv)
{
  int config[] = {2,4,1};
  neural_network_t* nn = construct_neural_network(config, 3);
  print_weights(nn);
  destruct_neural_network(nn);
  return 0;
}
