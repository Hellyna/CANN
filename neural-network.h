#ifndef NEURAL_NETWORK_H_12AFA1B9_118C_4A47_BFB9_66D964F377ED
#define NEURAL_NETWORK_H_12AFA1B9_118C_4A47_BFB9_66D964F377ED

#include "error-data.h"

struct training_set_t {
  size_t    training_set_size;
  size_t    input_size;
  double**  target_inputs;
  size_t    output_size;
  double**  target_outputs;
};

struct neural_network_t {
  size_t*   config;
  size_t    config_size;

  double*** weights;
  double    min_weight;
  double    max_weight;

  double**  feed_forwards;
  double**  layer_sums;
  double**  layer_deltas;

  error_data_t* error_data;
};

typedef struct training_set_t   training_set_t;
typedef struct neural_network_t neural_network_t;

training_set_t*
construct_training_set (const char* input_data_path,
                        const char* output_data_path);

void
destruct_training_set (training_set_t* ts);

neural_network_t*
construct_neural_network (const size_t* config,
                          const size_t  config_size);

neural_network_t*
construct_neural_network_full (const size_t* config,
                               const size_t  config_size,
                               const double  min_weight,
                               const double  max_weight,
                               void          (*initialize_weights) (const neural_network_t*));

void
destruct_neural_network (neural_network_t* nn);

void
initialize_nguyen_widrow_weights (const neural_network_t* nn);

void
initialize_uniform_weights (const neural_network_t* nn);

void
train_neural_network (const neural_network_t* nn,
                      const training_set_t*   ts,
                      double                  (*activation_function)  (const double),

                      double                  (*derivative_function)  (const double),

                      void                    (*propagation_loop)     (const neural_network_t*,
                                                                       const training_set_t*,
                                                                       const size_t,
                                                                       void*),
                      void*                   propagation_data);

#endif
