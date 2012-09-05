#ifndef ERROR_DATA_H_8D1C1F7B_182C_4487_8CD9_9F053221AC6C
#define ERROR_DATA_H_8D1C1F7B_182C_4487_8CD9_9F053221AC6C

#include <stddef.h>

enum error_calculation_t
{
  MEAN_SQUARE,
  ROOT_MEAN_SQUARE,
  SUM_OF_SQUARES
};

struct error_data_t
{
  double square_sum_error;
  size_t square_sum_error_count;
};

typedef struct error_data_t error_data_t;
typedef enum error_calculation_t error_calculation_t;

error_data_t*
construct_error_data ();

void
destruct_error_data (error_data_t* errord);

void
reset_error_data (error_data_t* errord);

double
update_error (error_data_t* errord,
              const double  ideal,
              const double  actual);

double
calculate_error (const error_data_t* errord,
                 const error_calculation_t       error_calculation_type);
#endif
