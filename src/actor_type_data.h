#ifndef MPI_ACTOR_TYPE_DATA_H_
#define MPI_ACTOR_TYPE_DATA_H_


typedef struct {
    MPI_Actor_main_function *main;
    int count;
    MPI_Datatype type;
    void *initial_data;
} Actor_type_data;


int attach_Actor_type_data(
    MPI_Actor_main_function *actor_main_function,
    int count, MPI_Datatype type, void *initial_data,
    MPI_Datatype actor_type
);

int get_Actor_type_data(
    MPI_Datatype actor_type, Actor_type_data **actor_type_data, int *flag
);



#endif  /* MPI_ACTOR_TYPE_DATA_H_ */
