//#define NDEBUG
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "util/util.h"
#include "libcsv/csv.h"
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
  training_set_t* ts = malloc_exit_if_null(sizeof(training_set_t));

  csv_data_t* input_data = construct_csv_data(input_data_path);
  csv_data_t* output_data = construct_csv_data(output_data_path);

  // VALIDATE: training_set_size
  if (input_data->line_count != output_data->line_count)
  {
    printferr_and_exit("Input and output data file entries are not equal.\n");
  }

  // INIT: ts->training_set_size
  ts->training_set_size = input_data->line_count - 1;

  // VALIDATE: ts->input_size
  int i, temp = input_data->entry_counts[0];
  for (i = 1; i < ts->training_set_size; ++i)
  {
    if (temp != input_data->entry_counts[i])
    {
      printferr_and_exit("Malformed input data file.\n");
    }
    temp = input_data->entry_counts[i];
  }

  // INIT: ts->input_size
  ts->input_size = temp;

  // VALIDATE: ts->output_size
  temp = output_data->entry_counts[0];
  for (i = 1; i < ts->training_set_size; ++i)
  {
    if (temp != output_data->entry_counts[i])
    {
      printferr_and_exit("Malformed output data file.\n");
    }
    temp = output_data->entry_counts[i];
  }

  // INIT: ts->output_size
  ts->output_size = temp;

  // MALLOC: ts->target_inputs
  // INIT: ts->target_inputs
  // MALLOC: ts->target_outputs
  // INIT: ts->target_outputs
  ts->target_inputs = malloc_exit_if_null(sizeof(double*) * ts->training_set_size);
  ts->target_outputs = malloc_exit_if_null(sizeof(double*) * ts->training_set_size);

  int j;
  for (i = 0; i < ts->training_set_size; ++i)
  {
    ts->target_inputs[i] = calloc_exit_if_null(ts->input_size, sizeof(double));
    for (j = 0; j < ts->input_size; ++j)
    {
      ts->target_inputs[i][j] = atof(input_data->data[i + 1][j]);
    }

    ts->target_outputs[i] = calloc_exit_if_null(ts->output_size, sizeof(double));
    for (j = 0; j < ts->output_size; ++j)
    {
      ts->target_outputs[i][j] = atof(output_data->data[i + 1][j]);
    }
  }

  destruct_csv_data(input_data);
  destruct_csv_data(output_data);
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
    free_and_null(ts->target_inputs[i]);
    free_and_null(ts->target_outputs[i]);
  }
  free_and_null(ts->target_inputs);
  free_and_null(ts->target_outputs);

  // FREE: ts
  free_and_null(ts);
}


inline neural_network_t*
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
  neural_network_t* nn = malloc_exit_if_null(sizeof(neural_network_t));

  // INIT: nn->config_size
  nn->config_size = config_size;

  // MALLOC: nn->config
  nn->config = malloc_exit_if_null(sizeof(int) * config_size);

  // INIT: nn->config
  memcpy(nn->config, config, sizeof(int) * config_size);

  // INIT: nn->min_weight
  nn->min_weight = min_weight;

  // INIT: nn->max_weight
  nn->max_weight = max_weight;

  // MALLOC: nn->weights
  nn->weights = malloc_exit_if_null((nn->config_size - 1) * SIZEOF_PTR);

  size_t i, j;
  for (i = 0; i < nn->config_size - 1; ++i)
    {
    nn->weights[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      nn->weights[i][j] = malloc_exit_if_null(sizeof(double) * nn->config[i + 1]);
    }
  }

  // INIT: nn->weights
  initialize_nguyen_widrow_weights(nn);

  // MALLOC: nn->feed_forwards
  nn->feed_forwards = malloc_exit_if_null(nn->config_size * SIZEOF_PTR);
  for (i = 0; i < nn->config_size; ++i)
  {
    nn->feed_forwards[i] = malloc_exit_if_null(nn->config[i] * sizeof(double));
  }
  return nn;
}


void
destruct_neural_network (neural_network_t* nn)
{
  // FREE: nn->weights
  size_t i, j;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(nn->weights[i][j]);
    }
    free_and_null(nn->weights[i]);
  }
  free_and_null(nn->weights);

  // FREE: nn->feed_forwards
  for (i = 0; i < nn->config_size; ++i)
  {
    free_and_null(nn->feed_forwards[i]);
  }
  free_and_null(nn->feed_forwards);

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


inline void
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


inline void
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

inline double
elliott (double x, double s, bool is_symmetric)
{
  return (is_symmetric) ? ((x * s) / (1.0 + fabs(x * s)))
                        : ((0.5 * (x * s) / (1.0 + fabs(x * s))) + 0.5);
}

inline void
feed_forward (const neural_network_t* nn,
              const training_set_t*   ts,
              const size_t            training_set_index)
{
  static int i, j, k;
  static double sum;
  for (i = 0; i < nn->config[0]; ++i)
  {
    nn->feed_forwards[0][i] = ts->target_inputs[training_set_index][i];
  }

  for (i = 1; i < nn->config_size; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      sum = 0.0;
      for (k = 0; k < nn->config[i - 1]; ++k)
      {
        sum += nn->feed_forwards[i - 1][k] * nn->weights[i - 1][k][j];
      }
      nn->feed_forwards[i][j] = elliott(sum, 1.0, false);
    }
  }
}

inline double
resilient_propagate (const neural_network_t*  nn,
                     const training_set_t*    ts,
                     const size_t             training_set_index)
{
  return 0.0;
}

void
train (neural_network_t*  nn,
       training_set_t*    ts,
       double             threshold_error)
{
  double maximum_error = 999999;
  double current_error;
  bool looping = true;
  size_t training_set_index = 0;
  size_t training_set_size = ts->training_set_size;
  while (looping)
  {
    if (training_set_index >= training_set_size - 1)
    {
      training_set_index = 0;
    }
    feed_forward(nn, ts, training_set_index);
    current_error = resilient_propagate(nn, ts, training_set_index);
    looping = current_error > maximum_error;
    if (looping)
    {
      maximum_error = current_error;
    }
  }
}


int main (int argc, char** argv)
{
  int config[] = {2,4,1};
  neural_network_t* nn = construct_neural_network(config, 3);
  training_set_t* ts = construct_training_set("xor.in", "xor.out");
  destruct_training_set(ts);
  print_weights(nn);
  destruct_neural_network(nn);
  return 0;
}
