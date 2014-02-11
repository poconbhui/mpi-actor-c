#include_directories("${PROJECT_SOURCE_DIR}/src")

add_library( actor
    "${PROJECT_SOURCE_DIR}/src/actor_create.c"
    "${PROJECT_SOURCE_DIR}/src/type_create_actor.c"
)
