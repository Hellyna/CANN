#ifndef VALIDATION_H_2E18D049_EC8A_4515_B9EC_A86E4EDE2995
#define VALIDATION_H_2E18D049_EC8A_4515_B9EC_A86E4EDE2995

#include "libcsv/csv.h"

size_t
validate_csv_io_data_line_count (const csv_data_t* const input_data,
                                 const csv_data_t* const output_data);

size_t
validate_csv_data_entry_counts (const csv_data_t* const data);

#endif
