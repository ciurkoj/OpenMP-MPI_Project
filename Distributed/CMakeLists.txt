cmake_minimum_required(VERSION 3.10)
project(Parallel VERSION 0.1.0)

set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/out)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
# set(HEADER_FILES ${PROJECT_SOURCE_DIR}ScheduleType.h ${PROJECT_SOURCE_DIR}vectorManipulation.h)

file(GLOB_RECURSE src SourceFiles/*.cpp)
file(GLOB_RECURSE headers SourceFiles/*.h)
add_executable(${PROJECT_NAME} ${src} ${headers})
# target_include_directories(Parallel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/*)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
target_link_libraries(${PROJECT_NAME} ${MPI_LIBRARIES} )

set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
