#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


// Actor return status
typedef int MPI_Actor_status;
#define MPI_ACTOR_ALIVE 1
#define MPI_ACTOR_DEAD  0

// Actor type functions
typedef void (*MPI_Actor_type_init_fn)(void);
typedef MPI_Actor_status (*MPI_Actor_type_main_fn)(void);
typedef void (*MPI_Actor_type_cleanup_fn)(void);


// Actor type struct
typedef struct {
    MPI_Actor_type_init_fn init;
    MPI_Actor_type_main_fn main;
    MPI_Actor_type_cleanup_fn cleanup;
} MPI_Actor_type;


void test_actor_init() {
    printf("Actor init!\n");
}

MPI_Actor_status test_actor_main() {
    printf("Actor main!\n");

    return MPI_ACTOR_ALIVE;
}

void test_actor_cleanup() {
    printf("Actor cleanup!\n");
}
int MPI_Actor_type_create(
    MPI_Actor_type_init_fn init,
    MPI_Actor_type_main_fn main,
    MPI_Actor_type_cleanup_fn cleanup,
    MPI_Actor_type* actor_type
) {
    actor_type->init = init;
    actor_type->main = main;
    actor_type->cleanup = cleanup;

    return MPI_SUCCESS;
}

int MPI_Actor_type_commit(MPI_Actor_type* actor_type) { return MPI_SUCCESS; }


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    MPI_Actor_type test_actor;
    
    MPI_Actor_type_create(
        test_actor_init, test_actor_main, test_actor_cleanup,
        &test_actor
    );

    MPI_Actor_type_commit(&test_actor);

    test_actor.init();
    test_actor.main();
    test_actor.cleanup();


    printf("Hello\n");

    MPI_Finalize();
    return EXIT_SUCCESS;
}
