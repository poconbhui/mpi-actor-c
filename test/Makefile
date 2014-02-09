MPICC=mpicc
CFLAGS=-Wall
CLIBS=-L. -ltest -L../src -lactor

TESTS.C=$(wildcard *.test.c)
TESTS=$(patsubst %.test.c, %.test, $(TESTS.C))

LIBTEST=libtest.a

all: $(TESTS)
	for test in $(TESTS); do\
		LD_LIBRARY_PATH=../src:$$LD_LIBRARY_PATH ./$$test; \
	done

%.test: %.test.o $(LIBTEST)
	$(MPICC) $(CFLAGS) $^ -o $@ $(CLIBS)

%.o: %.c
	$(MPICC) $(CFLAGS) $^ -c -o $@ $(CLIBS)

libtest.a: test.o
	rm -f libtest.a
	ar -cvq libtest.a test.o

clean:
	rm -f libtest.a
	rm -f *.o
	rm -f $(TESTS)
