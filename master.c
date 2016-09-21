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
  char *defaultFileName = "logfile.txt";
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
          char message[50];
          sprintf(message, "Option -%c requires an argument. Using default value.", optopt);
          puts(message);
          buildAndAddErrorMessage(message, programName, sValue);
          sValue = 42;
        }
        else if (optopt == 'l') {
          char message[50];
          sprintf(message, "Option -%c requires an argument. Using default value.", optopt);
          puts(message);
          buildAndAddErrorMessage(message, programName, sValue);
          filename = defaultFileName;
        }
        else if (optopt == 'i') {
          char message[50];
          sprintf(message, "Option -%c requires an argument. Using default value.", optopt);
          puts(message);
          buildAndAddErrorMessage(message, programName, sValue);
          filename = defaultFileName;
        }
        else if (optopt == 't') {
          char message[50];
          sprintf(message, "Option -%c requires an argument. Using default value.", optopt);
          puts(message);
          buildAndAddErrorMessage(message, programName, sValue);
          filename = defaultFileName;
        }
        else if (isprint (optopt)) {
          char message[50];
          sprintf(message, "Unknown option -%c. Terminating.", optopt);
          puts(message);
          printShortHelpMessage();
          buildAndAddErrorMessage(message, programName, sValue);
          if(!saveLog(filename)) {
            printf("Unable to save to specified file. Saving to default file.\n");
            saveLog(defaultFileName);
          }
          clearLog();
          return 1;
        }
        else {
          printShortHelpMessage();
          char message[50];
          sprintf(message, "Unknown option character `\\x%x'. Terminating.", optopt);
          buildAndAddErrorMessage(message, programName, sValue);
          if(!saveLog(filename)) {
            printf("Unable to save to specified file. Saving to default file.\n");
            saveLog(defaultFileName);
          }
          clearLog();
          return 1; 
        }
      }

  if(!saveLog(filename)) {
      printf("Unable to save to specified file. Saving to default file.\n");
      saveLog(defaultFileName);
    }
  clearLog();

  for (index = optind; index < argc; index++) {
    char message[21 + sizeof(argv[index])];
    sprintf(message, "Non-option argument %s", argv[index]);
    buildAndAddErrorMessage(message, programName, sValue);
    nonOptArgFlag = 1;
  }

  if(nonOptArgFlag) {
    printShortHelpMessage();
    if(!saveLog(filename)) {
      printf("Unable to save to specified file. Saving to default file.\n");
      saveLog(defaultFileName);
    }
    clearLog();
    return 1;
  }

  if(hflag) {
    printHelpMessage();
  }

  if(!hflag) {

  }
  if(!saveLog(filename)) {
      printf("Unable to save to specified file. Saving to default file.\n");
      saveLog(defaultFileName);
  }
  clearLog();

  return 0;
}
