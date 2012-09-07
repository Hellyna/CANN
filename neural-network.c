#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "util/util.h"
#include "libcsv/csv.h"
#include "validation.h"

#include "neural-network.h"

neural_network_t*
_construct_neural_network (const size_t* const config,
                           const size_t        config_size)
{
  // MALLOC: nn
  neural_network_t* nn = malloc_exit_if_null(sizeof(neural_network_t));

  // INIT: nn->config_size
  nn->config_size = config_size;

  // MALLOC: nn->config
  nn->config = malloc_exit_if_null(sizeof(size_t) * config_size);

  // INIT: nn->config
  memcpy(nn->config, config, sizeof(size_t) * config_size);

  size_t num_weight_layers = nn->config_size - 1;
  // MALLOC: nn->weights
  nn->weights = malloc_exit_if_null(num_weight_layers * SIZEOF_PTR);

  size_t i, j;
  for (i = 0; i < num_weight_layers; ++i)
  {
    nn->weights[i] = malloc_exit_if_null(SIZEOF_PTR * nn->config[i]);
    for (j = 0; j < nn->config[i]; ++j)
    {
      nn->weights[i][j] = malloc_exit_if_null(sizeof(double) * nn->config[i + 1]);
    }
  }

  return nn;
}


neural_network_t*
construct_neural_network (const size_t* const config,
                          const size_t        config_size,
                          const double        min_weight,
                          const double        max_weight,

                          void                (*weight_initialization_function) (const neural_network_t* const,
                                                                                 const double,
                                                                                 const double))
{
  neural_network_t* nn = _construct_neural_network(config, config_size);
  // INIT: nn->weights
  (*weight_initialization_function) (nn, min_weight, max_weight);

  return nn;
}


neural_network_t*
construct_neural_network_from_file (const char* const path)
{
  csv_data_t* const data = construct_csv_data(path);
  size_t i;
  size_t  temp = data->entry_counts[0],
          num_consecutive_same_lines = 1,
          num_same_line_blocks = 1,
          config_size,
          config[1024];
  for (i = 1; i < data->line_count; ++i)
  {
    if (temp != data->entry_counts[i])
    {
      config[num_same_line_blocks - 1] = num_consecutive_same_lines;
      num_consecutive_same_lines = 1;
      ++num_same_line_blocks;
    }
    else
    {
      ++num_consecutive_same_lines;
    }
    temp = data->entry_counts[i];
  }
  config_size = num_same_line_blocks + 1;
  config[num_same_line_blocks - 1] = num_consecutive_same_lines;
  config[num_same_line_blocks] = temp;

  destruct_csv_data(data);

  neural_network_t* nn = _construct_neural_network(config, config_size);
  load_neural_network_weights(nn, path);

  return nn;
}


void
destruct_neural_network (neural_network_t* nn)
{
  // FREE: nn->weights
  size_t num_weight_layers = nn->config_size - 1;
  size_t i, j;
  for (i = 0; i < num_weight_layers; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      free_and_null(nn->weights[i][j]);
    }
    free_and_null(nn->weights[i]);
  }
  free_and_null(nn->weights);

  // FREE: config
  free_and_null(nn->config);

  // FREE: nn
  free_and_null(nn);
}


void
initialize_nguyen_widrow_weights (const neural_network_t* const nn,
                                  const double                  min_weight,
                                  const double                  max_weight)
{
  initialize_uniform_weights(nn, min_weight, max_weight);

  size_t num_input = nn->config[0];
  size_t num_hidden = 0;
  size_t i;
  for (i = 1; i < nn->config_size - 1; ++i)
  {
    num_hidden += nn->config[i];
  }
  double beta = 0.7 * pow(num_hidden, 1.0/num_input);

  size_t j, k;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      double n = 0.0;
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        double x = nn->weights[i][j][k];
        n += x * x;
      }
      n = sqrt(n);

      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        double x = nn->weights[i][j][k];
        nn->weights[i][j][k] = (x * beta) / n;
      }
    }
  }
}


void
initialize_uniform_weights (const neural_network_t* const nn,
                            const double                  min_weight,
                            const double                  max_weight)
{
  srand((unsigned)time(NULL));

  size_t i, j, k;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        nn->weights[i][j][k] = rand_double() * (max_weight - min_weight) + min_weight ;
      }
    }
  }
}

void
save_neural_network_weights (const neural_network_t*  const nn,
                             const char*              const path)
{
  FILE* fp = fopen(path, "wb");
  exit_if_null(fp);
  size_t i, j, k;
  char buffer[1024];
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%g", nn->weights[i][j][k]);
        csv_fwrite(fp, buffer, strlen(buffer));
        if (k < nn->config[i + 1] - 1)
          fputc(',', fp);
      }
      fputc('\n', fp);
    }
  }
  fclose(fp);
}



void
load_neural_network_weights (const neural_network_t*  const nn,
                             const char*              const path)
{
  csv_data_t* data = construct_csv_data(path);
  validate_neural_network_file(nn, data);
  size_t i, j, k, csv_data_index = 0;
  for (i = 0; i < nn->config_size - 1; ++i)
  {
    for (j = 0; j < nn->config[i]; ++j)
    {
      for (k = 0; k < nn->config[i + 1]; ++k)
      {
        nn->weights[i][j][k] = strtod(data->data[csv_data_index + j][k], NULL);
      }
    }
    csv_data_index += nn->config[i];
  }
  destruct_csv_data(data);
}
