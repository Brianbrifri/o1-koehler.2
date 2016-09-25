#include <ctype.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <time.h>


void alarmHandler(int);
void sigquitHandler(int);

int main (int argc, char **argv) {
  int iValue = 3;
  int tValue = 30;
  char *fileName;
  char *defaultFileName = "test.out";
  char *option = NULL;
  char *short_options = "i:l:t:";
  int c;

  while((c = getopt_long (argc, argv, short_options, NULL, NULL)) != -1) 
    switch (c) {
      case 'i':
        iValue = atoi(optarg);
        break;
      case 'l':
        fileName = optarg;
        break;
      case 't':
        tValue = atoi(optarg) + 10;
        break;
      case '?':
        fprintf(stderr, "    Arguments were not passed correctly to slave %d. Terminating.", getpid());
        return -1;
    }
  
  signal(SIGQUIT, sigquitHandler);
  int i;
  for(i = 0; i < iValue; i++) {
    fprintf(stderr,"    Slave %d about to enter critical section...\n", getpid());
    sleep(1);
    fprintf(stderr,"    Slave %d incrementing variable...\n", getpid());
    fprintf(stderr,"    Slave %d writing to file...\n", getpid());
    sleep(1);
    fprintf(stderr,"    Slave %d about to exit critical section...\n", getpid());
  }
  
}

void sigquitHandler(int sig) {
  printf("Child %d has received sigquit signal\n", getpid());
  kill(getpid(), SIGTERM);
  sleep(1);
  kill(getpid(), SIGKILL);
}

