#include <math.h>

#include "util/util.h"

#include "error-data.h"

error_data_t*
construct_error_data ()
{
  // MALLOC: errord
  error_data_t* errord = malloc_exit_if_null(sizeof(error_data_t));
  reset_error_data(errord);
  return errord;
}

void
destruct_error_data (error_data_t* errord)
{
  // FREE: errord
    free_and_null(errord);
}

inline void
reset_error_data (error_data_t* errord)
{
  errord->square_sum_error = 0.0;
  errord->square_sum_error_count = 0;
}

inline double
update_error (error_data_t* errord,
              const double  actual,
              const double  ideal)
{
  double error = ideal - actual;
  errord->square_sum_error += error * error;
  ++(errord->square_sum_error_count);
  return error;
}

static inline double
calculate_mean_square_error (const error_data_t* errord)
{
  return errord->square_sum_error / errord->square_sum_error_count;
}

static inline double
calculate_root_mean_square_error (const error_data_t* errord)
{
  return sqrt(calculate_mean_square_error(errord));
}

static inline double
calculate_sum_of_squares_error (const error_data_t* errord)
{
  return errord->square_sum_error / 2;
}

double
calculate_error (const error_data_t* errord,
                 const error_calculation_t       error_calculation_type)
{
  if (errord->square_sum_error_count == 0)
    return 0;

  double error;
  switch (error_calculation_type)
  {
    case MEAN_SQUARE:
      error = calculate_mean_square_error(errord);
      break;
    case ROOT_MEAN_SQUARE:
      error = calculate_root_mean_square_error(errord);
      break;
    case SUM_OF_SQUARES:
      error = calculate_sum_of_squares_error(errord);
      break;
    default:
      error = -1.0;
  }
  return error;
}

