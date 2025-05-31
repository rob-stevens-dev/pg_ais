cmake_minimum_required(VERSION 3.10)
project(pg_ais C)

set(CMAKE_C_STANDARD 11)

find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})

add_library(pg_ais SHARED src/pg_ais.c)
target_link_libraries(pg_ais ${PostgreSQL_LIBRARIES})

set_target_properties(pg_ais PROPERTIES PREFIX "")
install(TARGETS pg_ais DESTINATION lib)
install(FILES sql/pg_ais--0.1.sql DESTINATION share/postgresql/extension)
install(FILES pg_ais.control DESTINATION share/postgresql/extension)
