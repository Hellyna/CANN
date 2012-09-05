#include "util/util.h"

#include "validation.h"

size_t
validate_csv_io_data_line_count (const csv_data_t* const input_data,
                                 const csv_data_t* const output_data)
{
  const size_t input_line_count = input_data->line_count;
  if (input_line_count != output_data->line_count)
  {
    putserr_and_exit("Input and output data file entries are not equal.\n");
  }

  return input_line_count;
}

size_t
validate_csv_data_entry_counts (const csv_data_t* const data)
{
  size_t i, temp = data->entry_counts[0];
  for (i = 1; i < data->line_count; ++i)
  {
    if (temp != data->entry_counts[i])
    {
      putserr_and_exit("Malformed csv data file.\n");
    }
    temp = data->entry_counts[i];
  }

  return temp;
}

