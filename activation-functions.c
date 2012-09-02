#include <math.h>

#include "activation-functions.h"

double
elliott_activation (const double x)
{
  double temp = x * ELLIOTT_SLOPE;
  return (((0.5 * temp) / (1.0 + fabs(temp))) + 0.5);
}

double
elliott_derivative (const double value_before_activation,
                    const double value_after_activation)
{
  (void) value_after_activation;
  double temp = 1.0 + fabs(value_before_activation * ELLIOTT_SLOPE);
  return ELLIOTT_SLOPE / (2.0 * (temp) * (temp));
}

double
elliott_symmetric_activation (const double x)
{
  double temp = x * ELLIOTT_SLOPE;
  return (temp / (1.0 + fabs(temp)));
}

double
elliott_symmetric_derivative (const double value_before_activation,
                              const double value_after_activation)
{
  (void) value_after_activation;
  double temp = 1.0 + fabs(value_before_activation * ELLIOTT_SLOPE);
  return ELLIOTT_SLOPE / (temp * temp);
}

double
tanh_activation (const double x)
{
  return tanh(x);
}

double
tanh_derivative (const double value_before_activation,
                 const double value_after_activation)
{
  (void) value_before_activation;
  return (1.0 - value_after_activation * value_after_activation);
}

double
sigmoid_activation (const double x)
{
  return 1.0 / (1.0 + exp(-1.0 * x));
}

double
sigmoid_derivative (const double value_before_activation,
                    const double value_after_activation)
{
  (void) value_before_activation;
  return value_after_activation * (1.0 - value_after_activation);
}

