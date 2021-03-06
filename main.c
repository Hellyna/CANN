#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <getopt.h>

#include "util/util.h"
#include "neural-network.h"
#include "training.h"
#include "activation-functions.h"
#include "resilient-propagation.h"
#include "time-series.h"

//static int verbose_flag;

int
main (int argc, char** argv)
{
  /*
  int option;
    while (1)
    {
      static struct option long_options[] =
      {

        {"verbose", no_argument,       &verbose_flag, 1},
        {"brief",   no_argument,       &verbose_flag, 0},

        {"add",     no_argument,       0, 'a'},
        {"append",  no_argument,       0, 'b'},
        {"delete",  required_argument, 0, 'd'},
        {"create",  required_argument, 0, 'c'},
        {"train",   required_argument, 0, 't'},
        {0, 0, 0, 0}
      };

           int option_index = 0;

           option = getopt_long (argc, argv, "abc:d:f:",
                            long_options, &option_index);


           if (option == -1)
             break;

           switch (option)
             {
             case 0:

               if (long_options[option_index].flag != 0)
                 break;
               printf ("option %s", long_options[option_index].name);
               if (optarg)
                 printf (" with arg %s", optarg);
               printf ("\n");
               break;

             case 'a':
               puts ("option -a\n");
               break;

             case 'b':
               puts ("option -b\n");
               break;

             case 'c':
               printf ("option -c with value `%s'\n", optarg);
               break;

             case 'd':
               printf ("option -d with value `%s'\n", optarg);
               break;

             case 'f':
               printf ("option -f with value `%s'\n", optarg);
               break;

             case '?':

               break;

             default:
               abort ();
             }
         }


       if (verbose_flag)
         puts ("verbose flag is set");


       if (optind < argc)
         {
           printf ("non-option ARGV-elements: ");
           while (optind < argc)
             printf ("%s ", argv[optind++]);
           putchar ('\n');
         }
         */

  (void) argc;
  (void) argv;
  time_series_data_t* tsd = construct_time_series_data("libcsv/test.csv");
  struct tm fromt;
  struct tm tot;
  memset(&fromt, 0, sizeof(struct tm));
  memset(&tot, 0, sizeof(struct tm));
  fromt.tm_year = 2010 - 1900;
  fromt.tm_mon = 1 - 1;
  fromt.tm_mday = 1;

  tot.tm_year = 2010 - 1900;
  tot.tm_mon = 3 - 1;
  tot.tm_mday = 1;

  generate_training_set_files_from_time_series_data (tsd, mktime(&fromt), mktime(&tot), 5, 2, "snp500.in", "snp500.out");
  destruct_time_series_data(tsd);

  size_t config[] = {30,35,12};
  //size_t config[] = {2,4,1};
  neural_network_t* nn = construct_neural_network(config, 3, -2.0, 2.0, &initialize_nguyen_widrow_weights);
  training_t* training = construct_training(nn, &elliott_activation, &elliott_derivative, false);
  resilient_propagation_data_t* rprop_data = construct_resilient_propagation_data(nn);
 // training_set_t* ts = construct_training_set("xor.in", "xor.out");
  training_set_t* ts = construct_training_set("snp500.in", "snp500.out");
  normalize_training_set(ts);
  //debug_training_set(ts);
  printf("Final error rate: %g\n", train_neural_network(training, nn, ts, &resilient_propagation_loop, rprop_data, 20000));
  save_neural_network_weights(nn, "snp500.weights");
 // load_neural_network_weights(nn, "lol");
//  printf("Final error rate: %g\n", train_neural_network(training, nn, ts, &resilient_propagation_loop, rprop_data, 20000));
//  destruct_neural_network(nn);
//  nn = construct_neural_network_from_file("lol");
//  printf("Final error rate: %g\n", train_neural_network(training, nn, ts, &resilient_propagation_loop, rprop_data, 20000));
  destruct_training(training, nn);
  destruct_resilient_propagation_data(rprop_data, nn);
  destruct_training_set(ts);
  destruct_neural_network(nn);


  return 0;
}
