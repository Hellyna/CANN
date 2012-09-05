#ifndef NEURAL_NETWORK_H_12AFA1B9_118C_4A47_BFB9_66D964F377ED
#define NEURAL_NETWORK_H_12AFA1B9_118C_4A47_BFB9_66D964F377ED

#include "error-data.h"

struct neural_network_t {
  size_t*          config;
  size_t           config_size;

  double***        weights;
  double           min_weight;
  double           max_weight;
};

typedef struct neural_network_t neural_network_t;

neural_network_t*
construct_neural_network (const size_t* const config,
                          const size_t        config_size,
                          const double        min_weight,
                          const double        max_weight,

                          void          (*weight_initialization_function) (const neural_network_t* const));

void
destruct_neural_network (neural_network_t* nn);

void
initialize_nguyen_widrow_weights (const neural_network_t* const nn);

void
initialize_uniform_weights (const neural_network_t* const nn);

void
save_neural_network_weights (const neural_network_t*  const nn,
                             const char*              const path);

void
load_neural_network_weights (const neural_network_t*  const nn,
                             const char*              const path);
#endif
