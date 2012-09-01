//#undef NN_DEBUG
#define NN_DEBUG

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "util/util.h"
#include "libcsv/csv.h"
#include "resilient-propagation.h"
#include "activation-functions.h"
#include "neural-network.h"

#define DEFAULT_MIN_IMPROVEMENT 0.0000001
#define DEFAULT_CYCLES_OVER_DEFAULT_MIN_IMPROVEMENT 100

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
  size_t i, temp = input_data->entry_counts[0];
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
  ts->target_inputs = malloc_exit_if_null(SIZEOF_PTR * ts->training_set_size);
  ts->target_outputs = malloc_exit_if_null(SIZEOF_PTR * ts->training_set_size);

  size_t j;
  for (i = 0; i < ts->training_set_size; ++i)
  {
    ts->target_inputs[i] = malloc_exit_if_null(ts->input_size * sizeof(double));
    for (j = 0; j < ts->input_size; ++j)
    {
      ts->target_inputs[i][j] = atof(input_data->data[i + 1][j]);
#ifdef NN_DEBUG
      printf("ts->target_inputs[%d][%d]: %g %s\n", i, j, ts->target_inputs[i][j], input_data->data[i + 1][j]);
#endif
    }

    ts->target_outputs[i] = malloc_exit_if_null(ts->output_size * sizeof(double));
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
  size_t i;
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


neural_network_t*
construct_neural_network (const size_t* config,
                          const size_t  config_size)
{
  return construct_neural_network_full(config, config_size, 0.0, 1.0, &initialize_nguyen_widrow_weights);
}


neural_network_t*
construct_neural_network_full (const size_t* config,
                               const size_t  config_size,
                               const double  min_weight,
                               const double  max_weight,
                               void          (*initialize_weights) (const neural_network_t*))
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
  (*initialize_weights) (nn);

  // MALLOC: nn->feed_forwards
  nn->feed_forwards = malloc_exit_if_null(nn->config_size * SIZEOF_PTR);
  for (i = 0; i < nn->config_size; ++i)
  {
    nn->feed_forwards[i] = malloc_exit_if_null(nn->config[i] * sizeof(double));
  }

  // MALLOC: nn->layer_sums
  nn->layer_sums = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    nn->layer_sums[i] = malloc_exit_if_null(nn->config[i + 1] * sizeof(double));
  }

  // MALLOC: nn->layer_deltas
  nn->layer_deltas = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    nn->layer_deltas[i] = malloc_exit_if_null(nn->config[i + 1] * sizeof(double));
  }

  // MALLOC: nn->error_data
  nn->error_data = construct_error_data();

  return nn;
}


void
destruct_neural_network (neural_network_t* nn)
{
  // FREE: nn->error_data
  destruct_error_data(nn->error_data);

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

  // FREE: nn->feed_forwards
  for (i = 0; i < nn->config_size; ++i)
  {
    free_and_null(nn->feed_forwards[i]);
  }
  free_and_null(nn->feed_forwards);

  // FREE: nn->layer_sums
  for (i = 0; i < num_weight_layers; ++i)
  {
    free_and_null(nn->layer_sums[i]);
  }
  free_and_null(nn->layer_sums);

  // FREE: nn->layer_deltas
  for (i = 0; i < num_weight_layers; ++i)
  {
    free_and_null(nn->layer_deltas[i]);
  }
  free_and_null(nn->layer_deltas);

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


static inline void
feed_forward (const neural_network_t* nn,
              const training_set_t*   ts,
              const size_t            training_set_index,
              double                  (*activation_function) (const double))
{
  size_t i, j, k;
  double sum;
  for (i = 0; i < nn->config[0]; ++i)
  {
    nn->feed_forwards[0][i] = ts->target_inputs[training_set_index][i];
#ifdef NN_DEBUG
    printf("Input %d: %g\n", i, ts->target_inputs[training_set_index][i] );
#endif
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
      nn->layer_sums[i - 1][j] = sum;
      nn->feed_forwards[i][j] = (*activation_function) (sum);
    }
  }
}

static inline void
calculate_deltas (const neural_network_t* nn,
                  const training_set_t*   ts,
                  const size_t            training_set_index,
                  double                  (*derivative_function) (const double))
{
  reset_error_data(nn->error_data);
  size_t j;
  double error;
  const size_t last_neuron_row = nn->config_size - 1;
  const size_t last_weight_row = last_neuron_row - 1;
  for (j = 0; j < nn->config[last_neuron_row]; ++j)
  {
#ifdef NN_DEBUG
    printf("Trained output %d: %g\n", j, nn->feed_forwards[last_neuron_row][j]);
#endif
    error =
      update_error(nn->error_data,
        ts->target_outputs[training_set_index][j], nn->feed_forwards[last_neuron_row][j]);
#ifdef NN_DEBUG
    printf("Target output %d: %g\n", j, ts->target_outputs[training_set_index][j]);
#endif
    nn->layer_deltas[last_weight_row][j] =
      ((*derivative_function) (nn->layer_sums[last_weight_row][j])) * error;
  }
#ifdef NN_DEBUG
  printf("\n");
#endif
}

void
train_neural_network (const neural_network_t* nn,
                      const training_set_t*   ts,
                      double                  (*activation_function)  (const double),

                      double                  (*derivative_function)  (const double),

                      void                    (*propagation_loop)     (const neural_network_t*,
                                                                       const training_set_t*,
                                                                       const size_t,
                                                                       void*),

                      void*                   propagation_data)
{
  double best_error = DBL_MAX;
  double current_error;
  size_t minor_improvement_cycles = 0;
  size_t training_set_index = 0;
  size_t training_set_size = ts->training_set_size;
  while (true)
  {
    if (training_set_index == training_set_size)
    {
      training_set_index = 0;
    }
    feed_forward(nn, ts, training_set_index, activation_function);
    calculate_deltas(nn, ts, training_set_index, derivative_function);
    current_error = calculate_error(nn->error_data, MEAN_SQUARE);

    if (fabs(best_error - current_error) < DEFAULT_MIN_IMPROVEMENT)
    {
      ++minor_improvement_cycles;
      if (minor_improvement_cycles > DEFAULT_CYCLES_OVER_DEFAULT_MIN_IMPROVEMENT)
        break;

    } else {
      minor_improvement_cycles = 0;
    }

    best_error = fmin(current_error, best_error);
#ifdef NN_DEBUG
    printf("Current error: %g\n", current_error);
    printf("\n");
#endif
    (*propagation_loop) (nn, ts, training_set_index, propagation_data);
    ++training_set_index;
  }
}


int main (int argc, char** argv)
{
  size_t config[] = {2,4,1};
  neural_network_t* nn = construct_neural_network(config, 3);
  resilient_propagation_data_t* rprop_data = construct_resilient_propagation_data(nn, &elliott_derivative);
  training_set_t* ts = construct_training_set("xor.in", "xor.out");
  train_neural_network(nn, ts, &elliott_activation, &elliott_derivative, &resilient_propagation_loop, rprop_data);

  destruct_resilient_propagation_data(rprop_data, nn);
  destruct_training_set(ts);
  destruct_neural_network(nn);
  return 0;
}
