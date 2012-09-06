/*!
  \file csv.h
  \brief a convenience wrapper around libcsv (written by Robert Gamble)
  \author Hellyna Ng (hellyna@hellyna.com)
  */
#ifndef CSV_H_5120D992_9901_495F_8826_9098CD9DA3C8
#define CSV_H_5120D992_9901_495F_8826_9098CD9DA3C8

#include "libcsv.h"

typedef struct csv_parser csv_parser;

/*!
  The csv_data_t \b struct
  */
struct csv_data_t
{
  /*!
    The number of lines in the csv file.
    */
  size_t  line_count;
  /*!
    The number of entries in each line in a csv file.
    */
  size_t* entry_counts;
  /*!
    The csv data, implemented in a two-dimensional array holding data strings.
    */
  char*** data;
};

typedef struct csv_data_t csv_data_t;

/*!
  Constructs and recursively allocates memory for a new csv_data_t
  \param path the path to the associated csv file.
  \return a new csv_data_t instance.
  */
csv_data_t*
construct_csv_data (const char* path);

/*!
  Destructs and recursively free memory for the associated csv_data_t
  \param csvd the csv_data_t instance to free and destruct.
  */
void
destruct_csv_data (csv_data_t* csvd);

/*!
  Prints data in the associated csv_data_t instance. Used for debugging purposes.
  \param csvd the csv_data_t instance to print.
  */
void
print_csv_data (const csv_data_t* csvd);

#endif
