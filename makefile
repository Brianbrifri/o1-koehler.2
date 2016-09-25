all: slavedriver slaverunner
masterObjects = master.o logger.o
slaveObjects = slave.o

slavedriver: $(masterObjects)
	gcc -g -o slavedriver $(masterObjects)
master.o: master.c logger.h
	gcc -g -c master.c
logger.o: logger.c logger.h
	gcc -g -c logger.c

slaverunner: slave.c
	gcc -g -o slaverunner slave.c

.PHONY: clean
clean:
	-rm slavedriver slaverunner $(masterObjects) $(slaveObjects)
