/*!
  \file training.h
  \brief the extensible propagation trainer for this neural network.
  \author Hellyna Ng (hellyna@hellyna.com)
  */
#ifndef TRAINING_H_D7CB23D4_F49B_11E1_850E_303D6188709B
#define TRAINING_H_D7CB23D4_F49B_11E1_850E_303D6188709B

#include <stdbool.h>

#include "training-set.h"
#include "error-data.h"
#include "neural-network.h"

/*!
  The minimum improvement this training should have over \b DEFAULT_CYCLES_OVER_DEFAULT_MIN_IMPROVEMENT epoches
  */
#define DEFAULT_MIN_IMPROVEMENT 0.0000001
/*!
  The minimum amount of training epoches that the error rate is staying within \b DEFAULT_MIN_IMPROVEMENT
  */
#define DEFAULT_CYCLES_OVER_DEFAULT_MIN_IMPROVEMENT 100

/*!
  The training_t \b struct
  */
struct training_t
{
  double**          _post_activated_sums;
  double**          _pre_activated_sums;
  double**          _deltas;
  /*!
    The gradients for this current training epoch.
    */
  double***         gradients;
  /*!
    The gradients for the previous training epoch.
    */
  double***         previous_gradients;
  /*!
    The pointer to an associated error_data_t instance (for error tracking)
    */
  error_data_t*     error_data;
  double            (*_activation_function) (const double);

  double            (*_derivative_function) (const double,
                                             const double);
  bool              _fix_flat_spot;
};

typedef struct training_t training_t;

/*!
  Constructs and recursively allocating memory for a new training_t instance.
  \param nn the associated neural_network_t instance to derive essential data from.
  \param activation_function the activation function to use from those defined in activation-functions.h
  \param derivative_function the derivative function to use from those defined in activation-functions.h
  \param fix_flat_spot set this to true if you are using a sigmoid activation function.
         Sigmoid activation function are susceptible to the flat-spot problem.
  \return a new training_t instance.
  */
training_t*
construct_training (const neural_network_t* nn,
                    double                  (*activation_function) (const double),
                    double                  (*derivative_function) (const double,
                                                                    const double),
                    const bool              fix_flat_spot);

/*!
  Destructs and recursively free memory for this training_t instance
  \param training the training_t instance to free and destruct.
  \param nn the associated neural_network_t instance to derive essential data from.
  */
void
destruct_training(training_t*             training,
                  const neural_network_t* nn);

/*!
  Trains the associated neural_network_t instance, with the training set instance training_set_t.
  \param training the training_t instance to associate with.
  \param nn the neural_network_t instance to train
  \param ts the training_set_t instance to derive data from and train.
  \param propagation_loop the propagation function. Currently only resilient_propagation_loop() is supported.
  \param propagation_data the data associated with the propagation function to be passed along.
  \param print_every_x_epoch print a message every x epoch. If this value is 0, then no messages are printed.
  \return the final error rate for this training session.
  */
double
train_neural_network (const training_t*       training,
                      const neural_network_t* nn,
                      const training_set_t*   ts,
                      void                    (*propagation_loop) (void*,
                                                                   const neural_network_t*,
                                                                   const training_t*),

                      void* const             propagation_data,
                      const size_t            print_every_x_epoch);
#endif
