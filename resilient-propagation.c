#include <math.h>

#include "util/util.h"
#include "util/math.h"

#include "resilient-propagation.h"

resilient_propagation_data_t*
construct_resilient_propagation_data (const neural_network_t* nn)
{
  // MALLOC: data
  resilient_propagation_data_t* data =
    malloc_exit_if_null(sizeof(resilient_propagation_data_t));

  // INIT: data->_previous_error
  data->_previous_error = 0.0;

  const size_t num_weight_layers = nn->config_size - 1;

  size_t i, j;
  // MALLOC: data->_previous_weight_changes
  data->_previous_weight_changes = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    data->_previous_weight_changes[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      data->_previous_weight_changes[i][j] = malloc_exit_if_null(nn->config[i + 1] * sizeof(double));
    }
  }

  // MALLOC: data->_update_values
  size_t k;
  data->_update_values = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);
  for (i = 0; i < num_weight_layers; ++i)
  {
    data->_update_values[i] = malloc_exit_if_null(nn->config[i] * SIZEOF_PTR);
    for (j = 0; j < nn->config[i]; ++j)
    {
      data->_update_values[i][j] = malloc_exit_if_null(nn->config[i + 1] * sizeof(double));
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        data->_update_values[i][j][k] = RPROP_INITIAL_UPDATE;
      }
    }
  }

  return data;
}

void
destruct_resilient_propagation_data (resilient_propagation_data_t* data,
                                     const neural_network_t*       nn)
{
  size_t i, j;
  // FREE: data->_update_values
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(data->_update_values[i][j]);
    }
    free_and_null(data->_update_values[i]);
  }
  free_and_null(data->_update_values);

  // FREE: data->_previous_weight_changes
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(data->_previous_weight_changes[i][j]);
    }
    free_and_null(data->_previous_weight_changes[i]);
  }
  free_and_null(data->_previous_weight_changes);

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
update_weight (const resilient_propagation_data_t* data,
               const training_t*                   training,
               // current_layer_index
               const size_t                        cli,
               // current_layer_neuron_index
               const size_t                        clni,
               // next_layer_neuron_index
               const size_t                        nlni)
{
  // current_weight_layer_index
  double weight_change = 0.0, delta;
  short gradient_change = sign(training->gradients[cli][clni][nlni] * training->previous_gradients[cli][clni][nlni]);
  switch (gradient_change)
  {
    case 1:
      delta = data->_update_values[cli][clni][nlni] * RPROP_CHANGE_IF_POSITIVE;
      delta = fmin(delta, RPROP_DELTA_MAX);
      weight_change = sign(training->gradients[cli][clni][nlni]) * delta;
      data->_update_values[cli][clni][nlni] = delta;
      training->previous_gradients[cli][clni][nlni] = training->gradients[cli][clni][nlni];
      break;

    case -1:
      delta = data->_update_values[cli][clni][nlni] * RPROP_CHANGE_IF_NEGATIVE;
      delta = fmax(delta, RPROP_DELTA_MIN);
      data->_update_values[cli][clni][nlni] = delta;

      if (training->error_data->square_sum_error > data->_previous_error)
        weight_change = -(data->_previous_weight_changes[cli][clni][nlni]);

      training->previous_gradients[cli][clni][nlni] = 0.0;
      break;

    default:
      delta = data->_update_values[cli][clni][nlni];
      weight_change = sign(training->gradients[cli][clni][nlni]) * delta;
      training->previous_gradients[cli][clni][nlni] = training->gradients[cli][clni][nlni];
  }

  return weight_change;
}

void
resilient_propagation_loop (void*                    resilient_propagation_data,
                            const neural_network_t*  nn,
                            const training_t*        training)
{
  resilient_propagation_data_t* data = (resilient_propagation_data_t*) resilient_propagation_data;
  // current_layer_index
  size_t cli,
  // next_layer_index
         nli,
  // current_layer_neuron_index
         clni,
  // next_layer_neuron_index
         nlni;

  double weight_change;
  for (cli = nn->config_size - 2; cli > 0; --cli)
  {
    nli = cli + 1;
    for (clni = 0; clni < nn->config[cli]; ++clni)
    {
      for (nlni = 0; nlni < nn->config[nli]; ++nlni)
      {
        weight_change = update_weight(data, training, cli, clni, nlni);
        data->_previous_weight_changes[cli][clni][nlni] = weight_change;
        nn->weights[cli][clni][nlni] += weight_change;
        training->gradients[cli][clni][nlni] = 0.0;
      }
    }
  }
  data->_previous_error = training->error_data->square_sum_error;
}

