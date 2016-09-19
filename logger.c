#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "logger.h"

typedef struct list_struct {
  data_t item;
  struct list_struct *next;
} log_t;

static log_t *headptr = NULL;
static log_t *tailptr = NULL;

char * addMsg(data_t data) {
  log_t *newEntry;
  int entrySize;

  entrySize = sizeof(log_t) + strlen(data.string) + 1;
  if((newEntry = (log_t *) (malloc(entrySize))) == NULL) {
    perror("Error: malloc newEntry ");
    return "Could not malloc for log\n";
  }

  newEntry->item.time = data.time;
  newEntry->item.string = (char *)newEntry + sizeof(log_t);
  strcpy(newEntry->item.string, data.string);
  newEntry->next = NULL;

  if(headptr == NULL) {
    headptr = newEntry;
  }
  else {
    tailptr->next = newEntry;
  }
  tailptr = newEntry;

  return "Successfully added data to log\n";
}

void clearLog(void) {
  if(!headptr) {
    return;
  }
  log_t *nodeptr;
  log_t *next;

  nodeptr = headptr;
  next = nodeptr->next;

  while(nodeptr) {
    next = nodeptr->next;
    free(nodeptr);
    nodeptr = next;
  }

  headptr = NULL;
  tailptr = NULL;
}

char *getLog(void) {
  log_t *nodeptr;
  size_t nodeSize = 30;
  char *logString;
  char *noError = malloc(nodeSize);

  noError[0] = 0;

  strcat(noError, "No logs t'day mate!\n\n");

  if(!headptr) {
    return noError;
  }

  nodeptr = headptr;

  while(nodeptr) {
    nodeSize += (sizeof(log_t) + strlen(nodeptr->item.string) + 4);
    nodeptr = nodeptr->next;
  }

  logString = malloc(nodeSize);
  if(!logString) {
    perror("Error malloc-ing for logString ");
    return noError;
  }

  logString[0] = 0;
  strcat(logString, "*****Begin Log*****\n\n");
  nodeptr = headptr;

  while(nodeptr) {
    strcat(logString, nodeptr->item.string);
    strcat(logString, "\n");
    nodeptr = nodeptr->next;
  }
  strcat(logString, "\n");

  free(noError);
  return logString;
}

int saveLog(char *filename) {
  char *logString; 
  FILE *file = fopen(filename, "a");

  if(!file) {
    perror("Error opening file ");
    return 0;
  }
  else {

    logString = getLog();
    fprintf(file, "%s", logString);

    if(fclose(file)) {
      perror("Error closing file ");
    } 
  }
  free(logString);
  return 1;
}

void printHelpMessage(void) {
    printf("\nThank you for using the help menu!\n");
    printf("The following is a helpful guide to enable you to use this\n");
    printf("debugger program to the best of your ability!\n\n");
    printf("-h, --help: Prints this help message.\n");
    printf("-n: Allows you to set the number of messages to the alien planet Krudo.\n");
    printf("\tThe default value is 42.\n");
    printf("-l: Allows you to set the filename for the logger so the aliens can see how bad you mess up.\n");
    printf("\tThe default value is logfile.txt.\n");
}

void printShortHelpMessage(void) {
  printf("\nAcceptable options are:\n");
  printf("[-h], [--help], [-l][required_arg], [-n][required_arg]\n\n");
}

void buildAndAddErrorMessage(char *errorMessage, char *programName, int nValue) {
  data_t newEntry;

  clock_gettime(CLOCK_REALTIME, &newEntry.time);
  long unsigned timeInMillisNum = newEntry.time.tv_nsec;
  long unsigned timeInSecsNum = newEntry.time.tv_sec;

  char timeInMillis[30];
  char nValueMessage[30];

  sprintf(timeInMillis, "%lu%lu", timeInSecsNum, timeInMillisNum);
  sprintf(nValueMessage, "nValue = %d - ", nValue);
  int size = strlen(timeInMillis) + strlen(errorMessage) + strlen(nValueMessage) + strlen(programName) + 50;

  char fullError[size];
  fullError[0] = 0;

  strcat(fullError, programName);
  strcat(fullError, ": ");
  strcat(fullError, timeInMillis);
  strcat(fullError, ": Error: ");
  strcat(fullError, nValueMessage);
  strcat(fullError, errorMessage);
  newEntry.string = fullError;
  printf("%s", addMsg(newEntry));
}
