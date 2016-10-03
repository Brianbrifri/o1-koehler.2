#include <ctype.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#include "struct.h"

void alarmHandler(int);
void sigquitHandler(int);
void zombieKiller(int);
volatile sig_atomic_t sigNotReceived = 1;
pid_t myPid;

int main (int argc, char **argv) {
  srand(time(0));
  int iValue = 3;
  int tValue = 30;
  int nValue = 0;
  int shmid = 0;
  data *sharedStates;
  int *sharedInt;
  char *fileName;
  char *defaultFileName = "test.out";
  char *option = NULL;
  char *short_options = "i:l:m:n:t:";
  FILE *file;
  int c;
  myPid = getpid();

  //Ignore SIGINT so that it can be handled below
  signal(SIGINT, SIG_IGN);

  //get options from parent process
  opterr = 0;
  while((c = getopt(argc, argv, short_options)) != -1) 
    switch (c) {
      case 'i':
        iValue = atoi(optarg);
        break;
      case 'l':
        fileName = optarg;
        break;
      case 'm':
        shmid = atoi(optarg);
        break;
      case 'n':
        nValue = atoi(optarg);
        break;
      case 't':
        tValue = atoi(optarg) + 10;
        break;
      case '?':
        fprintf(stderr, "    Arguments were not passed correctly to slave %d. Terminating.", myPid);
        exit(-1);
    }

  if((sharedStates = (data *)shmat(shmid, NULL, 0)) == (void *) -1) {
    perror("Could not attach shared mem");
    exit(1);
  }
  fprintf(stderr, "    Slave %d attached to shared memory location %d\n", nValue, shmid);

  

  //set the sigquitHandler for the SIGQUIT signal
  signal(SIGQUIT, sigquitHandler);

  //Set an alarm to 10 more seconds than the parent process
  //so that the child will be killed if parents gets killed
  //and child becomes slave of init
  signal(SIGALRM, zombieKiller);
  alarm(tValue);

  int i = 0;
  int j;
  int random;

  //While loop to write iValue times as long as the quit signal has not been received
  while(i < iValue && sigNotReceived) {
    do {
      fprintf(stderr,"    Slave %d entrance %d\n", nValue, i + 1);

      //Raise my flag
      sharedStates->flag[nValue] = want_in;
      //Set local variable
      j = sharedStates->turn;

      //Wait until it's my turn
      while(j != nValue) {
        j = (sharedStates->flag[j] != idle) ? sharedStates->turn : (j + 1) % sharedStates->totalProcesses;
      }


      //Declare intentions to enter critical section
      sharedStates->flag[nValue] = in_cs;
      random = rand() % 3;
      printf("Random sleep value: %d\n", random);
      sleep(random);

      //Check that no one else is in the critical section
      for(j = 0; j < sharedStates->totalProcesses; j++) {
        if((j != nValue) && (sharedStates->flag[j] == in_cs)) {
          break;
        }
      }

    }while ((j < sharedStates->totalProcesses) || (sharedStates->turn != nValue && sharedStates->flag[j] != idle));

    //Increment shared variable
    sharedStates->sharedInt++;
    fprintf(stderr,"    Slave %d about to enter critical section...\n", nValue);

    //Assign turn to self and enter critical section
    sharedStates->turn = nValue;

    file = fopen(fileName, "a");
    if(!file) {
      perror("Error opening file");
      exit(-1);
    }
    
    fprintf(file,"    File modified by process number %d at the time with shared number %d\n", nValue + 1, sharedStates->sharedInt);
    if(fclose(file)) {
      perror("Error closing file");
    }

    random = rand() % 3;
    printf("Random sleep value: %d\n", random);
    sleep(random);

    //Exit section
    j = (sharedStates->turn + 1) % sharedStates->totalProcesses;

    while(sharedStates->flag[j] == idle) {
      j = (j + 1) % sharedStates->totalProcesses;
    }

    //Assign turn to next waiting process and change own flag to idle
    sharedStates->turn = j;
    sharedStates->flag[nValue] = idle;

    fprintf(stderr,"    Slave %d about to exit critical section...\n", nValue);
    i++;
    
  }
  
  if(sigNotReceived) {
    fprintf(stderr, "    Slave %d finished with work\n", nValue);
  }

  if(shmdt(sharedStates) == -1) {
    perror("Slave could not detach shared memory");
  }

  fprintf(stderr, "    Slave %d detached from shared memory location %d\n", nValue, shmid);

  kill(myPid, SIGTERM);
  sleep(1);
  kill(myPid, SIGKILL);
  
}

//This handles SIGQUIT being sent from parent process
//It sets the volatile int to 0 so that the while loop will exit. 
void sigquitHandler(int sig) {
  printf("    PID %d has received signal %s (%d)\n", myPid, strsignal(sig), sig);
  sigNotReceived = 0;
}

//function to kill itself if the alarm goes off,
//signaling that the parent could not kill it off
void zombieKiller(int sig) {
  printf("Slave %d is killing itself\n", myPid);
  kill(myPid, SIGTERM);
  sleep(1);
  kill(myPid, SIGKILL);
}
