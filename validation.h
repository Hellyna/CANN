/*!
  \file validation.h
  \brief Provides various validation facilities for this neural network
  \author Hellyna Ng (hellyna@hellyna.com)
  */
#ifndef VALIDATION_H_2E18D049_EC8A_4515_B9EC_A86E4EDE2995
#define VALIDATION_H_2E18D049_EC8A_4515_B9EC_A86E4EDE2995

#include "libcsv/csv.h"
#include "neural-network.h"
#include "training-set.h"

/*!
  Validates if both input and output csv training set files are valid with respect to each other.
  \param input_data the csv_data_t instance created with the path to the input data file.
  \param output_data the csv_data_t instance created with the path to the output data file.
  \return the number of lines in those two files, which should be the same.
  */
size_t
validate_csv_io_data_line_count (const csv_data_t* const input_data,
                                 const csv_data_t* const output_data);

/*!
  Validates the entry counts in the csv training set file.
  \param data the associated csv_data_t instance created with the path to the data file.
  \return the number of entry count in each line in that file, which should be the same for every line.
  */
size_t
validate_csv_data_entry_counts (const csv_data_t* const data);

/*!
  Validates if the associated neural_network_t instance and training_set_t instance can match.
  \param nn the associated neural_network_t instance to validate with.
  \param ts the associated training_set_t instance to validate with.
  */
void
validate_matching_neural_network_and_training_set (const neural_network_t*  const nn,
                                                   const training_set_t*    const ts);

/*!
  Validates the weights file created by save_neural_network()
  \param nn the associated neural_network_t instance to validate with.
  \param data the associated csv_data_t instance which is created with the path to the associated weight file to validate with.
  */
void
validate_neural_network_file (const neural_network_t* const nn,
                              const csv_data_t*       const data);
#endif
