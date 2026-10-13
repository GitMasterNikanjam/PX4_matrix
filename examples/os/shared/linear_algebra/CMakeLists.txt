cmake_minimum_required(VERSION 3.14)

project(Matrix_exp1 LANGUAGES CXX)

get_filename_component(PX4_MATRIX_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)

add_executable(Matrix_exp1 Matrix_exp1.cpp)

target_compile_features(Matrix_exp1 PRIVATE cxx_std_11)
target_include_directories(Matrix_exp1 PRIVATE "${PX4_MATRIX_ROOT}")
set_target_properties(Matrix_exp1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if(MSVC)
	target_compile_options(Matrix_exp1 PRIVATE /W4)
else()
	target_compile_options(Matrix_exp1 PRIVATE -Wall -Wextra -Wpedantic)
endif()
