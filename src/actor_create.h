#ifndef MPI_ACTOR_CREATE_H_
#define MPI_ACTOR_CREATE_H_

/** Create an Actor communicator.
 *
 * This is a blocking call. All processes should supply the same
 * values for the arguments.
 */
int MPI_Actor_create(
    MPI_Comm comm_old, int nactor_types,
    MPI_Datatype *actor_types,
    MPI_Comm *comm_actor
);


/** Retrieve information about an Actor communicator.
 *
 * This returns a list of the actor types registered to an Actor communicator.
 */
int MPI_Actor_get(
    MPI_Comm comm_actor, int nactor_types,
    MPI_Datatype *actor_types
);

#endif  // MPI_ACTOR_CREATE_H_
