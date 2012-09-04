#include "util/util.h"


#include "normalization.h"

normalization_data_t*
construct_normalization_data_from_csv_data (const csv_data_t* csv_data)
{
  (void) csv_data;
  // MALLOC: data
  normalization_data_t* data = malloc_exit_if_null(sizeof(normalization_data_t));

  return data;
}

void
destruct_normalization_data_initialized_with_csv_data (normalization_data_t*  normalization_data,
                                                       const csv_data_t*      csv_data)
{
  (void) csv_data;
  // FREE: data
  free_and_null(normalization_data);
}

static inline double _normalize_entry (const double entry,
                                       const double entry_min,
                                       const double entry_max)
{
  return (entry - entry_min) / (entry_max - entry_min);
}
