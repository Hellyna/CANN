#include <math.h>

#include "activation-functions.h"

double
elliott_activation (const double x)
{
  double temp = x * ELLIOTT_SLOPE;
  return (((0.5 * temp) / (1.0 + fabs(temp))) + 0.5);
}

double
elliott_derivative (const double x)
{
  double temp = 1.0 + fabs(x * ELLIOTT_SLOPE);
  return ELLIOTT_SLOPE / (2.0 * (temp) * (temp));
}

double
elliott_symmetric_activation (const double x)
{
  double temp = x * ELLIOTT_SLOPE;
  return (temp / (1.0 + fabs(temp)));
}

double
elliott_symmetric_derivative (const double x)
{
  double temp = 1.0 + fabs(x * ELLIOTT_SLOPE);
  return ELLIOTT_SLOPE / (temp * temp);
}

double
tanh_activation (const double x)
{
  double temp = exp(x * 2.0);
  return (temp - 1.0) / (temp + 1.0);
}

double
tanh_derivative (const double x)
{
  return 1.0 - pow(tanh_activation(x), 2.0);
}

double
sigmoid_activation (const double x)
{
  return 1.0 / (1.0 + exp(-1.0 * x));
}

double
sigmoid_derivative (const double x)
{
  return x * (1.0 - x);
}

