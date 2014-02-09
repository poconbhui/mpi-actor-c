MPICC=mpicc

all:
	make -C src

test: all
	make -C test
