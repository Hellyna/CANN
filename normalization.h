#ifndef NORMALIZATION_H_CDFBDB8B_1557_46EA_99C6_1B3719B0E819
#define NORMALIZATION_H_CDFBDB8B_1557_46EA_99C6_1B3719B0E819

#include "libcsv/csv.h"

struct normalization_data_t
{
  char*    _types;
  void**   _pre_processed;
  void**   _post_processed;
};

typedef struct normalization_data_t normalization_data_t;

normalization_data_t*
construct_normalization_data_from_csv_data (const csv_data_t* csv_data);

void
destruct_normalization_data_initialized_with_csv_data (normalization_data_t*  normalization_data,
                                                       const csv_data_t*      csv_data);

#endif
