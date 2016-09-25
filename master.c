#include <ctype.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <time.h>

//#include "logger.h"

void alarmHandler(int);
void waitFunction(int);
void printHelpMessage(void);
void printShortHelpMessage(void);

int main (int argc, char **argv)
{
  int hflag = 0;
  int nonOptArgFlag = 0;
  int index;
  int sValue = 5;
  int iValue = 3;
  int tValue = 10;
  const int MAXSLAVE = 20;
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
        if(sValue > MAXSLAVE) {
          sValue = 20;
          fprintf(stderr, "No more than 20 slave processes allowed. Reverting to 20.\n");
        }
        break;
      case 'l':
        filename = optarg;
        break;
      case 'i':
        iValue = atoi(optarg);
        break;
      case 't':
        tValue = atoi(optarg);  
        printf("%d\n", tValue);
        break;
      case '?':
        if (optopt == 's') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.\n", optopt);
          sValue = 5;
        }
        else if (optopt == 'l') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.\n", optopt);
          filename = defaultFileName;
        }
        else if (optopt == 'i') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.\n", optopt);
          iValue = 3;
        }
        else if (optopt == 't') {
          fprintf(stderr, "Option -%c requires an argument. Using default value.\n", optopt);
          tValue = 20;
        }
        else if (isprint (optopt)) {
          fprintf(stderr, "Unknown option -%c. Terminating.\n", optopt);
          return -1;
        }
        else {
          printShortHelpMessage();
          return 0; 
        }
      }

  
  for (index = optind; index < argc; index++) {
    fprintf(stderr, "Non-option argument %s\n", argv[index]);
    nonOptArgFlag = 1;
  }

  if(nonOptArgFlag) {
    printShortHelpMessage();
    return 0;
  }

  if(hflag) {
    printHelpMessage();
    return 0;
  }

  alarm(tValue);
  waitFunction(tValue + 10);

  return 0;
}

void alarmHandler(int SIG){
  signal(SIGALRM, SIG_IGN);
  kill(getpgrp(), SIGKILL);
}

void waitFunction(int tValue) {
  time_t startTime = time(NULL);
  time_t elapsedTime = time(NULL);

  while(elapsedTime <= startTime + tValue) {
    long prevTime = elapsedTime;
    elapsedTime = time(NULL);
    if(elapsedTime > prevTime) {
      printf("Master time: %d\n", elapsedTime - startTime - 1); 
    }
  }
}

void printHelpMessage(void) {
    printf("\nThank you for using the help menu!\n");
    printf("The following is a helpful guide to enable you to use this\n");
    printf("slavedriver program to the best of your ability!\n\n");
    printf("-h, --help: Prints this help message.\n");
    printf("-s: Allows you to set the number of slave process to run.\n");
    printf("\tThe default value is 5.\n");
    printf("-l: Allows you to set the filename for the logger so the aliens can see how bad you mess up.\n");
    printf("\tThe default value is test.out.\n");
    printf("-i: Allows you to set the number of times each slave enters the critical section of code.\n");
    printf("\tThe default value is 3.\n");
    printf("-t: Allows you set the wait time for the master process until it kills the slaves\n");
    printf("\tThe default value is 20\n");
}

void printShortHelpMessage(void) {
  printf("\nAcceptable options are:\n");
  printf("[-h], [--help], [-l][required_arg], [-s][required_arg], [-i][required_arg], [-t][required_arg]\n\n");
}


