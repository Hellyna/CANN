/*!
  \file training-set.h
  \brief Organizes and manages training set data.
  \author Hellyna Ng (hellyna@hellyna.com)
  */

#ifndef TRAINING_SET_H_624FF634_F49B_11E1_B46C_AB3C6188709B
#define TRAINING_SET_H_624FF634_F49B_11E1_B46C_AB3C6188709B

#include <stdbool.h>
#include <stddef.h>

/*!
  The training_set_t \b struct.
  */
struct training_set_t {
  /*!
    The size of the entire training set.
    */
  size_t    training_set_size;
  /*!
    The input size of this training set.
    */
  size_t    input_size;
  /*!
    The minimum input entry in this training set.
    */
  double*   input_entries_min;
  /*!
    The maximum input entry in this training set.
    */
  double*   input_entries_max;
  /*!
    The descriptions of the inputs of this training set.
    */
  char**    input_entries_desc;
  /*!
    The target inputs of this training set
    */
  double**  target_inputs;
  /*!
    The output size of this training set.
    */
  size_t    output_size;
  /*!
    The minimum output entry in this training set.
    */
  double*   output_entries_min;
  /*!
    The maximum output entry in this training set.
    */
  double*   output_entries_max;
  /*!
    The descriptions of the outputs of this training set.
    */
  char**    output_entries_desc;
  /*!
    The target outputs of this training set
    */
  double**  target_outputs;
  /*!
    Used internally. Sets to true if data is already normalized. Defaults to false.
    */
  bool      _is_normalized;
};

typedef struct training_set_t   training_set_t;

/*!
  Constructs and recursively allocate memory for a new training_set_t instance.
  \param input_data_path the path to the file holding the input data set.
  \param output_data_path the path to the file holding the output data set.
  \return a new training_set_t instance
  */
training_set_t*
construct_training_set (const char* const input_data_path,
                        const char* const output_data_path);
/*!
  Destructs and recursively free memory for this training_set_t instance.
  \param ts the training_set_t instance to destruct and free.
  */
void
destruct_training_set (training_set_t* const ts);

/*!
  Normalizes the associated training set. Will do nothing if the internal \b _is_normalized flag is set to true.
  \param ts the training_set_t instance to normalize.
  */
void
normalize_training_set (training_set_t* const ts);

/*!
  Denormalizes the associated training set back to its default ranges. Will do nothing if the internal
  \b _is_normalized flag is set to false.
  \param ts the training_set_t instance to denormalize.
  */
void
denormalize_training_set (training_set_t* const ts);

/*!
  Debug the associated training set, printing its contents
  \param ts the training_set_t instance to debug.
  */
void
debug_training_set (const training_set_t* const ts);

#endif
