#ifndef ACTIVATION_FUNCTIONS_H_873F8A15_A471_4936_B2F1_FCC6465BFBBA
#define ACTIVATION_FUNCTIONS_H_873F8A15_A471_4936_B2F1_FCC6465BFBBA

#define ELLIOTT_SLOPE 1.0

double
elliott_activation (const double x);

double
elliott_derivative (const double x);

double
elliott_symmetric_activation (const double x);

double
elliott_symmetric_derivative (const double b);

double
tanh_activation (const double x);

double
tanh_derivative (const double x);

double
sigmoid_activation (const double x);

double
sigmoid_derivative (const double x);

#endif