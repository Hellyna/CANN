#include <stdlib.h>

#include "csv.h"

int
main (int argc, char** argv)
{
  csv_data_t* csvd = construct_csv_data("test.csv");
  //print_csv_data(csvd);
  destruct_csv_data(csvd);
  csvd = construct_csv_data("non-existent.csv");

  return EXIT_SUCCESS;
}
