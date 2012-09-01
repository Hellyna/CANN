#ifndef CSV_H_5120D992_9901_495F_8826_9098CD9DA3C8
#define CSV_H_5120D992_9901_495F_8826_9098CD9DA3C8

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

#endif
