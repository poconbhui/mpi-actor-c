#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


/****************************************************************************/
/* MPI Actor library types                                                  */
/****************************************************************************/


/* Actor return statuses */
typedef int MPI_Actor_status;
#define MPI_ACTOR_ALIVE 1
#define MPI_ACTOR_DEAD  0

typedef struct MPI_Actor_type_ MPI_Actor_type;

/* Actor type definition functions */
typedef void (*MPI_Actor_type_init_fn)(
    MPI_Comm comm_actor, MPI_Actor_type actor_type, void* data
);
typedef MPI_Actor_status (*MPI_Actor_type_main_fn)(
    MPI_Comm comm_actor, MPI_Actor_type actor_type, void* data
);
typedef void (*MPI_Actor_type_cleanup_fn)(
    MPI_Comm comm_actor, MPI_Actor_type actor_type, void* data
);


/* Actor type struct */
struct MPI_Actor_type_ {
    MPI_Actor_type_init_fn init;
    MPI_Actor_type_main_fn main;
    MPI_Actor_type_cleanup_fn cleanup;

    int data_count;
    MPI_Datatype data_type;
};


/* Internal actor instance type */
typedef struct {
    MPI_Actor_type actor_type;
    void* data_ptr;
} _Actor;



/****************************************************************************/
/* User facing MPI Actor type management routines                           */
/****************************************************************************/


/* MPI Actor type creation */
int MPI_Actor_type_create(
    MPI_Actor_type_init_fn init,
    MPI_Actor_type_main_fn main,
    MPI_Actor_type_cleanup_fn cleanup,
    int data_count, MPI_Datatype data_type,
    MPI_Actor_type* actor_type
) {
    actor_type->init = init;
    actor_type->main = main;
    actor_type->cleanup = cleanup;

    actor_type->data_count = data_count;
    actor_type->data_type = data_type;

    return MPI_SUCCESS;
}


/* MPI Actor type committing */
int MPI_Actor_type_commit(MPI_Actor_type* actor_type) {
    return MPI_SUCCESS;
}


/* MPI Actor type freeing */
int MPI_Actor_type_free(MPI_Actor_type* actor_type) {
    return MPI_SUCCESS;
}


/* Get the data_size requested for an MPI Actor type */
int MPI_Actor_data_count(MPI_Actor_type actor_type, int* data_count) {
    *data_count = actor_type.data_count;

    return MPI_SUCCESS;
}


/* Get the data_type requested for an MPI Actor type */
int MPI_Actor_data_type(MPI_Actor_type actor_type, MPI_Datatype* data_type) {
    *data_type = actor_type.data_type;

    return MPI_SUCCESS;
}


/****************************************************************************/
/* Internal Actor instance management routines                              */
/****************************************************************************/


/* Initialise an instance of an Actor type */
int _actor_init(
    MPI_Comm comm_actor, MPI_Actor_type actor_type, _Actor* actor
) {
    /* Get the size of the MPI_Datatype for mallocing */
    int data_size;

    /* Set actor_type */
    actor->actor_type = actor_type;

    /* Allocate data_ptr */
    MPI_Type_size(actor_type.data_type, &data_size);
    actor->data_ptr = malloc(data_size*(actor_type.data_count));

    printf(
        "Size: %d x %d = %d\n",
        data_size, actor_type.data_count, data_size*(actor_type.data_count)
    );

    /* Run init function */
    actor_type.init(comm_actor, actor_type, actor->data_ptr);

    return MPI_SUCCESS;
}


/* Destroy an instance of an Actor type */
int _actor_destroy(MPI_Comm comm_actor, _Actor* actor) {
    actor->actor_type.cleanup(comm_actor, actor->actor_type, actor->data_ptr);

    free(actor->data_ptr);

    actor->data_ptr = NULL;

    return MPI_SUCCESS;
}



/****************************************************************************/
/* Define the test actor type                                               */
/****************************************************************************/


/* Data type info */
int test_actor_data_count = 10;
MPI_Datatype test_actor_data_type = MPI_FLOAT;


/* The initialisation routine */
void test_actor_init(
    MPI_Comm comm_actor, MPI_Actor_type actor_type, void* void_data
) {
    float *data = void_data;
    int data_count;
    int i;

    printf("Actor init!\n");

    MPI_Actor_data_count(actor_type, &data_count);

    for(i=0; i<data_count; i++) {
        data[i] = i;
    }
}


/* The main routine */
MPI_Actor_status test_actor_main(
    MPI_Comm comm_actor, MPI_Actor_type actor_type, void* void_data
) {
    float *data = void_data;
    int data_count;
    int i;

    printf("Actor main!\n");

    MPI_Actor_data_count(actor_type, &data_count);

    for(i=0; i<data_count; i++) {
        printf("-- Data %d: %f\n", i, data[i]);
    }

    return MPI_ACTOR_ALIVE;
}


/* The cleanup routine */
void test_actor_cleanup(
    MPI_Comm comm_actor, MPI_Actor_type actor_type, void* void_data
) {
    float *data = void_data;
    int data_count;
    int i;

    printf("Actor cleanup!\n");

    MPI_Actor_data_count(actor_type, &data_count);

    for(i=0; i<data_count; i++) {
        data[i] = 0;
    }
}



/* Do a quick ugly run of the routines outlined above */
int main(int argc, char* argv[]) {

    /* Cheat by using MPI_COMM_WORLD */
    MPI_Comm comm_actor = MPI_COMM_WORLD;

    MPI_Actor_type test_actor_type_1;
    MPI_Actor_type test_actor_type_2;

    _Actor test_actor_1;
    _Actor test_actor_2;


    MPI_Init(&argc, &argv);

    
    /* Create the test_actor_types */
    MPI_Actor_type_create(
        test_actor_init, test_actor_main, test_actor_cleanup,
        test_actor_data_count, test_actor_data_type,
        &test_actor_type_1
    );
    MPI_Actor_type_create(
        test_actor_init, test_actor_main, test_actor_cleanup,
        test_actor_data_count*2, test_actor_data_type,
        &test_actor_type_2
    );
    MPI_Actor_type_commit(&test_actor_type_1);
    MPI_Actor_type_commit(&test_actor_type_2);


    /* Create an instance of the test_actors */
    _actor_init(comm_actor, test_actor_type_1, &test_actor_1);
    _actor_init(comm_actor, test_actor_type_2, &test_actor_2);


    /* Try out the test_actor_type functions */

    /* test_actor_1 */
    test_actor_1.actor_type.init(
        comm_actor, test_actor_type_1, test_actor_1.data_ptr
    );
    test_actor_1.actor_type.main(
        comm_actor, test_actor_type_1, test_actor_1.data_ptr
    );
    test_actor_1.actor_type.cleanup(
        comm_actor, test_actor_type_1, test_actor_1.data_ptr
    );

    /* test_actor_2 */
    test_actor_2.actor_type.init(
        comm_actor, test_actor_type_2, test_actor_2.data_ptr
    );
    test_actor_2.actor_type.main(
        comm_actor, test_actor_type_2, test_actor_2.data_ptr
    );
    test_actor_2.actor_type.cleanup(
        comm_actor, test_actor_type_2, test_actor_2.data_ptr
    );


    /* Destroy the test_actors */
    _actor_destroy(comm_actor, &test_actor_1);
    _actor_destroy(comm_actor, &test_actor_2);


    /* Free the test_actor_types */
    MPI_Actor_type_free(&test_actor_type_1);
    MPI_Actor_type_free(&test_actor_type_2);


    MPI_Finalize();
    return EXIT_SUCCESS;
}
