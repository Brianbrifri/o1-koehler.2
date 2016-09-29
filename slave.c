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
  int *sharedInt;
  char *fileName;
  char *defaultFileName = "test.out";
  char *option = NULL;
  char *short_options = "i:l:m:n:t:";
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

  if((sharedInt = (int *)shmat(shmid, NULL, 0)) == (void *) -1) {
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

  //While loop to write iValue times as long as the quit signal has not been received
  while(i < iValue && sigNotReceived) {
    fprintf(stderr,"    Slave %d entrance %d\n", nValue, i + 1);
    fprintf(stderr,"    Slave %d about to enter critical section...\n", nValue);
    int random = rand() % 3;
    sleep(random);
    fprintf(stderr,"    Slave %d incrementing variable...\n", nValue);
    fprintf(stderr,"    Slave %d writing to file...\n", nValue);
    random = rand() % 3;
    sleep(random);
    fprintf(stderr,"    Slave %d about to exit critical section...\n", nValue);
    i++;
  }

  if(shmdt(sharedInt) == -1) {
    perror("Slave tried to detach shared memory");
  }

  fprintf(stderr, "    Slave %d detached from shared memory location %d\n", nValue, shmid);

  kill(myPid, SIGTERM);
  sleep(1);
  kill(myPid, SIGKILL);
  
}

//This handles SIGQUIT being sent from parent process
//It sets the volatile int to 0 so that the while loop will exit. 
void sigquitHandler(int sig) {
  printf("    Slave %d has received signal %s (%d)\n", myPid, strsignal(sig), sig);
  sigNotReceived = 0;
}

//function to kill itself if the alarm goes off,
//signaling that the parent could not kill it off
void zombieKiller(int sig) {
  kill(myPid, SIGTERM);
  sleep(1);
  kill(myPid, SIGKILL);
}
