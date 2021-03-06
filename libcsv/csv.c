#include <string.h>
#include "../util/util.h"
#include "csv.h"

static size_t line_index;
static size_t entry_index;

static void
count_lines (int delim, void* csv_data)
{
  (void) delim;
  csv_data_t* csvd = (csv_data_t*) csv_data;
  ++(csvd->line_count);
}

static void
increment_line (int delim, void* csv_data)
{
  (void) delim;
  (void) csv_data;
  ++line_index;
  entry_index = 0;
}

static void
count_entries (void* entry, size_t entry_length, void* csv_data)
{
  (void) entry;
  (void) entry_length;
  csv_data_t* csvd = (csv_data_t*) csv_data;
  ++(csvd->entry_counts[line_index]);
}

static void
read_entries (void* entry, size_t entry_length, void* csv_data)
{
  csv_data_t* csvd = (csv_data_t*) csv_data;
  csvd->data[line_index][entry_index] = malloc_exit_if_null(sizeof(char) * entry_length + 1);
  memcpy(csvd->data[line_index][entry_index], entry, sizeof(char) * entry_length + 1);
  ++entry_index;
}

static inline void
csv_exit_if_error (csv_parser* parser)
{
  printferr("Error while parsing file: %s\n", csv_strerror(csv_error(parser)));
  exit(EXIT_FAILURE);
}

csv_data_t*
construct_csv_data (const char* path)
{
  FILE* fp = fopen(path, "rb");
  exit_if_null(fp);

  csv_parser parser;
  char buffer[1024];
  size_t bytes_read;

  exit_if_not_zero(csv_init(&parser, CSV_STRICT | CSV_APPEND_NULL));

  // MALLOC: csvd
  csv_data_t* csvd = malloc_exit_if_null(sizeof(csv_data_t));

  // Count csv file lines
  csvd->line_count = 0;
  while ((bytes_read = fread(buffer, 1, 1024, fp)) > 0)
  {
    if (csv_parse(&parser, buffer, bytes_read, NULL, count_lines, csvd) != bytes_read)
    {
      csv_exit_if_error(&parser); }
  }

  // MALLOC: csvd->entry_counts
  csvd->entry_counts = calloc_exit_if_null(csvd->line_count, sizeof(size_t));

  // MALLOC: csvd->data (lines)
  csvd->data = malloc_exit_if_null(SIZEOF_PTR * csvd->line_count);

  // Count csv file entries in each line
  csv_fini(&parser, NULL, NULL, csvd);
  rewind(fp);
  line_index = 0;
  while ((bytes_read = fread(buffer, 1, 1024, fp)) > 0)
  {
    if (csv_parse(&parser, buffer, bytes_read, count_entries, increment_line, csvd) != bytes_read)
    {
      csv_exit_if_error(&parser);
    }
  }

  // MALLOC: csvd->data (fields in malloc'd lines)
  size_t i;
  for (i = 0; i < csvd->line_count; ++i)
  {
    csvd->data[i] = malloc_exit_if_null(SIZEOF_PTR * csvd->entry_counts[i]);
  }

  // Read each entry into data, including mallocing the associated space.
  csv_fini(&parser, NULL, NULL, csvd);
  rewind(fp);
  line_index = 0;
  entry_index = 0;
  while ((bytes_read = fread(buffer, 1, 1024, fp)) > 0)
  {
    if (csv_parse(&parser, buffer, bytes_read, read_entries, increment_line, csvd) != bytes_read)
    {
      csv_exit_if_error(&parser);
    }
  }

  fclose(fp);
  csv_free(&parser);
  return csvd;
}


void
destruct_csv_data (csv_data_t* csvd)
{
  size_t i, j;
  for (i = 0; i < csvd->line_count; ++i)
  {
    for (j = 0; j < csvd->entry_counts[i]; ++j)
    {
      // FREE: csvd->data[i][j]
      free_and_null(csvd->data[i][j]);
    }
    // FREE: csvd->data[i]
    free_and_null(csvd->data[i]);
  }

  // FREE: csvd->data
  free_and_null(csvd->data);

  // FREE: csvd->entry_counts
  free_and_null(csvd->entry_counts);

  // FREE: csvd
  free_and_null(csvd);
}

void
print_csv_data (const csv_data_t* csvd)
{
  printf("csvd->line_count = %d\n", csvd->line_count);
  size_t i, j;
  for (i = 0; i < csvd->line_count; ++i)
  {
    printf("csvd->entry_count[%d] = %d\n", i, csvd->entry_counts[i]);
    for (j = 0; j < csvd->entry_counts[i]; ++j)
    {
      printf("csvd->data[%d][%d] = %s\n", i, j, csvd->data[i][j]);
    }
  }
}
