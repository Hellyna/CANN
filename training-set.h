#ifndef TRAINING_SET_H_624FF634_F49B_11E1_B46C_AB3C6188709B
#define TRAINING_SET_H_624FF634_F49B_11E1_B46C_AB3C6188709B

#include <stdbool.h>
#include <stddef.h>

struct training_set_t {
  size_t    training_set_size;

  size_t    input_size;
  double*   input_entries_min;
  double*   input_entries_max;
  char**    input_entries_desc;
  double**  target_inputs;

  size_t    output_size;
  double*   output_entries_min;
  double*   output_entries_max;
  char**    output_entries_desc;
  double**  target_outputs;

  bool      _is_normalized;
};

typedef struct training_set_t   training_set_t;

training_set_t*
construct_training_set (const char* const input_data_path,
                        const char* const output_data_path);

void
destruct_training_set (training_set_t* const ts);

void
normalize_training_set (training_set_t* const ts);

void
denormalize_training_set (training_set_t* const ts);

void
debug_training_set (const training_set_t* const ts);

#endif
