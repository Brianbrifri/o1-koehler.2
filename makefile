all: slavedriver slaverunner
masterObjects = master.o 
slaveObjects = slave.o

slavedriver: master.c
	gcc -g -o slavedriver master.c

slaverunner: slave.c
	gcc -g -o slaverunner slave.c

.PHONY: clean
clean:
	-rm slavedriver slaverunner $(masterObjects) $(slaveObjects)
