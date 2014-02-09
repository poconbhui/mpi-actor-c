MPICC=mpicc
CFLAGS=-L. -ltest -L../src -lactor

TESTS=actor_type.test

LIBTEST=libtest.a

all: $(TESTS)
	for test in $(TESTS); do\
		LD_LIBRARY_PATH=../src:$$LD_LIBRARY_PATH ./$$test; \
	done

%.test: %.test.o $(LIBTEST)
	$(MPICC) $^ -o $@ $(CFLAGS)

%.o: %.c
	$(MPICC) $^ -c -o $@

libtest.a: test.o
	rm libtest.a
	ar -cvq libtest.a test.o
