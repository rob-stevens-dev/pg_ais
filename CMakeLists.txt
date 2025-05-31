cmake_minimum_required(VERSION 3.10)
project(pg_ais C)

set(CMAKE_C_STANDARD 99)

# Locate pg_config
find_program(PG_CONFIG pg_config REQUIRED)

# Get PostgreSQL include and lib paths
execute_process(
    COMMAND ${PG_CONFIG} --includedir-server
    OUTPUT_VARIABLE PG_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${PG_CONFIG} --libdir
    OUTPUT_VARIABLE PG_LIB_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${PG_CONFIG} --pkglibdir
    OUTPUT_VARIABLE PG_PKGLIB_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

include_directories(${PG_INCLUDE_DIR} src)
link_directories(${PG_LIB_DIR})

# Extension library
add_library(pg_ais SHARED
    src/pg_ais.c
    src/parse_ais.c
    src/ais_core.c
)

target_include_directories(pg_ais PRIVATE
    ${PG_INCLUDE_DIR}
    src
)

target_link_libraries(pg_ais PRIVATE pq)

# Output to PostgreSQL libdir
set_target_properties(pg_ais PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PG_PKGLIB_DIR}
    PREFIX ""
)

# Install the shared library
install(TARGETS pg_ais DESTINATION ${PG_PKGLIB_DIR})

# -------------------------------
# CMocka Unit Test Configuration
# -------------------------------
enable_testing()

add_executable(pg_ais_tests
    test/test_pg_ais.c
    src/pg_ais_core.c
    src/ais_core.c
)

target_include_directories(pg_ais_tests PRIVATE
    ${PG_INCLUDE_DIR}
    src
)

target_link_libraries(pg_ais_tests PRIVATE
    cmocka
)

add_test(NAME pg_ais_tests COMMAND pg_ais_tests)
