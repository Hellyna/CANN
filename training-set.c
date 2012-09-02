
#include "util/util.h"
#include "libcsv/csv.h"

#include "training-set.h"

training_set_t*
construct_training_set (const char* input_data_path,
                        const char* output_data_path)
{
  // MALLOC: ts
  training_set_t* ts = malloc_exit_if_null(sizeof(training_set_t));

  csv_data_t* input_data = construct_csv_data(input_data_path);
  csv_data_t* output_data = construct_csv_data(output_data_path);

  // VALIDATE: training_set_size
  if (input_data->line_count != output_data->line_count)
  {
    printferr_and_exit("Input and output data file entries are not equal.\n");
  }

  // INIT: ts->training_set_size
  ts->training_set_size = input_data->line_count - 1;

  // VALIDATE: ts->input_size
  size_t i, temp = input_data->entry_counts[0];
  for (i = 1; i < ts->training_set_size; ++i)
  {
    if (temp != input_data->entry_counts[i])
    {
      printferr_and_exit("Malformed input data file.\n");
    }
    temp = input_data->entry_counts[i];
  }

  // INIT: ts->input_size
  ts->input_size = temp;

  // VALIDATE: ts->output_size
  temp = output_data->entry_counts[0];
  for (i = 1; i < ts->training_set_size; ++i)
  {
    if (temp != output_data->entry_counts[i])
    {
      printferr_and_exit("Malformed output data file.\n");
    }
    temp = output_data->entry_counts[i];
  }

  // INIT: ts->output_size
  ts->output_size = temp;

  // MALLOC: ts->target_inputs
  // INIT: ts->target_inputs
  // MALLOC: ts->target_outputs
  // INIT: ts->target_outputs
  ts->target_inputs = malloc_exit_if_null(SIZEOF_PTR * ts->training_set_size);
  ts->target_outputs = malloc_exit_if_null(SIZEOF_PTR * ts->training_set_size);

  size_t j;
  for (i = 0; i < ts->training_set_size; ++i)
  {
    ts->target_inputs[i] = malloc_exit_if_null(ts->input_size * sizeof(double));
    for (j = 0; j < ts->input_size; ++j)
    {
      ts->target_inputs[i][j] = atof(input_data->data[i + 1][j]);
    }

    ts->target_outputs[i] = malloc_exit_if_null(ts->output_size * sizeof(double));
    for (j = 0; j < ts->output_size; ++j)
    {
      ts->target_outputs[i][j] = atof(output_data->data[i + 1][j]);
    }
  }

  destruct_csv_data(input_data);
  destruct_csv_data(output_data);
  return ts;
}


void
destruct_training_set (training_set_t* ts)
{
  // FREE: ts->target_inputs
  // FREE: ts->target_outputs
  size_t i;
  for (i = 0; i < ts->training_set_size; ++i)
  {
    free_and_null(ts->target_inputs[i]);
    free_and_null(ts->target_outputs[i]);
  }
  free_and_null(ts->target_inputs);
  free_and_null(ts->target_outputs);

  // FREE: ts
  free_and_null(ts);
}
