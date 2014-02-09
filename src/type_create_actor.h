#ifndef MPI_ACTOR_TYPE_H_
#define MPI_ACTOR_TYPE_H_

#include <mpi.h>


/* Actor status definitions */
#define MPI_ACTOR_ALIVE 1
#define MPI_ACTOR_DEAD  0


/* Actor function definitions */
typedef void MPI_Actor_initialise_function(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* state
);

typedef int MPI_Actor_main_function(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* state
);

typedef void MPI_Actor_destroy_function(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* state
);


/* Actor creation function */
int MPI_Type_create_actor(
    MPI_Actor_initialise_function *actor_initialise_function,
    MPI_Actor_main_function *actor_main_function,
    MPI_Actor_destroy_function *actor_destroy_function,
    int count, MPI_Datatype type,
    MPI_Datatype *actor_type
);


/* Return the number of data elements held by the Actor type */
int MPI_Actor_data_count(MPI_Datatype actor_type, int *count);

/* Return the underlying data type being used by the actor */
int MPI_Actor_data_type(MPI_Datatype actor_type, MPI_Datatype *type);


#endif // MPI_ACTOR_TYPE_H_
