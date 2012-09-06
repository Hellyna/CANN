/*!
  \file activation-functions.h
  \brief Implements various activation and derivative functions for use in the
  neural network.
  \author Hellyna Ng (hellyna@hellyna.com)
  */
#ifndef ACTIVATION_FUNCTIONS_H_873F8A15_A471_4936_B2F1_FCC6465BFBBA
#define ACTIVATION_FUNCTIONS_H_873F8A15_A471_4936_B2F1_FCC6465BFBBA

/*!
  Defines the slope parameter used internally by elliott_activation(),
  elliott_derivative(), elliott_symmetric_activation() and
  elliott_symmetric_derivative()
  */
#define ELLIOTT_SLOPE 1.0

/*!
  The asymmetric elliott activation function.
  \param x the value to use for activation.
  \return the activated value.
  */
double
elliott_activation (const double x);

/*!
  The asymmetric elliott derivative function.
  \param value_before_activation the value used right before activation.
  \param value_after_activation the value used right after activation.
  \return the derivative value
  */
double
elliott_derivative (const double value_before_activation,
                    const double value_after_activation);

/*!
  The symmetric elliott activation function.
  \param x the value to use for activation.
  \return the activated value.
  */
double
elliott_symmetric_activation (const double x);

/*!
  The symmetric elliott derivative function.
  \param value_before_activation the value used right before activation.
  \param value_after_activation the value used right after activation.
  \return the derivative value
  */
double
elliott_symmetric_derivative (const double value_before_activation,
                              const double value_after_activation);

/*!
  The hyperbolic-tangent activation function.
  \param x the value to use for activation.
  \return the activated value.
  */
double
tanh_activation (const double x);

/*!
  The hyperbolic-tangent derivative function.
  \param value_before_activation the value used right before activation.
  \param value_after_activation the value used right after activation.
  \return the derivative value
  */
double
tanh_derivative (const double value_before_activation,
                 const double value_after_activation);

/*!
  The sigmoid activation function.
  \param x the value to use for activation.
  \return the activated value.
  */
double
sigmoid_activation (const double x);

/*!
  The sigmoid derivative function.
  \param value_before_activation the value used right before activation.
  \param value_after_activation the value used right after activation.
  \return the derivative value
  */
double
sigmoid_derivative (const double value_before_activation,
                    const double value_after_activation);

#endif
