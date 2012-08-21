//#define NDEBUG
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "util/util.h"
#include "neural-network.h"

static const double rPROPInitialUpdate = 0.1;
static const double rPROPDeltaMax = 50;
static const double rPROPDeltaMin = 1e-6;
static const double rPROPChangeIfNegative = 0.5;
static const double rPROPChangeIfPositive = 1.2;
static const double rPROPZeroTolerance = 0.00000000000000001;

training_set_t*
construct_training_set (const char* input_data_path,
                        const char* output_data_path)
{
  // MALLOC: ts
  training_set_t* ts = malloc(sizeof(training_set_t));
  exit_if_null(ts);

  csv_data_t* input_data = construct_csv_data(input_data_path);
  csv_data_t* output_data = construct_csv_data(output_data_path);

  // INIT: ts->training_set_size
  ts->training_set_size = input_data->line_count;

  int i, temp = input_data->entry_counts[0];
  for (i = 1; i < ts->training_set_size; ++i)
  {
    if (temp != input_data->entry_counts[i])
    {
    }
  }

  // INIT: ts->input_size
  ts->input_size = input_size;

  // INIT: ts->output_size
  ts->output_size = output_size;

  // MALLOC: ts->target_inputs
  // INIT: ts->target_inputs
  // MALLOC: ts->target_outputs
  // INIT: ts->target_outputs
  ts->target_inputs = malloc(sizeof(double*) * ts->training_set_size);
  exit_if_null(ts->target_inputs);
  ts->target_outputs = malloc(sizeof(double*) * ts->training_set_size);
  exit_if_null(ts->target_outputs);

  for (i = 0; i < ts->training_set_size; ++i)
  {
    ts->target_inputs[i] = malloc(sizeof(double) * ts->input_size);
    exit_if_null(ts->target_inputs[i]);
    memcpy(ts->target_inputs[i], target_inputs[i], sizeof(double) * ts->input_size);

    ts->target_outputs[i] = malloc(sizeof(double) * ts->output_size);
    exit_if_null(ts->target_outputs[i]);
    memcpy(ts->target_outputs[i], target_outputs[i], sizeof(double) * ts->output_size);
  }

  return ts;
}


void
destruct_training_set (training_set_t* ts)
{
  // FREE: ts->target_inputs
  // FREE: ts->target_outputs
  int i;
  for (i = 0; i < ts->training_set_size; ++i)
  {
    free(ts->target_inputs[i]);
    free(ts->target_outputs[i]);
  }
  free(ts->target_inputs);
  free(ts->target_outputs);

  // FREE: ts
  free(ts);
}


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
        printf("%g ", nn->weights[i][j][k]);
      }
        printf("\n");
    }
      printf("\n");
  }
}

int main (int argc, char** argv)
{
  int config[] = {2,4,1};
  double target_inputs[4][2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1}};
  double target_outputs[4][1] = { {0}, {1}, {1}, {0}};
  neural_network_t* nn = construct_neural_network(config, 3);
  print_weights(nn);
  destruct_neural_network(nn);
  return 0;
}
