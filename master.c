#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

#include "logger.h"

int main (int argc, char **argv)
{
  int hflag = 0;
  int nonOptArgFlag = 0;
  int index;
  int sValue = 5;
  int iValue = 3;
  int tValue = 20;
  char *filename = "test.out";
  char *defaultFileName = "test.out";
  char *programName = argv[0];
  char *option = NULL;
  char *short_options = "hs:l:i:t:";
  int c;

  struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {0,     0,            0,  0},
    {}
  };
  
  opterr = 0;
  while ((c = getopt_long (argc, argv, short_options, long_options, NULL)) != -1)
    switch (c)
      {
      case 'h':
        hflag = 1;
        break;
      case 's':
        sValue = atoi(optarg);
        break;
      case 'l':
        filename = optarg;
        break;
      case 'i':
        iValue = atoi(optarg);
      case 't':
        tValue = atoi(optarg);  
      case '?':
        if (optopt == 's') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.", optopt);
          sValue = 42;
        }
        else if (optopt == 'l') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.", optopt);
          filename = defaultFileName;
        }
        else if (optopt == 'i') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.", optopt);
          iValue = 3;
        }
        else if (optopt == 't') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.", optopt);
          tValue = 20;
        }
        else if (isprint (optopt)) {
          fprintf(stderr, "Unknown option -%c. Terminating.", optopt);
          return 1;
        }
        else {
          printShortHelpMessage();
          return 1; 
        }
      }

  
  for (index = optind; index < argc; index++) {
    fprintf(stderr, "Non-option argument %s", argv[index]);
    nonOptArgFlag = 1;
  }

  if(nonOptArgFlag) {
    printShortHelpMessage();
    return 1;
  }

  if(hflag) {
    printHelpMessage();
  }

  if(!hflag) {

  }

  return 0;
}
