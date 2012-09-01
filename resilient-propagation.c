#include <math.h>

#include "util/util.h"
#include "util/math.h"

#include "resilient-propagation.h"

const double RPROP_INITIAL_UPDATE = 0.1;
const double RPROP_DELTA_MAX = 50;
const double RPROP_DELTA_MIN = 1e-6;
const double RPROP_CHANGE_IF_NEGATIVE = 0.5;
const double RPROP_CHANGE_IF_POSITIVE = 1.2;
const double RPROP_ZERO_TOLERANCE = 0.00000000000000001;
const double BACKPROP_LEARNING_RATE = 0.3;

resilient_propagation_data_t*
construct_resilient_propagation_data (const neural_network_t* nn,
                                      double                  (*derivative_function) (const double))
{
  // MALLOC: data
  resilient_propagation_data_t* data =
    malloc_exit_if_null(sizeof(resilient_propagation_data_t));
  // INIT: data->derivative_function
  data->derivative_function = derivative_function;

  const size_t num_weight_layers = nn->config_size - 1;

  // MALLOC: data->gradients
  data->gradients = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  size_t i, j;
  for (i = 0; i < num_weight_layers; ++i)
  {
    data->gradients[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      data->gradients[i][j] = calloc_exit_if_null(nn->config[i + 1], sizeof(double));
    }
  }

  // MALLOC: data->previous_gradients
  data->previous_gradients = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    data->previous_gradients[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      data->previous_gradients[i][j] = calloc_exit_if_null(nn->config[i + 1], sizeof(double));
    }
  }

  // MALLOC: data->previous_weight_changes
  data->previous_weight_changes = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    data->previous_weight_changes[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      data->previous_weight_changes[i][j] = malloc_exit_if_null(nn->config[i + 1] * sizeof(double));
    }
  }

  // MALLOC: data->update_values
  size_t k;
  data->update_values = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    data->update_values[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      data->update_values[i][j] = malloc_exit_if_null(nn->config[i + 1] * sizeof(double));
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        data->update_values[i][j][k] = RPROP_INITIAL_UPDATE;
      }
    }
  }

  return data;
}

void
destruct_resilient_propagation_data (resilient_propagation_data_t* data,
                                     const neural_network_t*       nn)
{
  // FREE: data->gradients
  size_t i, j;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(data->gradients[i][j]);
    }
    free_and_null(data->gradients[i]);
  }
  free_and_null(data->gradients);

  // FREE: data->previous_gradients
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(data->previous_gradients[i][j]);
    }
    free_and_null(data->previous_gradients[i]);
  }
  free_and_null(data->previous_gradients);

  // FREE: data->update_values
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(data->update_values[i][j]);
    }
    free_and_null(data->update_values[i]);
  }
  free_and_null(data->update_values);

  // FREE: data->previous_weight_changes
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(data->previous_weight_changes[i][j]);
    }
    free_and_null(data->previous_weight_changes[i]);
  }
  free_and_null(data->previous_weight_changes);

  // FREE: data
  free_and_null(data);
}

static inline short
sign (const double x)
{
  if (fabs(x) < RPROP_ZERO_TOLERANCE)
    return 0;
  else if (x > 0)
    return 1;
  else
    return -1;
}

static inline double
update_weight (const neural_network_t*             nn,
               const resilient_propagation_data_t* data,
               // current_layer_index
               const size_t                        cli,
               // current_layer_neuron_index
               const size_t                        clni,
               // next_layer_neuron_index
               const size_t                        nlni)
{
  // current_weight_layer_index
  double weight_change, delta;
  short gradient_change = sign(data->gradients[cli][clni][nlni] * data->previous_gradients[cli][clni][nlni]);
  switch (gradient_change)
  {
    case 1:
      delta = data->update_values[cli][clni][nlni] * RPROP_CHANGE_IF_POSITIVE;
      delta = fmin(delta, RPROP_DELTA_MAX);
      weight_change = sign(data->gradients[cli][clni][nlni]) * delta;
      data->update_values[cli][clni][nlni] = delta;
      data->previous_gradients[cli][clni][nlni] = data->gradients[cli][clni][nlni];
      break;

    case -1:
      delta = data->update_values[cli][clni][nlni] * RPROP_CHANGE_IF_NEGATIVE;
      delta = fmax(delta, RPROP_DELTA_MIN);
      data->update_values[cli][clni][nlni] = delta;

      if (nn->error_data->square_sum_error > data->previous_error)
        weight_change = -(data->previous_weight_changes[cli][clni][nlni]);

      data->previous_gradients[cli][clni][nlni] = 0.0;
      break;

    default:
      delta = data->update_values[cli][clni][nlni];
      weight_change = sign(data->gradients[cli][clni][nlni]) * delta;
      data->previous_gradients[cli][clni][nlni] = data->gradients[cli][clni][nlni];
  }

  return weight_change;
}

void
resilient_propagation_loop (const neural_network_t*  nn,
                            const training_set_t*    ts,
                            const size_t             training_set_index,
                            void*                    resilient_propagation_data)
{
  resilient_propagation_data_t* data = (resilient_propagation_data_t*) resilient_propagation_data;
  // current_layer_index
  size_t cli,
  // previous_layer_index
         pli,
  // next_layer_index
         nli,
  // current_layer_neuron_index
         clni,
  // next_layer_neuron_index
         nlni;

  double error, delta, weight_change, output;

  for (cli = nn->config_size - 2; cli > 0; --cli)
  {
    pli = cli - 1;
    nli = cli + 1;
    for (clni = 0; clni < nn->config[cli]; ++clni)
    {
      error = 0.0;
      output = nn->feed_forwards[cli][clni];
      for (nlni = 0; nlni < nn->config[nli]; ++nlni)
      {
        delta = nn->layer_deltas[cli][nlni];
        data->gradients[cli][clni][nlni] += output * delta;
        error += nn->weights[cli][clni][nlni] * delta;
      }
      nn->layer_deltas[pli][clni] = error * (*(data->derivative_function)) (nn->layer_sums[pli][clni]);

      for (nlni = 0; nlni < nn->config[nli]; ++nlni)
      {
        weight_change = update_weight(nn, data, cli, clni, nlni);
        data->previous_weight_changes[cli][clni][nlni] = weight_change;
        nn->weights[cli][clni][nlni] += weight_change;
        data->gradients[cli][clni][nlni] = 0;
      }
    }
  }
  data->previous_error = nn->error_data->square_sum_error;
}

