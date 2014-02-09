#ifndef MPI_ACTOR_TYPE_H_
#define MPI_ACTOR_TYPE_H_


#include "actor.h"


/* Actor status definitions */

/* These values are returned from the Actor main function to signify */
/* whether the actor is still alive or if it is dead.                */
#define MPI_ACTOR_ALIVE 1
#define MPI_ACTOR_DEAD  0


/* Actor function prototypes */

/* These are used to define the function prototypes accepted by    */
/* the MPI_Type_create_actor routine for generating an Actor type. */
/* They correspond roughly to OO type class initialisation and     */
/* destruction routines with a member function called "main" which */
/* will be run when an actor receives a message.                   */
typedef void MPI_Actor_initialise_function(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* state
);

typedef int MPI_Actor_main_function(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* state
);

typedef void MPI_Actor_destroy_function(
    MPI_Comm comm_actor, MPI_Datatype actor_type, void* state
);


/* Actor creation function                          */
/* Not thread safe because of keyval initialisation */

/* This function creates an MPI_Datatype corresponding to an Actor  */
/* with an initialisation routine, a "main" routine which will be   */
/* run any time a message arrives for it, and a destruction routine */
/* which will be run when the actor dies.                           */
int MPI_Type_create_actor(
    MPI_Actor_initialise_function *actor_initialise_function,
    MPI_Actor_main_function *actor_main_function,
    MPI_Actor_destroy_function *actor_destroy_function,
    int count, MPI_Datatype type,
    MPI_Datatype *actor_type
);


/* Return the number of data elements held by the Actor type */
int MPI_Actor_data_count(MPI_Datatype actor_type, int *count);

/* Return the underlying data type being used by the Actor type */
int MPI_Actor_data_type(MPI_Datatype actor_type, MPI_Datatype *type);


#endif // MPI_ACTOR_TYPE_H_
