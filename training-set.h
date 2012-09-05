#ifndef TRAINING_SET_H_624FF634_F49B_11E1_B46C_AB3C6188709B
#define TRAINING_SET_H_624FF634_F49B_11E1_B46C_AB3C6188709B

#include <stdlib.h>

struct training_set_t {
  size_t    training_set_size;
  size_t    input_size;
  char**    input_entries_desc;
  double**  target_inputs;
  size_t    output_size;
  char**    output_entries_desc;
  double**  target_outputs;
};

typedef struct training_set_t   training_set_t;

training_set_t*
construct_training_set (const char* input_data_path,
                        const char* output_data_path);

void
destruct_training_set (training_set_t* ts);

#endif
