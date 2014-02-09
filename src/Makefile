MPICC=mpicc
CFLAGS=-Wall

all: libactor.so

libactor.so = actor_type.o actor_create.o

%.o: %.c
	$(MPICC) $(CFLAGS) $^ -fPIC -c -o $@

libactor.so: $(libactor.so)
	$(MPICC) -shared -Wl,-soname,libactor.so -o $@ $(libactor.so)
