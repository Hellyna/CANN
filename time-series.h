/*!
  \file time_series.h
  \brief Manages time series data.
  \author Hellyna Ng (hellyna@hellyna.com)
*/
#ifndef TIME_SERIES_H_739C5EB1_875D_4815_9959_777909E783D7
#define TIME_SERIES_H_739C5EB1_875D_4815_9959_777909E783D7

#include <time.h>
/*!
  The time_series_data_record_t \b struct.

  This is a partial type to be used by time_series_data_t for recording
  pairs of data. The reason is because the \b double* array size is recorded
  in the \b height variable of the parent struct.
  */
struct time_series_data_record_t
{
  /*!
    The time_t date of the entry.
  */
  time_t  date;
  /*!
    The data which is associated with the \b time_t \b date.
    */
  double* data;
};

typedef struct time_series_data_record_t time_series_data_record_t;

/*!
  The time_series_data_t \b struct.
  */
struct time_series_data_t
{
  /*!
    The number of records in this time_series_data_t.
    */
  size_t    height;
  /*!
    The number of fields in each record, excluding the date.
    */
  size_t    width;
  /*!
    The descriptions of the fields, excluding the self-explanatory date.
    */
  char**    desc;
  /*!
    The records of this time_series_data_t.
    */
  time_series_data_record_t*  records;
};

typedef struct time_series_data_t time_series_data_t;

/*!
  Constructs and recursively allocate memory for a new time_series_data_t instance.

  The resultant time_series_data_t instance will have its records sorted.

  \param path the path to the time-series csv file. Eg. S&P500.csv
  \return a new time_series_data_t instance.
  */
time_series_data_t*
construct_time_series_data (const char* const path);

/*!
  Destructs and recursively free memory for a time_series_data_t instance.
  */
void
destruct_time_series_data (time_series_data_t* const tsd);
#endif
