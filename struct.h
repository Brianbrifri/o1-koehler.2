#ifndef STRUCT_H
#define STRUCT_H
enum state { idle, want_in, in_cs};

typedef struct data_struct {
  int sharedInt;
  int turn;
  int totalProcesses;
  enum state flag[];
} data;

#endif
