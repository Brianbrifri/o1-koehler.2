objects = master.o logger.o

master: $(objects)
	gcc -g -o master $(objects)
master.o: master.c logger.h
	gcc -g -c master.c
logger.o: logger.c logger.h
	gcc -g -c logger.c

.PHONY: clean
clean:
	-rm master $(objects)
