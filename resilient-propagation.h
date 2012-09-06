/*!
  \file resilient-propagation.h
  \brief An implementation of the iRPROPPlus resilient propagation algorithm
         used in propagation neural networks.
  \author Hellyna Ng (hellyna@hellyna.com)
  */

#ifndef RESILIENT_PROPAGATION_H_A1214BCC_5A0A_4C3D_9447_D4C8BB8374A1
#define RESILIENT_PROPAGATION_H_A1214BCC_5A0A_4C3D_9447_D4C8BB8374A1

#include "neural-network.h"
#include "training.h"

/*!
  The initial update value used by the algorithm.
  */
#define RPROP_INITIAL_UPDATE      0.1
/*!
  The maximum possible update to a weight.
  */
#define RPROP_DELTA_MAX           50
/*!
  The minimum possible update to a weight.
  */
#define RPROP_DELTA_MIN           1e-6
/*!
  Factor to change the update value if the gradient is negative.
  */
#define RPROP_CHANGE_IF_NEGATIVE  0.5
/*!
  Factor to change the update value if the gradient is negative.
  */
#define RPROP_CHANGE_IF_POSITIVE  1.2
/*!
  The minimum double value considered as zero.
  */
#define RPROP_ZERO_TOLERANCE      0.00000000000000001

/*!
  Data used by this resilient propagation implementation.
  */
struct resilient_propagation_data_t
{
  double           _previous_error;
  double***        _previous_weight_changes;
  double***        _update_values;
};

typedef struct resilient_propagation_data_t resilient_propagation_data_t;

/*!
  Constructs a resilient_propagation_data_t instance, recursively allocating the memory for it.
  \param nn the associated neural_network_t instance to get data essential to the construction from.
  \return a new resilient_propagation_data_t instance.
  */
resilient_propagation_data_t*
construct_resilient_propagation_data (const neural_network_t* nn);

/*!
  Destructs and recursively free a resilient_propagation_data_t instance.
  \param data the resilient_propagation_data_t instance to free and destruct.
  \param nn the associated neural_network_t instance to get data essential to the destruction from.
  */
void
destruct_resilient_propagation_data (resilient_propagation_data_t* data,
                                     const neural_network_t*       nn);

/*!
  The main resilient propagation loop to be inserted as a parameter in train_neural_network()
  \param resilient_propagation_data the \b void pointer to the resilient_propagation_data_t instance.
  \param nn the associated neural_network_t instance to perform the loop on.
  \param training the associated training_t instance to perform the loop on.
  */
void
resilient_propagation_loop (void*                    resilient_propagation_data,
                            const neural_network_t*  nn,
                            const training_t*        training);

#endif
