#include "util/util.h"

#include "validation.h"

size_t
validate_csv_io_data_line_count (const csv_data_t* const input_data,
                                 const csv_data_t* const output_data)
{
  const size_t input_line_count = input_data->line_count;
  if (input_line_count != output_data->line_count)
  {
    putserr_and_exit("The number of lines in the output file and the input file is not equal.");
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
      putserr_and_exit("Malformed csv data file.");
    }
    temp = data->entry_counts[i];
  }

  return temp;
}

void
validate_matching_neural_network_and_training_set (const neural_network_t*  const nn,
                                                   const training_set_t*    const ts)
{
  if (nn->config[0] != ts->input_size)
    putserr_and_exit("Number of input neurons must be equal to the number of inputs in training set.");

  if (nn->config[nn->config_size - 1] != ts->output_size)
    putserr_and_exit("Number of output neurons must be equal to the number of outputs in the training set.");
}

void
validate_neural_network_weights_file (const neural_network_t* const nn,
                                      const csv_data_t*       const data)
{
  size_t i;
  size_t  temp = data->entry_counts[0],
          num_consecutive_same_lines = 1,
          num_same_line_blocks = 1,
          neural_network_index = 0;
  for (i = 1; i < data->line_count; ++i)
  {
    if (temp != data->entry_counts[i])
    {
      if (nn->config[neural_network_index] != num_consecutive_same_lines)
        putserr_and_exit("Malformed neural network weights file.");

      num_consecutive_same_lines = 1;
      ++num_same_line_blocks;

    }
    else
    {
      ++num_consecutive_same_lines;
    }
    temp = data->entry_counts[i];
  }
  if (num_same_line_blocks != nn->config_size - 1)
    putserr_and_exit("Malformed neural network weights file.");
}
