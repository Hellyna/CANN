#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

struct training_set_t {
  size_t    training_set_size;
  size_t    input_size;
  double**  target_inputs;
  size_t    output_size;
  double**  target_outputs;
};

struct neural_network_t {
  int*      config;
  size_t    config_size;

  double*** weights;
  double    min_weight;
  double    max_weight;

  double**  feed_forwards;
  double**  layer_sums;
  double**  layer_deltas;
};

typedef struct training_set_t   training_set_t;
typedef struct neural_network_t neural_network_t;

training_set_t*
construct_training_set (const char* input_data_path,
                        const char* output_data_path);

void
destruct_training_set (training_set_t* ts);

neural_network_t*
construct_neural_network (const int*    config,
                          const size_t  config_size);

neural_network_t*
construct_neural_network_full (const int*   config,
                               const size_t config_size,
                               const double min_weight,
                               const double max_weight,
                               void  (*initialize_weights) (const neural_network_t*));

void
destruct_neural_network (neural_network_t* nn);

void
initialize_nguyen_widrow_weights (const neural_network_t* nn);

void
initialize_uniform_weights (const neural_network_t* nn);

void
train_neural_network (const neural_network_t* nn,
                      const training_set_t*   ts,
                      const double            threshold_error);
void
train_neural_network_full (const neural_network_t*  nn,
                           const training_set_t*    ts,
                           double                   threshold_error,
                           double                   (*error_function) (const double,
                                                                       const double));

double
linear_error (const double target_value,
              const double trained_value);

double
tanh_error (const double target_value,
            const double trained_value);

#endif /* NEURAL_NETWORK_H */
