#ifndef RESILIENT_PROPAGATION_H_A1214BCC_5A0A_4C3D_9447_D4C8BB8374A1
#define RESILIENT_PROPAGATION_H_A1214BCC_5A0A_4C3D_9447_D4C8BB8374A1

#include "neural-network.h"
#include "training.h"

#define RPROP_INITIAL_UPDATE      0.1
#define RPROP_DELTA_MAX           50
#define RPROP_DELTA_MIN           1e-6
#define RPROP_CHANGE_IF_NEGATIVE  0.5
#define RPROP_CHANGE_IF_POSITIVE  1.2
#define RPROP_ZERO_TOLERANCE      0.00000000000000001

struct resilient_propagation_data_t
{
  double           _previous_error;
  double***        _previous_weight_changes;
  double***        _update_values;
};

typedef struct resilient_propagation_data_t resilient_propagation_data_t;

resilient_propagation_data_t*
construct_resilient_propagation_data (const neural_network_t* nn);

void
destruct_resilient_propagation_data (resilient_propagation_data_t* data,
                                     const neural_network_t*       nn);

void
resilient_propagation_loop (void*                    resilient_propagation_data,
                            const neural_network_t*  nn,
                            const training_t*        training);

#endif
