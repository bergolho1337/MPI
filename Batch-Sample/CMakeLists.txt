cmake_minimum_required(VERSION 2.8)

project(mpi_batch)

# Require MPI for this project:
find_package(MPI REQUIRED)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin )
include_directories(${MPI_INCLUDE_PATH})

add_executable(mpi_batch mpi_batch.c)

target_link_libraries(mpi_batch ${MPI_LIBRARIES} m)
