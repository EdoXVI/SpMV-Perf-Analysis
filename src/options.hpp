#pragma once

#include <getopt.h>
#include <cstdlib>

//////////////////////////////
//////////////////////////////

#define DEBUG false
#define NUM_ITER 1
#define DEFAULT_DATASET "test"
#define DIRECTED 1
#define ALGORITHM 1

//////////////////////////////
//////////////////////////////

struct Options {
    // Testing options;
    bool debug = DEBUG;
    uint num_iter = NUM_ITER;
    std::string dataset = DEFAULT_DATASET;
    int directed = DIRECTED;
    int algorithm = ALGORITHM;

    //////////////////////////////
    //////////////////////////////

    Options(int argc, char *argv[]) {

        int opt;
        static struct option long_options[] = {{"debug", no_argument, 0, 'd'},
                                               {"num_iter", required_argument, 0, 'i'},
                                               {"dataset", required_argument, 0, 's'},
                                               {"directed", no_argument, 0, 'u'},
                                               {"algorithm", required_argument, 0, 'a'},
                                               {0, 0, 0, 0}};
        // getopt_long stores the option index here;
        int option_index = 0;

        while ((opt = getopt_long(argc, argv, "di:s:ua:", long_options, &option_index)) != EOF) {
            switch (opt) {
                case 'd':
                    debug = true;
                    break;
                case 'i':
                    num_iter = atoi(optarg);
                    break;
                case 's':
                    dataset = optarg;
                    break;
                case 'u':
                    directed = 2;
                    break;
                case 'a':
                    algorithm = atoi(optarg);
                    break;
                default:
                    break;
            }
        }
    }
};
