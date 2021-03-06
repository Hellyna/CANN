#undef NN_DEBUG
//#define CANN_DEBUG

#include <stdio.h>

#include <float.h>
#include <stdbool.h>
#include <math.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "util/util.h"
#include "validation.h"

#include "training.h"

training_t*
construct_training (const neural_network_t* nn,
                    double                  (*activation_function) (const double),
                    double                  (*derivative_function) (const double,
                                                                    const double),
                    const bool              fix_flat_spot)
{
  // MALLOC: training
  training_t* training = malloc_exit_if_null(sizeof(training_t));

  // INIT: training->_activation_function
  training->_activation_function = activation_function;

  // INIT: training->_derivative_function
  training->_derivative_function = derivative_function;

  // INIT: training->_fix_flat_spot
  training->_fix_flat_spot = fix_flat_spot;

  // MALLOC: training->_post_activated_sums
  size_t i;
  training->_post_activated_sums = malloc_exit_if_null(nn->config_size * SIZEOF_PTR);
  for (i = 0; i < nn->config_size; ++i)
  {
    training->_post_activated_sums[i] = malloc_exit_if_null(nn->config[i] * sizeof(double));
  }

  // MALLOC: training->_pre_activated_sums
  training->_pre_activated_sums = malloc_exit_if_null(nn->config_size * SIZEOF_PTR);
  for (i = 0; i < nn->config_size; ++i)
  {
    training->_pre_activated_sums[i] = malloc_exit_if_null(nn->config[i] * sizeof(double));
  }

  // MALLOC: training->_deltas
  const size_t num_weight_layers = nn->config_size - 1;
  training->_deltas = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    training->_deltas[i] = malloc_exit_if_null(nn->config[i + 1] * sizeof(double));
  }

  // MALLOC: training->gradients
  training->gradients = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  size_t j;
  for (i = 0; i < num_weight_layers; ++i)
  {
    training->gradients[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      training->gradients[i][j] = calloc_exit_if_null(nn->config[i + 1], sizeof(double));
    }
  }

  // MALLOC: training->previous_gradients
  training->previous_gradients = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    training->previous_gradients[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      training->previous_gradients[i][j] = calloc_exit_if_null(nn->config[i + 1], sizeof(double));
    }
  }

  // MALLOC: nn->error_data
  training->error_data = construct_error_data();

  return training;
}

void
destruct_training(training_t*             training,
                  const neural_network_t* nn)
{
  // FREE: training->error_data
  destruct_error_data(training->error_data);

  // FREE: training->gradients
  size_t i, j;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(training->gradients[i][j]);
    }
    free_and_null(training->gradients[i]);
  }
  free_and_null(training->gradients);

  // FREE: training->previous_gradients
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(training->previous_gradients[i][j]);
    }
    free_and_null(training->previous_gradients[i]);
  }
  free_and_null(training->previous_gradients);

  // FREE: training->_post_activated_sums
  for (i = 0; i < nn->config_size; ++i)
  {
    free_and_null(training->_post_activated_sums[i]);
  }
  free_and_null(training->_post_activated_sums);

  // FREE: training->_pre_activated_sums
  for (i = 0; i < nn->config_size; ++i)
  {
    free_and_null(training->_pre_activated_sums[i]);
  }
  free_and_null(training->_pre_activated_sums);

  // FREE: training->_deltas
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    free_and_null(training->_deltas[i]);
  }
  free_and_null(training->_deltas);

  // FREE: training
  free_and_null(training);
}

static inline void
_update_delta (const training_t*       training,
               const double            error,
               const size_t            current_layer_index,
               const size_t            current_layer_neuron_index)
{
  double flat_spot_fix = 0.0;
  if (training->_fix_flat_spot)
    flat_spot_fix = 0.1;

  training->_deltas[current_layer_index - 1][current_layer_neuron_index] =
    error * ((*(training->_derivative_function))
      (training->_pre_activated_sums[current_layer_index][current_layer_neuron_index],
       training->_post_activated_sums[current_layer_index][current_layer_neuron_index]) + flat_spot_fix);
}

static inline void
_feed_forward (const training_t*        training,
               const neural_network_t*  nn,
               const training_set_t*    ts,
               const size_t             training_set_index)
{
  // current_layer_index
  size_t cli,
  // previous_layer_index
         pli,
  // current_layer_neuron_index
         clni,
  // previous_layer_neuron_index
         plni;

  double sum, target_input;
  cli = 0;
  for (clni = 0; clni < nn->config[cli]; ++clni)
  {
    target_input = ts->target_inputs[training_set_index][clni];
    training->_pre_activated_sums[cli][clni] = target_input;
    training->_post_activated_sums[cli][clni] = target_input;
#ifdef CANN_DEBUG
    printf("Input %d: %g\n", clni, ts->target_inputs[training_set_index][clni]);
#endif
  }

  for (cli = 1; cli < nn->config_size; ++cli)
  {
    pli = cli - 1;
    for (clni = 0; clni < nn->config[cli]; ++clni)
    {
      sum = 0.0;
      for (plni = 0; plni < nn->config[pli]; ++plni)
      {
        sum += training->_post_activated_sums[pli][plni] * nn->weights[pli][plni][clni];
      }
      training->_pre_activated_sums[cli][clni] = sum;
      training->_post_activated_sums[cli][clni] = (*(training->_activation_function)) (sum);
    }
  }
}

static inline void
_process_training_data (const training_t*       training,
                        const neural_network_t* nn,
                        const training_set_t*   ts,
                        const size_t            training_set_index)
{ // current_layer_index
  size_t  cli,
  // next_layer_index
          nli,
  // current_layer_neuron_index,
          clni,
  // next_layer_neuron_index,
          nlni;
  cli = nn->config_size - 1;
  double errors[nn->config[cli]], output, delta;
  for (clni = 0; clni < nn->config[cli]; ++clni)
  {
    errors[clni] =
      update_error(training->error_data,
        ts->target_outputs[training_set_index][clni], training->_post_activated_sums[cli][clni]);
  }

  for (clni = 0; clni < nn->config[cli]; ++clni)
  {
#ifdef CANN_DEBUG
    printf("Trained output %d: %g\n", clni, training->_post_activated_sums[cli][clni]);
    printf("Target output %d: %g\n", clni, ts->target_outputs[training_set_index][clni]);
#endif
    _update_delta(training, errors[clni], cli, clni);
  }

  double error;
  for (cli = nn->config_size - 2; cli > 0; --cli)
  {
    nli = cli + 1;
    for (clni = 0; clni < nn->config[cli]; ++clni)
    {
      error = 0.0;
      output = training->_post_activated_sums[cli][clni];
      for (nlni = 0; nlni < nn->config[nli]; ++nlni)
      {
        delta = training->_deltas[cli][nlni];
        training->gradients[cli][clni][nlni] += output * delta;
        error += nn->weights[cli][clni][nlni] * delta;
      }
      _update_delta(training, error, cli, clni);
    }
  }
#ifdef CANN_DEBUG
  printf("\n");
#endif
}

double
train_neural_network (const training_t*       training,
                      const neural_network_t* nn,
                      const training_set_t*   ts,
                      void                    (*propagation_loop) (void*,
                                                                   const neural_network_t*,
                                                                   const training_t*),

                      void* const             propagation_data,
                      const size_t            print_every_x_epoch)
{
  validate_matching_neural_network_and_training_set(nn, ts);
  double best_error = DBL_MAX;
  double current_error;
  size_t minor_improvement_cycles = 0;
  size_t training_set_index = 0;
  size_t epoch = 0;
  const size_t training_set_size = ts->training_set_size;
  while (true)
  {
    reset_error_data(training->error_data);

    for (training_set_index = 0; training_set_index < training_set_size; ++training_set_index)
    {
      _feed_forward(training, nn, ts, training_set_index);
      _process_training_data(training, nn, ts, training_set_index);
    }
    current_error = calculate_error(training->error_data, MEAN_SQUARE);

    if (fabs(best_error - current_error) < DEFAULT_MIN_IMPROVEMENT)
    {
      ++minor_improvement_cycles;
      if (minor_improvement_cycles > DEFAULT_CYCLES_OVER_DEFAULT_MIN_IMPROVEMENT)
      {
        best_error = fmin(current_error, best_error);
        printf("Training finished at epoch %d.\n", epoch);
        break;
      }
    } else {
      minor_improvement_cycles = 0;
    }

    best_error = fmin(current_error, best_error);
    if (print_every_x_epoch != 0 && epoch % print_every_x_epoch == 0)
    {
      printf("Epoch: %d, ", epoch);
      printf("Current error: %g\n", current_error);
    }
#ifdef CANN_DEBUG
    printf("\n");
#endif
    (*propagation_loop) (propagation_data, nn, training);
#ifdef CANN_DEBUG
    getchar();
#endif
    ++epoch;
  }
  return best_error;
}
