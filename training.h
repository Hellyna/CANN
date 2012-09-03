#ifndef TRAINING_H_D7CB23D4_F49B_11E1_850E_303D6188709B
#define TRAINING_H_D7CB23D4_F49B_11E1_850E_303D6188709B

#include <stdbool.h>

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
  bool              _fix_flat_spot;
};

typedef struct training_t training_t;

training_t*
construct_training (const neural_network_t* nn,
                    double                  (*activation_function) (const double),
                    double                  (*derivative_function) (const double,
                                                                    const double),
                    const bool              fix_flat_spot);

void
destruct_training(training_t* training,
                  const neural_network_t* nn);

double
train_neural_network (const training_t*       training,
                      const neural_network_t* nn,
                      const training_set_t*   ts,
                      void                    (*propagation_loop) (void*,
                                                                   const neural_network_t*,
                                                                   const training_t*),

                      void*                   propagation_data,
                      const size_t            print_every_x_epoch);
#endif
