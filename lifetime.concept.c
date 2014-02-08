#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


/* Handle for the actor_keyval attributes */
int actor_keyval;


/* Initialise the attributes for the actor_keyval on the */
/* input communicator.                                   */
/* This is called in MPI_Actor_create(...) */
int actor_keyval_init(MPI_Comm comm_actor) {
    printf("Initialising actor_keyval attrs\n");

    int *attrs = malloc(5*sizeof(int));
    int i;

    for(i=0; i<5; ++i) {
        attrs[i] = i;
    }

    /* Return error value from setting */
    return MPI_Comm_set_attr(comm_actor, actor_keyval, attrs);
}


/* Destroy the actor_keyval attributes */
int actor_keyval_delete(
    MPI_Comm comm_actor, int actor_keyval,
    void* attrs, void* extra_state
) {
    printf("Deleting actor_keyval attrs\n");

    free(attrs);

    return MPI_SUCCESS;
}


/* Initialise the MPI_Actor library. */
/* Expect to be called at every entry and ignored all but the first time. */
int MPI_Actor_init() {
    static int initialised = MPI_ERR_UNKNOWN;

    if(initialised != MPI_SUCCESS) {
        printf("Initialising actor_keyval\n");

        initialised = MPI_Comm_create_keyval(
            MPI_COMM_NULL_COPY_FN,
            actor_keyval_delete,
            &actor_keyval,
            NULL
        );
    }

    return initialised;
}


/* Create an actor communicator */
int MPI_Actor_create(MPI_Comm comm_old, MPI_Comm *comm_actor) {
    /* Run MPI_Actor_init at all possible entry points to the library */
    MPI_Actor_init();

    MPI_Comm_dup(comm_old, comm_actor);
    return actor_keyval_init(*comm_actor);
}


/* Return whether the input communicator is an actor communicator */
int is_actor_comm(MPI_Comm comm) {
    void* v;
    int flag;

    /* Run MPI_Actor_init at all possible entry points to the library */
    MPI_Actor_init();

    MPI_Comm_get_attr(comm, actor_keyval, &v, &flag);

    return (flag==1);
}


int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);


    MPI_Comm comm_actor;


    /* Is MPI_COMM_WORLD an actor comm? */
    /* Expect to see a message about construction here. */
    printf("CW: %d\n", is_actor_comm(MPI_COMM_WORLD));

    /* Is comm_actor an actor comm? */
    MPI_Actor_create(MPI_COMM_WORLD, &comm_actor);
    printf("CA: %d\n", is_actor_comm(comm_actor));

    /* Expect to see a message about destruction here. */
    MPI_Comm_free(&comm_actor);


    MPI_Finalize();
    return EXIT_SUCCESS;
}
