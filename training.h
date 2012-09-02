#ifndef TRAINING_H_D7CB23D4_F49B_11E1_850E_303D6188709B
#define TRAINING_H_D7CB23D4_F49B_11E1_850E_303D6188709B

#include "training-set.h"
#include "error-data.h"
#include "neural-network.h"

#define DEFAULT_MIN_IMPROVEMENT 0.0000001
#define DEFAULT_CYCLES_OVER_DEFAULT_MIN_IMPROVEMENT 100

struct training_t
{
  double**          _post_activated_sums;
  double**          _pre_activated_sums;
  double**          _deltas;
  double***         gradients;
  double***         previous_gradients;
  error_data_t*     error_data;
  double            (*_activation_function) (const double);

  double            (*_derivative_function) (const double,
                                             const double);
};

typedef struct training_t training_t;

training_t*
construct_training ();

void
destruct_training();

double
train_neural_network (const training_t*       training,
                      const neural_network_t* nn,
                      const training_set_t*   ts,
                      void                    (*propagation_loop) (void*,
                                                                   const neural_network_t*,
                                                                   const training_t*),

                      void*                   propagation_data);
#endif
