MPICC=mpicc

CONCEPTS.C=$(wildcard *.c)
CONCEPTS=$(patsubst %.c, %, $(CONCEPTS.C)) 

all: $(CONCEPTS)

run: all
	for concept in $(CONCEPTS); do ./$$concept; done

%.concept: %.concept.c
	$(MPICC) $^ -o $@ $(CFLAGS)
