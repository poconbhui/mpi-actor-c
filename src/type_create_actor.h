#ifndef MPI_ACTOR_TYPE_H_
#define MPI_ACTOR_TYPE_H_


#include "actor.h"


/* Actor status definitions */

/* These values are returned from the Actor main function to signify */
/* whether the actor is still alive or if it is dead.                */
#define MPI_ACTOR_ALIVE 1
#define MPI_ACTOR_DEAD  0


/* Actor function prototypes */

/* These are used to define the function prototype accepted by     */
/* the MPI_Type_create_actor routine for generating an Actor type. */
/* The MPI_Actor_main_function will be run any time a message      */
/* arrives for an actor.                                           */
typedef int MPI_Actor_main_function(
    MPI_Comm comm_actor, MPI_Datatype actor_type,
    void* message, int tag,
    void* state
);


/* Actor creation function                          */
/* Not thread safe because of keyval initialisation */

/* This function creates an MPI_Datatype corresponding to an Actor  */
/* with an initialisation routine, a "main" routine which will be   */
/* run any time a message arrives for it, and a destruction routine */
/* which will be run when the actor dies.                           */
/* When a new instance of an actor of type actor_type is created,   */
/* it will be passed a copy of the initial_data array.              */
int MPI_Type_create_actor(
    MPI_Actor_main_function *actor_main_function,
    int count, MPI_Datatype type, void* initial_data,
    MPI_Datatype *actor_type
);


/* Return the number of data elements and type held by the Actor type */
int MPI_Actor_get_datatypes(
    MPI_Datatype actor_type, int *count, MPI_Datatype *type
);

/* Copy the initial data passed to an instance of an actor to initial_data */
int MPI_Actor_get_data(
    MPI_Datatype actor_type,
    int count, MPI_Datatype type,
    void *initial_data
);


#endif /* MPI_ACTOR_TYPE_H_ */
