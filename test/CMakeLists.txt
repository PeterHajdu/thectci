# required for igloo:
add_definitions(-Wno-deprecated-declarations)
include_directories(igloo)

set(TEST_SOURCE_FILES
    test_runner.cpp
    test_hash.cpp
    test_id.cpp
    test_factory.cpp
    test_dispatcher.cpp
    test_component_registry.cpp
    test_service_registry.cpp
    )


add_executable(test_runner EXCLUDE_FROM_ALL ${TEST_SOURCE_FILES})
target_link_libraries(test_runner thectci ${LIBS})

get_target_property(TEST_RUNNER_BIN test_runner LOCATION) # for running

find_program( VALGRIND_PATH valgrind )

if(VALGRIND_PATH)
  set(RUNNER
    valgrind --leak-check=full --show-reachable=yes --log-file="valgrind.log"
    )
endif()

add_custom_target(test DEPENDS test_runner)
add_custom_command(TARGET test COMMAND ${RUNNER} ${TEST_RUNNER_BIN} --verbose DEPENDS test_runner)
