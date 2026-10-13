add_executable(Matrix_exp1 linear_algebra/Matrix_exp1.cpp)

target_compile_features(Matrix_exp1 PRIVATE cxx_std_11)
target_include_directories(Matrix_exp1 PRIVATE "${PX4_MATRIX_ROOT}")
set_target_properties(Matrix_exp1 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

if(MSVC)
	target_compile_options(Matrix_exp1 PRIVATE /W4)
else()
	target_compile_options(Matrix_exp1 PRIVATE -Wall -Wextra -Wpedantic)
endif()
