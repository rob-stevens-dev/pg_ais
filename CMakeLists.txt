cmake_minimum_required(VERSION 3.15)
project(pg_ais C)

# Compiler settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

# PostgreSQL discovery
find_package(PostgreSQL REQUIRED)
find_program(PG_CONFIG_EXECUTABLE pg_config REQUIRED)

execute_process(COMMAND ${PG_CONFIG_EXECUTABLE} --includedir
                OUTPUT_VARIABLE PG_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${PG_CONFIG_EXECUTABLE} --includedir-server
                OUTPUT_VARIABLE PG_INCLUDE_DIR_SERVER OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${PG_CONFIG_EXECUTABLE} --libdir
                OUTPUT_VARIABLE PG_LIB_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${PG_CONFIG_EXECUTABLE} --sharedir
                OUTPUT_VARIABLE PG_SHARE_DIR_BASE OUTPUT_STRIP_TRAILING_WHITESPACE)

# Install directories
set(PG_EXTENSION_DIR ${PG_LIB_DIR})
set(PG_SHARE_DIR ${PG_SHARE_DIR_BASE}/extension)

# Include and link paths
include_directories(${PG_INCLUDE_DIR})
include_directories(${PG_INCLUDE_DIR_SERVER})
link_directories(${PG_LIB_DIR})

# Source files for the extension
set(SOURCES
    src/pg_ais_core.c
    src/parse_ais.c
    src/ais_core.c
)

# Build shared object (must not have lib prefix)
add_library(pg_ais SHARED ${SOURCES})
set_target_properties(pg_ais PROPERTIES PREFIX "")
target_include_directories(pg_ais PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(pg_ais ${PostgreSQL_LIBRARIES})

# Install extension .so file to PostgreSQL lib directory
install(TARGETS pg_ais
        LIBRARY DESTINATION ${PG_EXTENSION_DIR})

# Install control and SQL files to extension sharedir
install(FILES
    pg_ais.control
    sql/pg_ais--0.1.sql
    DESTINATION ${PG_SHARE_DIR})

# Testing (only pure C logic, no PostgreSQL server APIs)
enable_testing()
add_executable(pg_ais_tests
    test/test_pg_ais.c
    src/parse_ais.c
)
target_compile_definitions(pg_ais_tests PRIVATE UNIT_TEST)
target_include_directories(pg_ais_tests PRIVATE ${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(pg_ais_tests cmocka)

add_test(NAME pg_ais_tests COMMAND pg_ais_tests)

# Optional test payloads (auto-generated)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/auto_test_payloads.c")
    add_executable(auto_payload_tests
        test/auto_test_payloads.c
        src/parse_ais.c
        src/parse_ais_msg.c
        src/bitfield.c
        src/shared_ais_utils.c
    )
    target_compile_definitions(auto_payload_tests PRIVATE UNIT_TEST)
    target_link_libraries(auto_payload_tests cmocka)
    add_test(NAME auto_payload_tests COMMAND auto_payload_tests)
endif()