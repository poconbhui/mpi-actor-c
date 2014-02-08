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
typedef void (*MPI_Actor_init_fn)(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* data
);
typedef MPI_Actor_status (*MPI_Actor_main_fn)(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* data
);
typedef void (*MPI_Actor_cleanup_fn)(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* data
);



/* Internal actor instance type */
typedef struct {
    MPI_Datatype actor_type;
    void* data_ptr;
} _Actor;



/****************************************************************************/
/* User facing MPI Actor type management routines                           */
/****************************************************************************/


/* Actor type keyval keys */
int actor_init_fn_key;
int actor_main_fn_key;
int actor_cleanup_fn_key;
int actor_data_count_key;


/* Allocate a new data_count_key space */
/* To allow actor_data_count_key_delete_fn to free data, duplicated
 * keyvals should be storing data in different memory locations */
int actor_data_count_key_copy_fn(
    MPI_Datatype actor_type,
    int data_count_key,
    void *extra_state,
    void *attribute_val_in,
    void *attribute_val_out,
    int *flag
) {
    int *attribute_val_in_ptr = attribute_val_in;
    int **attribute_val_out_ptr = attribute_val_out;

    *attribute_val_out_ptr = malloc(sizeof(int));
    **attribute_val_out_ptr = *attribute_val_in_ptr;

    *flag=1;

    return MPI_SUCCESS;
}


/* Free the data_count_key space */
int actor_data_count_key_delete_fn(
    MPI_Datatype actor_type,
    int data_count_key,
    void *attribute_val,
    void *extra_state
){
    free(attribute_val);

    return MPI_SUCCESS;
}


/* Initialise keyvals */
int _initialise_actor_type_keyvals(void) {
    static int initialised = MPI_ERR_UNKNOWN;

    if(initialised != MPI_SUCCESS) {
        MPI_Type_create_keyval(
            MPI_TYPE_DUP_FN,
            MPI_TYPE_NULL_DELETE_FN,
            &actor_init_fn_key,
            NULL
        );
        MPI_Type_create_keyval(
            MPI_TYPE_DUP_FN,
            MPI_TYPE_NULL_DELETE_FN,
            &actor_main_fn_key,
            NULL
        );
        MPI_Type_create_keyval(
            MPI_TYPE_DUP_FN,
            MPI_TYPE_NULL_DELETE_FN,
            &actor_cleanup_fn_key,
            NULL
        );

        MPI_Type_create_keyval(
            actor_data_count_key_copy_fn,
            actor_data_count_key_delete_fn,
            &actor_data_count_key,
            NULL
        );

        initialised = MPI_SUCCESS;
    }

    return initialised;
}


/* MPI Actor type creation */
int MPI_Type_create_actor(
    MPI_Actor_init_fn init,
    MPI_Actor_main_fn main,
    MPI_Actor_cleanup_fn cleanup,
    int data_count, MPI_Datatype data_type,
    MPI_Datatype* actor_type
) {

    /* Define the type and commit early.
     * MPI-2 says committing twice is allowed and a null-op
     */
    MPI_Type_contiguous(
        data_count, data_type,
        actor_type
    );
    MPI_Type_commit(actor_type);


    /* Initialise keyvals if not already initialised */
    _initialise_actor_type_keyvals();


    /* Set function keyvals */
    MPI_Type_set_attr(*actor_type, actor_init_fn_key, init);
    MPI_Type_set_attr(*actor_type, actor_main_fn_key, main);
    MPI_Type_set_attr(*actor_type, actor_cleanup_fn_key, cleanup);


    /* Allocate data for data_count keyval and set it */
    int *actor_data_count_key_data = malloc(sizeof(int));
    *actor_data_count_key_data = data_count;
    MPI_Type_set_attr(
        *actor_type, actor_data_count_key, actor_data_count_key_data
    );


    return MPI_SUCCESS;
}


/* Get the data_size requested for an MPI Actor type */
int MPI_Actor_data_count(MPI_Datatype actor_type, int* data_count) {
    int *data_count_ptr = NULL;
    int flag;

    MPI_Type_get_attr(actor_type, actor_data_count_key, &data_count_ptr, &flag);

    *data_count = *data_count_ptr;

    return MPI_SUCCESS;
}


/****************************************************************************/
/* Internal Actor instance management routines                              */
/****************************************************************************/


/* Initialise an instance of an Actor type */
int _actor_init(
    MPI_Comm comm_actor, MPI_Datatype actor_type, _Actor* actor
) {
    /* Get the size of the MPI_Datatype for mallocing */
    int data_size;
    int data_count;

    MPI_Actor_init_fn init_fn;
    int init_fn_flag;


    /* Set actor_type */
    actor->actor_type = actor_type;

    /* Allocate data_ptr */
    MPI_Type_size(actor_type, &data_size);
    MPI_Actor_data_count(actor_type, &data_count);

    actor->data_ptr = malloc(data_size);

    printf(
        "Size: %d x %d = %d\n",
        (int) ((float)data_size)/data_count, data_count, data_size
    );

    /* Run init function */
    MPI_Type_get_attr(actor_type, actor_init_fn_key, &init_fn, &init_fn_flag);
    init_fn(comm_actor, actor_type, actor->data_ptr);

    return MPI_SUCCESS;
}


/* Destroy an instance of an Actor type */
int _actor_destroy(MPI_Comm comm_actor, _Actor* actor) {
    MPI_Actor_cleanup_fn cleanup_fn;
    int flag;

    MPI_Type_get_attr(
        actor->actor_type, actor_cleanup_fn_key, &cleanup_fn, &flag
    );
    cleanup_fn(comm_actor, actor->actor_type, actor->data_ptr);

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
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* void_data
) {
    float *data = void_data;
    int data_count;
    int i;

    printf("Actor init!\n");

    MPI_Actor_data_count(actor_type, &data_count);

    printf("Size of data_count: %d\n", data_count);

    for(i=0; i<data_count; i++) {
        data[i] = i;
    }
}


/* The main routine */
MPI_Actor_status test_actor_main(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* void_data
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
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* void_data
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

    MPI_Datatype test_actor_type_1;
    MPI_Datatype test_actor_type_2;
    MPI_Datatype test_actor_type_copy;

    _Actor test_actor_1;
    _Actor test_actor_2;
    _Actor test_actor_copy;

    MPI_Actor_init_fn init_fn;
    MPI_Actor_main_fn main_fn;
    MPI_Actor_cleanup_fn cleanup_fn;

    int flag;


    MPI_Init(&argc, &argv);

    
    /* Create the test_actor_types */
    MPI_Type_create_actor(
        test_actor_init, test_actor_main, test_actor_cleanup,
        test_actor_data_count, test_actor_data_type,
        &test_actor_type_1
    );
    MPI_Type_create_actor(
        test_actor_init, test_actor_main, test_actor_cleanup,
        test_actor_data_count*2, test_actor_data_type,
        &test_actor_type_2
    );
    MPI_Type_commit(&test_actor_type_1);
    MPI_Type_commit(&test_actor_type_2);

    MPI_Type_dup(test_actor_type_1, &test_actor_type_copy);


    /* Create an instance of the test_actors */
    _actor_init(comm_actor, test_actor_type_1, &test_actor_1);
    _actor_init(comm_actor, test_actor_type_2, &test_actor_2);
    _actor_init(comm_actor, test_actor_type_copy, &test_actor_copy);


    /* Try out the test_actor_type functions */

    /* test_actor_1 */
    printf("Running test_actor_1\n");

    MPI_Type_get_attr(test_actor_type_1, actor_init_fn_key, &init_fn, &flag);
    init_fn(
        comm_actor, test_actor_type_1, test_actor_1.data_ptr
    );

    MPI_Type_get_attr(test_actor_type_1, actor_main_fn_key, &main_fn, &flag);
    main_fn(
        comm_actor, test_actor_type_1, test_actor_1.data_ptr
    );

    MPI_Type_get_attr(
        test_actor_type_1, actor_cleanup_fn_key, &cleanup_fn, &flag
    );
    cleanup_fn(
        comm_actor, test_actor_type_1, test_actor_1.data_ptr
    );

    /* test_actor_2 */
    printf("Running test_actor_2\n");

    MPI_Type_get_attr(test_actor_type_2, actor_init_fn_key, &init_fn, &flag);
    init_fn(
        comm_actor, test_actor_type_2, test_actor_2.data_ptr
    );

    MPI_Type_get_attr(test_actor_type_2, actor_main_fn_key, &main_fn, &flag);
    main_fn(
        comm_actor, test_actor_type_2, test_actor_2.data_ptr
    );

    MPI_Type_get_attr(
        test_actor_type_2, actor_cleanup_fn_key, &cleanup_fn, &flag
    );
    cleanup_fn(
        comm_actor, test_actor_type_2, test_actor_2.data_ptr
    );

    /* test_actor_copy */
    printf("Running test_actor_copy first time\n");

    MPI_Type_get_attr(test_actor_type_copy, actor_init_fn_key, &init_fn, &flag);
    init_fn(
        comm_actor, test_actor_type_copy, test_actor_copy.data_ptr
    );

    MPI_Type_get_attr(test_actor_type_copy, actor_main_fn_key, &main_fn, &flag);
    main_fn(
        comm_actor, test_actor_type_copy, test_actor_copy.data_ptr
    );

    MPI_Type_get_attr(
        test_actor_type_copy, actor_cleanup_fn_key, &cleanup_fn, &flag
    );
    cleanup_fn(
        comm_actor, test_actor_type_copy, test_actor_copy.data_ptr
    );


    /* Destroy the test_actors */
    _actor_destroy(comm_actor, &test_actor_1);
    _actor_destroy(comm_actor, &test_actor_2);


    /* Free the test_actor_types */
    MPI_Type_free(&test_actor_type_1);
    MPI_Type_free(&test_actor_type_2);


    /* Try the copy again */

    /* test_actor_copy */
    printf("Running test_actor_copy second time\n");
    MPI_Type_get_attr(test_actor_type_copy, actor_init_fn_key, &init_fn, &flag);
    init_fn(
        comm_actor, test_actor_type_copy, test_actor_copy.data_ptr
    );

    MPI_Type_get_attr(test_actor_type_copy, actor_main_fn_key, &main_fn, &flag);
    main_fn(
        comm_actor, test_actor_type_copy, test_actor_copy.data_ptr
    );

    MPI_Type_get_attr(
        test_actor_type_copy, actor_cleanup_fn_key, &cleanup_fn, &flag
    );
    cleanup_fn(
        comm_actor, test_actor_type_copy, test_actor_copy.data_ptr
    );


    /* Delete the copy */
    _actor_destroy(comm_actor, &test_actor_copy);
    MPI_Type_free(&test_actor_type_copy);

    MPI_Finalize();
    return EXIT_SUCCESS;
}
