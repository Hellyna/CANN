#include <stdlib.h>
#include <stdio.h>

#include "neural-network.h"
#include "activation-functions.h"
#include "resilient-propagation.h"
#include "training-set.h"
#include "training.h"

int main (int argc, char** argv)
{
  (void) argc;
  (void) argv;
  size_t config[] = {2,4,1};
  neural_network_t* nn = construct_neural_network(config, 3, 0.0, 1.0, &initialize_nguyen_widrow_weights);
  training_t* training = construct_training(nn, &sigmoid_activation, &sigmoid_derivative);
  resilient_propagation_data_t* rprop_data = construct_resilient_propagation_data(nn);
  training_set_t* ts = construct_training_set("xor.in", "xor.out");
  printf("Final error rate: %g\n", train_neural_network(training, nn, ts, &resilient_propagation_loop, rprop_data));

  destruct_training(training, nn);
  destruct_resilient_propagation_data(rprop_data, nn);
  destruct_training_set(ts);
  destruct_neural_network(nn);
  return 0;
}
