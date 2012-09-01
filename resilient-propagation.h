#ifndef RESILIENT_PROPAGATION_H_A1214BCC_5A0A_4C3D_9447_D4C8BB8374A1
#define RESILIENT_PROPAGATION_H_A1214BCC_5A0A_4C3D_9447_D4C8BB8374A1

#include "neural-network.h"

struct resilient_propagation_data_t
{
  double    previous_error;
  double*** gradients;
  double*** previous_gradients;
  double*** previous_weight_changes;
  double*** update_values;
  double    (*derivative_function) (const double);
};

typedef struct resilient_propagation_data_t resilient_propagation_data_t;

resilient_propagation_data_t*
construct_resilient_propagation_data (const neural_network_t* nn,
                                      double                  (*derivative_function) (const double));

void
destruct_resilient_propagation_data (resilient_propagation_data_t* data,
                                     const neural_network_t*       nn);

void
resilient_propagation_loop (const neural_network_t*  nn,
                            const training_set_t*    ts,
                            const size_t             training_set_index,
                            void*                    resilient_propagation_data);

#endif
