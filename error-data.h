/*!
  \file error-data.h
  \brief Handles the calculation of error data in the neural network.
  \author Hellyna Ng (hellyna@hellyna.com)
  */
#ifndef ERROR_DATA_H_8D1C1F7B_182C_4487_8CD9_9F053221AC6C
#define ERROR_DATA_H_8D1C1F7B_182C_4487_8CD9_9F053221AC6C

#include <stddef.h>

/*!
  An \b enum specifying the error calculation mode.
  */
enum error_calculation_t
{
  /*!
    The mean-square error calculation mode.
    */
  MEAN_SQUARE,
  /*!
    The root-mean-square error calculation mode.
    */
  ROOT_MEAN_SQUARE,
  /*!
    The sum-of-squares error calculation mode.
    */
  SUM_OF_SQUARES
};

/*!
  A \b struct storing the error information vital for the calculation of error.
  */
struct error_data_t
{
  double square_sum_error;
  size_t square_sum_error_count;
};

typedef struct error_data_t error_data_t;
typedef enum error_calculation_t error_calculation_t;

/*!
  Constructs, initializes and recursively allocate memory for an empty
  error_data_t instance.
  \return a new error_data_t instance.
  */
error_data_t*
construct_error_data ();

/*!
  Destructs and recursively free memory for an error_data_t instance.
  \param errord the error_data_t instance to destruct and free.
  */
void
destruct_error_data (error_data_t* errord);

/*!
  Reinitializes an error_data_t instance.
  \param errord the error_data_t instance to reinitialize.
  */
void
reset_error_data (error_data_t* errord);

/*!
  Updates the error_data_t instance with the provided ideal and actual data.
  \param errord the error_data_t instance to update.
  \param ideal the ideal value.
  \param actual the actual value.
  \return a error rate between the ideal and actual. (ideal - actual).
  */
double
update_error (error_data_t* errord,
              const double  ideal,
              const double  actual);

/*!
  Calculates the specified error type from an error_data_t instance.
  \param errord the error_data_t instance to calculate from.
  \param error_calculation_type the error_calculation_t enum type.
  \return the calculated error.
  */
double
calculate_error (const error_data_t*        errord,
                 const error_calculation_t  error_calculation_type);
#endif
