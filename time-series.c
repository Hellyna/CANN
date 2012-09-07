#include <string.h>

#include "libcsv/csv.h"
#include "util/util.h"
#include "validation.h"

#include "time-series.h"
int _compare_time_series_record (const void* a,
                                 const void* b)
{
  time_series_data_record_t* const record_a = (time_series_data_record_t*) a;
  time_series_data_record_t* const record_b = (time_series_data_record_t*) b;

  return (record_b->date - record_a->date);
}

time_series_data_t*
construct_time_series_data (const char* const path)
{
  // MALLOC, INIT: csvd
  csv_data_t* const csvd = construct_csv_data(path);

  // MALLOC: tsd
  time_series_data_t* const tsd = malloc_exit_if_null(sizeof(time_series_data_t));

  // INIT: tsd->width
  tsd->width = validate_csv_data_entry_counts(csvd) - 1;

  // INIT: tsd->height
  tsd->height = csvd->line_count - 1;

  // MALLOC: tsd->records
  tsd->records = malloc_exit_if_null(tsd->height * sizeof(time_series_data_record_t));
  // MALLOC: tsd->records->data
  size_t i, j;
  int sscanf_data[3],
      num_read;
  struct tm t;
  for (i = 0; i < tsd->height; ++i)
  {
    num_read = sscanf(csvd->data[i + 1][0], "%d%*c%d%*c%d", &(sscanf_data[0]), &(sscanf_data[1]), &(sscanf_data[2]));

    if (num_read == EOF)
    {
      perror("Error");
      exit(EXIT_FAILURE);
    }

    if (num_read < 3 || sscanf_data[0] < 1900)
    {
      putserr_and_exit("Malformed time-series data file.");
    }

    memset(&t, 0, sizeof(struct tm));
    t.tm_year = sscanf_data[0];
    t.tm_mon = sscanf_data[1];
    t.tm_mday = sscanf_data[2];
    tsd->records[i].date = mktime(&t);
    tsd->records[i].data = malloc_exit_if_null(tsd->width * sizeof(double));
    for (j = 0; j < tsd->width; ++j)
    {
      tsd->records[i].data[j] = strtod(csvd->data[i + 1][j + 1], NULL);
    }
  }

  // QUICKSORT: tsd->records
  qsort(tsd->records, tsd->height, sizeof(time_series_data_record_t), &_compare_time_series_record);

  // MALLOC: tsd->desc
  tsd->desc = malloc_exit_if_null((tsd->width + 1) * SIZEOF_PTR);
  for (i = 0; i < tsd->width + 1; ++i)
  {
    tsd->desc[i] = malloc_exit_if_null((strlen(csvd->data[0][i]) + 1) * sizeof(char));
    strncpy(tsd->desc[i], csvd->data[0][i], strlen(csvd->data[0][i]));
  }

  // FREE: csvd
  destruct_csv_data(csvd);
  return tsd;
}

void
destruct_time_series_data (time_series_data_t* const tsd)
{
  // FREE: tsd->records
  size_t i;
  for (i = 0; i < tsd->height; ++i)
  {
    free_and_null(tsd->records[i].data);
  }
  free_and_null(tsd->records);

  // FREE: tsd->desc
  for (i = 0; i < tsd->width + 1; ++i)
  {
    free_and_null(tsd->desc[i]);
  }
  free_and_null(tsd->desc);

  // FREE: tsd
  free_and_null(tsd);
}
