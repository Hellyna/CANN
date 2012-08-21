#ifndef CSV_H
#define CSV_H

#include "libcsv.h"

typedef struct csv_parser csv_parser;

struct csv_data_t
{
  size_t  line_count;
  size_t* entry_counts;
  char*** data;
};

typedef struct csv_data_t csv_data_t;

csv_data_t*
construct_csv_data (const char* path);

void
destruct_csv_data (csv_data_t* csvd);

void
print_csv_data (const csv_data_t* csvd);

#endif /* CSV_H */
