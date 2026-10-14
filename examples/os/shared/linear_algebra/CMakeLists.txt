# Require CMake 3.14 or newer to configure this example.
cmake_minimum_required(VERSION 3.14)

# Declare this example as a C++ project named Matrix_exp1.
project(Matrix_exp1 LANGUAGES CXX)

# Resolve the PX4_matrix source directory from this CMakeLists.txt location.
# CMAKE_CURRENT_LIST_DIR is .../examples/os/shared/linear_algebra; going up four levels reaches PX4_matrix.
get_filename_component(PX4_MATRIX_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../.." ABSOLUTE)

# Build Matrix_exp1.cpp as an executable target named Matrix_exp1.
add_executable(Matrix_exp1 Matrix_exp1.cpp)

# Require the C++11 language standard when compiling this target.
target_compile_features(Matrix_exp1 PRIVATE cxx_std_11)
# Add PX4_matrix as a private include search path for this target.
target_include_directories(Matrix_exp1 PRIVATE "${PX4_MATRIX_ROOT}")
# Place the built executable in the top-level build directory.
set_target_properties(Matrix_exp1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Use the warning option supported by the active compiler.
if(MSVC)
	# Enable the MSVC level-4 warning set.
	target_compile_options(Matrix_exp1 PRIVATE /W4)
else()
	# Enable common warnings and extra/pedantic diagnostics on non-MSVC compilers.
	target_compile_options(Matrix_exp1 PRIVATE -Wall -Wextra -Wpedantic)
endif()
