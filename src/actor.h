#ifndef MPI_ACTOR_H_
#define MPI_ACTOR_H_


#include <mpi.h>


/* Global type definitions */
struct Actor_model_state_s;
typedef struct Actor_model_state_s* MPI_Actor_model_state;

#include "type_create_actor.h"
#include "actor_create.h"
#include "actor_start.h"


#endif  // MPI_ACTOR_H_
