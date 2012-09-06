/*!
  \file neural-network.h
  \brief Creates and maintains neural networks.
  \author Hellyna Ng (hellyna@hellyna.com)
  */
#ifndef NEURAL_NETWORK_H_12AFA1B9_118C_4A47_BFB9_66D964F377ED
#define NEURAL_NETWORK_H_12AFA1B9_118C_4A47_BFB9_66D964F377ED

#include "error-data.h"

/*!
  The neural_network_t \b struct.
  */
struct neural_network_t {
  /*!
    The neural network config array.

    This array states how the neural network structure should be. For example, an array containing:
    \verbatim
    size_t config[4] = {2, 4, 5, 2};
    \endverbatim
    will contain 2 neurons in the input layer, two hidden layers containing 4 and 5 neurons, and one output layer containing 2 neurons.
    */
  size_t*          config;
  /*!
    The neural network config size.

    States the size of the \link config config.
    */
  size_t           config_size;
  /*!
    The neural network weights

    Stores all the weights in the neural network in a 3-dimensional jagged array.

    The first dimension contains the number of neuron layers - 1. The second dimension
    points to the 'from' (current) neuron and the third dimension points to the 'to' neuron.
    */
  double***        weights;
};

typedef struct neural_network_t neural_network_t;

/*!
  Constructs a neural_network_t instance, dynamically allocating all the memory in it recursively.
  \param config the config structure as stated in neural_network_t.
  \param config_size the config size as stated in neural_network_t.
  \param min_weight the absolute possible minimum of the generated weights.
  \param max_weight the absolute possible maximum of the generated weights.
  \param weight_initialization_function an initialization function.
         There are two provided choices: initialize_uniform_weights()
         and initialize_nguyen_widrow_weights(). Or you can write your own.
  \return a neural_network_t instance, or would have exit-ed if any error occurs.
        */
neural_network_t*
construct_neural_network (const size_t* const config,
                          const size_t        config_size,
                          const double        min_weight,
                          const double        max_weight,

                          void                (*weight_initialization_function) (const neural_network_t* const,
                                                                                 const double,
                                                                                 const double));
/*!
  Constructs a neural_network_t instance, dynamically allocating all the memory in it recursively.
  \param path the file in which the weights from a previous training session is stored.
         This file is created by save_neural_network_weights().
  \return a neural_network_t instance, or would have exit-ed if any error occurs.
  */
neural_network_t*
construct_neural_network_from_file (const char* const path);

/*!
  Destructs a neural_network_t instance, recursively free-ing all the dynamically allocated memory in it.
  \param nn the neural_network_t instance to free and destroy.
  */
void
destruct_neural_network (neural_network_t* nn);

/*!
  Initializes a neural_network_t instance with Nguyen-Widrow weights.
  \param nn the neural_network_t instance to initialize.
  \param min_weight the minimum weight possible for this initialization.
  \param max_weight the maximum weight possible for this initialization.
  */
void
initialize_nguyen_widrow_weights (const neural_network_t* const nn,
                                  const double                  min_weight,
                                  const double                  max_weight);

/*!
  Initializes a neural_network_t instance with uniformly-distributed weights.
  \param nn the neural_network_t instance to initialize.
  \param min_weight the minimum weight possible for this initialization.
  \param max_weight the maximum weight possible for this initialization.
  */
void
initialize_uniform_weights (const neural_network_t* const nn,
                            const double                  min_weight,
                            const double                  max_weight);

/*!
  Saves the weights of the current neural_network_t instance to a file.
  \param nn the neural_network_t instance to save.
  \param path the path + filename to save to.
  */
void
save_neural_network_weights (const neural_network_t*  const nn,
                             const char*              const path);

/*!
  Loads the weights from a file previously saved by save_neural_network_weights(),
  into a compatible neural_network_t instance.

  This function will exit if any error in the file is encounted.

  \param nn the neural_network_t instance to load into.
  \param path the path + filename to load from.
  */
void
load_neural_network_weights (const neural_network_t*  const nn,
                             const char*              const path);
#endif
