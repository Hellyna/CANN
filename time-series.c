#include <string.h>

#include "libcsv/csv.h"
#include "util/util.h"
#include "validation.h"

#include "time-series.h"
static int _compare_time_series_record (const void* a,
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
if (num_read < 3 || sscanf_data[0] < 1900) {
      putserr_and_exit("Malformed time-series data file.");
    }

    memset(&t, 0, sizeof(struct tm));
    t.tm_year = sscanf_data[0] - 1900;
    t.tm_mon = sscanf_data[1] - 1;
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
  tsd->desc = malloc_exit_if_null((tsd->width) * SIZEOF_PTR);
  for (i = 0; i < tsd->width; ++i)
  {
    tsd->desc[i] = malloc_exit_if_null((strlen(csvd->data[0][i + 1]) + 1) * sizeof(char));
    strncpy(tsd->desc[i], csvd->data[0][i + 1], strlen(csvd->data[0][i + 1]));
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
  for (i = 0; i < tsd->width; ++i)
  {
    free_and_null(tsd->desc[i]);
  }
  free_and_null(tsd->desc);

  // FREE: tsd
  free_and_null(tsd);
}

void
generate_training_set_files_from_time_series_data (const time_series_data_t* const tsd,
                                                   const time_t                    from,
                                                   const time_t                    to,
                                                   const size_t                    input_training_block_size,
                                                   const size_t                    output_training_block_size,
                                                   const char*               const input_training_set_file_name,
                                                   const char*               const output_training_set_file_name)
{
  time_series_data_record_t from_record_temp;
  from_record_temp.date = from;
  time_series_data_record_t to_record_temp;
  to_record_temp.date = to;
  time_series_data_record_t* from_record;
  time_series_data_record_t* to_record;
  size_t i, j;
  for (i = 0; i < 5; ++i)
  {
    from_record = (time_series_data_record_t*) bsearch(&from_record_temp, tsd->records, tsd->height, sizeof(time_series_data_record_t), &_compare_time_series_record);
    if (from_record == NULL)
    {
      printf("Requested from-record with time-stamp %s not found.\n", ctime(&(from_record_temp.date)));
      puts("Incrementing time-stamp...");
      from_record_temp.date += 86400;
    }
    else
    {
      printf("from-record with time-stamp %s ", ctime(&(from_record_temp.date)));
      if (i == 0)
        puts("found.");
      else
        puts("found instead.");

      break;
    }
  }

  for (i = 0; i < 5; ++i)
  {
    to_record = (time_series_data_record_t*) bsearch(&to_record_temp, tsd->records, tsd->height, sizeof(time_series_data_record_t), &_compare_time_series_record);
    if (to_record == NULL)
    {
      printf("Requested to-record with time-stamp %s not found.\n", ctime(&(to_record_temp.date)));
      puts("Incrementing time-stamp...");
      to_record_temp.date += 86400;
    }
    else
    {
      printf("to-record with time-stamp %s ", ctime(&(to_record_temp.date)));
      if (i == 0)
        puts("found.");
      else
        puts("found instead.");

      break;
    }
  }

  if (from_record == NULL || to_record == NULL)
  {
    putserr_and_exit("Unable to generate training set files because the dates are not found.");
  }

  FILE* finputts = fopen(input_training_set_file_name, "wb");
  exit_if_null(finputts);
  FILE* foutputts = fopen(output_training_set_file_name, "wb");
  exit_if_null(foutputts);
  char buffer[1024];

  for (i = 0; i < input_training_block_size; ++i)
  {
    for (j = 0; j < tsd->width; ++j)
    {
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "%s%d", tsd->desc[j], i);
      csv_fwrite(finputts, buffer, strlen(buffer));
      if (j < tsd->width - 1 || i < input_training_block_size - 1)
        fputc(',', finputts);
    }
  }
  fputc('\n', finputts);

  for (i = 0; i < output_training_block_size; ++i)
  {
    for (j = 0; j < tsd->width; ++j)
    {
      memset(buffer, 0, sizeof(buffer));
      sprintf(buffer, "%s%d", tsd->desc[j], i);
      csv_fwrite(foutputts, buffer, strlen(buffer));
      if (j < tsd->width - 1 || i < output_training_block_size - 1)
        fputc(',', foutputts);
    }
  }
  fputc('\n', foutputts);

  time_series_data_record_t* input_training_start = from_record;
  time_series_data_record_t* input_training_end = from_record - (input_training_block_size - 1);
  time_series_data_record_t* output_training_start = input_training_end - 1;
  time_series_data_record_t* output_training_end = output_training_start - (output_training_block_size - 1);

  time_series_data_record_t* input_iter;
  time_series_data_record_t* output_iter;
  while (output_training_end != to_record - 1)
  {
    for (input_iter = input_training_start; input_iter != input_training_end - 1; --input_iter)
    {
      for (j = 0; j < tsd->width; ++j)
      {
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%g", input_iter->data[j]);
        csv_fwrite(finputts, buffer, strlen(buffer));
        if (j < tsd->width - 1 || input_iter != input_training_end)
          fputc(',', finputts);
      }
    }
    fputc('\n', finputts);

    for (output_iter = output_training_start; output_iter != output_training_end - 1; --output_iter)
    {
      for (j = 0; j < tsd->width; ++j)
      {
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%g", output_iter->data[j]);
        csv_fwrite(foutputts, buffer, strlen(buffer));
        if (j < tsd->width - 1 || output_iter != output_training_end)
          fputc(',', foutputts);
      }
    }
    fputc('\n', foutputts);

    --input_training_start;
    --input_training_end;
    --output_training_start;
    --output_training_end;
  }

  fclose(finputts);
  fclose(foutputts);
}

