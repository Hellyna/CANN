#include <string.h>
#include <float.h>

#include "util/util.h"
#include "libcsv/csv.h"
#include "validation.h"

#include "training-set.h"

training_set_t*
construct_training_set (const char* const input_data_path,
                        const char* const output_data_path)
{
  // MALLOC: ts
  training_set_t* ts = malloc_exit_if_null(sizeof(training_set_t));

  csv_data_t* input_data = construct_csv_data(input_data_path);
  csv_data_t* output_data = construct_csv_data(output_data_path);

  // INIT: ts->training_set_size
  ts->training_set_size = validate_csv_io_data_line_count(input_data, output_data) - 1;

  // INIT: ts->input_size
  ts->input_size = validate_csv_data_entry_counts(input_data);

  // INIT: ts->output_size
  ts->output_size = validate_csv_data_entry_counts(output_data);

  // MALLOC: ts->target_inputs
  // INIT: ts->target_inputs
  // MALLOC: ts->target_outputs
  // INIT: ts->target_outputs
  ts->target_inputs = malloc_exit_if_null(SIZEOF_PTR * ts->training_set_size);
  ts->target_outputs = malloc_exit_if_null(SIZEOF_PTR * ts->training_set_size);

  size_t i, j;
  for (i = 0; i < ts->training_set_size; ++i)
  {
    ts->target_inputs[i] = malloc_exit_if_null(ts->input_size * sizeof(double));
    for (j = 0; j < ts->input_size; ++j)
    {
      ts->target_inputs[i][j] = strtod(input_data->data[i + 1][j], NULL);
    }

    ts->target_outputs[i] = malloc_exit_if_null(ts->output_size * sizeof(double));
    for (j = 0; j < ts->output_size; ++j)
    {
      ts->target_outputs[i][j] = strtod(output_data->data[i + 1][j], NULL);
    }
  }

  // MALLOC: ts->input_entries_desc
  // MALLOC: ts->input_entries_max
  // MALLOC: ts->input_entries_min
  size_t size = ts->input_size * SIZEOF_PTR;
  ts->input_entries_desc = malloc_exit_if_null(size);
  ts->input_entries_min = malloc_exit_if_null(ts->input_size * sizeof(double));
  ts->input_entries_max = calloc_exit_if_null(ts->input_size, sizeof(double));
  for (i = 0; i < ts->input_size; ++i)
  {
    size = (strlen(input_data->data[0][i]) + 1) * sizeof(char);
    ts->input_entries_desc[i] = malloc_exit_if_null(size);
    strncpy(ts->input_entries_desc[i], input_data->data[0][i], strlen(input_data->data[0][i]));
    ts->input_entries_min[i] = DBL_MAX;
  }

  // MALLOC: ts->output_entries_desc
  // MALLOC: ts->output_entries_max
  // MALLOC: ts->output_entries_min
  size = ts->output_size * SIZEOF_PTR;
  ts->output_entries_desc = malloc_exit_if_null(size);
  ts->output_entries_min = malloc_exit_if_null(ts->output_size * sizeof(double));
  ts->output_entries_max = calloc_exit_if_null(ts->output_size, sizeof(double));
  for (i = 0; i < ts->output_size; ++i)
  {
    size = (strlen(output_data->data[0][i]) + 1) * sizeof(char);
    ts->output_entries_desc[i] = malloc_exit_if_null(size);
    strncpy(ts->output_entries_desc[i], output_data->data[0][i], strlen(output_data->data[0][i]));
    ts->output_entries_min[i] = DBL_MAX;
  }

  // INIT: ts->input_entries_min, ts->input_entries_max
  for (j = 0; j < ts->input_size; ++j) {
    for (i = 0; i < ts->training_set_size; ++i)
    {
      size = ts->target_inputs[i][j];
      if (ts->input_entries_min[j] > size)
        ts->input_entries_min[j] = size;

      if (ts->input_entries_max[j] < size)
        ts->input_entries_max[j] = size;
    }
  }

  // INIT: ts->output_entries_min, ts->output_entries_max
  for (j = 0; j < ts->output_size; ++j) {
    for (i = 0; i < ts->training_set_size; ++i)
    {
      size = ts->target_outputs[i][j];
      if (ts->output_entries_min[j] > size)
        ts->output_entries_min[j] = size;

      if (ts->output_entries_max[j] < size)
        ts->output_entries_max[j] = size;
    }
  }

  // INIT: ts->_is_normalized
  ts->_is_normalized = false;

  destruct_csv_data(input_data);
  destruct_csv_data(output_data);
  return ts;
}


void
destruct_training_set (training_set_t* const ts)
{
  // FREE: ts->input_entries_desc
  // FREE: ts->input_entries_min
  // FREE: ts->input_entries_max
  size_t i;
  for (i = 0; i < ts->input_size; ++i)
  {
    free_and_null(ts->input_entries_desc[i]);
  }
  free_and_null(ts->input_entries_desc);
  free_and_null(ts->input_entries_min);
  free_and_null(ts->input_entries_max);

  // FREE: ts->output_entries_desc
  // FREE: ts->output_entries_min
  // FREE: ts->output_entries_max
  for (i = 0; i < ts->output_size; ++i)
  {
    free_and_null(ts->output_entries_desc[i]);
  }
  free_and_null(ts->output_entries_desc);
  free_and_null(ts->output_entries_min);
  free_and_null(ts->output_entries_max);

  // FREE: ts->target_inputs
  // FREE: ts->target_outputs
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


inline static double
_normalize (const double entry,
            const double min,
            const double max)
{
  return (entry - min) / (max - min);
}

inline static double
_denormalize (const double entry,
              const double min,
              const double max)
{
  return ((entry * max) - (entry * min) + min);
}

void
normalize_training_set (training_set_t* const ts)
{
  if (ts->_is_normalized == false)
  {
    size_t i, j;
    double max, min;
    for (j = 0; j < ts->input_size; ++j)
    {
      max = ts->input_entries_max[j];
      min = ts->input_entries_min[j];
      for (i = 0; i < ts->training_set_size; ++i)
      {
        ts->target_inputs[i][j] = _normalize(ts->target_inputs[i][j], min, max);
      }
    }

    for (j = 0; j < ts->output_size; ++j)
    {
      max = ts->output_entries_max[j];
      min = ts->output_entries_min[j];
      for (i = 0; i < ts->training_set_size; ++i)
      {
        ts->target_outputs[i][j] = _normalize(ts->target_outputs[i][j], min, max);
      }
    }
  }
  else
  {
    puts("Training set is already normalized.\n");
  }

  ts->_is_normalized = true;
}

void
denormalize_training_set (training_set_t* const ts)
{
  if (ts->_is_normalized == true)
  {
    size_t i, j;
    double max, min;
    for (j = 0; j < ts->input_size; ++j)
    {
      max = ts->input_entries_max[j];
      min = ts->input_entries_min[j];
      for (i = 0; i < ts->training_set_size; ++i)
      {
        ts->target_inputs[i][j] = _denormalize(ts->target_inputs[i][j], min, max);
      }
    }

    for (j = 0; i < ts->output_size; ++j)
    {
      max = ts->output_entries_max[j];
      min = ts->output_entries_min[j];
      for (i = 0; i < ts->training_set_size; ++i)
      {
        ts->target_outputs[i][j] = _denormalize(ts->target_outputs[i][j], min, max);
      }
    }
  }
  else
  {
    puts("Training set is already denormalized or not normalized.\n");
  }

  ts->_is_normalized = false;
}

void
debug_training_set (const training_set_t* const ts)
{
  size_t i, j;
  for (i = 0; i < ts->training_set_size; ++i)
  {
    for (j = 0; j < ts->input_size; ++j)
    {
      printf("ts->target_inputs[%d][%d] = %g\n", i, j, ts->target_inputs[i][j]);
    }

    for (j = 0; j < ts->output_size; ++j)
    {
      printf("ts->target_outputs[%d][%d] = %g\n", i, j, ts->target_outputs[i][j]);
    }
  }
}

