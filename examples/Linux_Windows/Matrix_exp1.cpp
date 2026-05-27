/**
 * @file Matrix_exp1.cpp
 * @brief Example program demonstrating the matrix library functionality
 * 
 * This example shows various operations of the matrix library including:
 * - Matrix construction and initialization
 * - Basic arithmetic operations
 * - Matrix multiplication
 * - Slicing and submatrix operations
 * - Utility functions
 * - Element-wise operations
 * - Angle wrapping utilities
 */

/*
# Assuming the matrix library headers are in the current directory or include path
mkdir -p build && g++ -std=c++11 -O2 -o ./build/Matrix_exp1 Matrix_exp1.cpp -lm 

# Run the example
./build/Matrix_exp1
*/

// ########################################################################
// Include Libraries

#include <iostream>
#include <iomanip>

#include "../matrix/Matrix.hpp"

using namespace matrix;

// ######################################################################

// Helper function to print section headers
void printSection(const char* title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

// Helper function to print a matrix with a label
template<typename Type, size_t M, size_t N>
void printMatrix(const char* label, const Matrix<Type, M, N>& mat) {
    std::cout << std::left << std::setw(25) << label << " = ";
    std::cout << mat << std::endl;
}

// Overload for slices (convert to matrix first)
template<typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
void printMatrix(const char* label, const SliceT<MatrixT, Type, P, Q, M, N>& slice) {
    std::cout << std::left << std::setw(25) << label << " = ";
    // Convert slice to matrix for printing
    Matrix<Type, P, Q> temp;
    for(size_t i = 0; i < P; i++) {
        for(size_t j = 0; j < Q; j++) {
            temp(i, j) = slice(i, j);
        }
    }
    std::cout << temp << std::endl;
}

// Helper to print a column vector (Nx1 matrix)
template<typename Type, size_t N>
void printVector(const char* label, const Matrix<Type, N, 1>& vec) {
    std::cout << std::left << std::setw(25) << label << " = [";
    for(size_t i = 0; i < N; i++) {
        std::cout << vec(i, 0);
        if(i < N-1) std::cout << ", ";
    }
    std::cout << "]^T" << std::endl;
}

// Helper to print a row vector (1xN matrix)
template<typename Type, size_t N>
void printRowVector(const char* label, const Matrix<Type, 1, N>& vec) {
    std::cout << std::left << std::setw(25) << label << " = [";
    for(size_t i = 0; i < N; i++) {
        std::cout << vec(0, i);
        if(i < N-1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

// ############################################################################

int main() 
{
    std::cout << std::fixed << std::setprecision(4);
    
    // ========================================================================
    // 1. Matrix Construction and Basic Operations
    // ========================================================================
    printSection("1. Matrix Construction");
    
    // Default constructor (zero-initialized)
    Matrix<float, 3, 3> A;
    printMatrix("Zero matrix (3x3)", A);
    
    // Identity matrix
    Matrix<float, 3, 3> I;
    I.setIdentity();
    printMatrix("Identity matrix", I);
    
    // Matrix from 2D array
    float data[2][3] = {{1, 2, 3}, {4, 5, 6}};
    Matrix<float, 2, 3> B(data);
    printMatrix("Matrix from 2D array", B);
    
    // Matrix from flat array (row-major)
    float flat_data[6] = {7, 8, 9, 10, 11, 12};
    Matrix<float, 2, 3> C(flat_data);
    printMatrix("Matrix from flat array", C);
    
    // Factory functions
    auto Z = zeros<float, 2, 2>();
    printMatrix("zeros<float,2,2>()", Z);
    
    auto O = ones<float, 2, 2>();
    printMatrix("ones<float,2,2>()", O);
    
    // ========================================================================
    // 2. Element Access and Assignment
    // ========================================================================
    printSection("2. Element Access");
    
    Matrix<float, 3, 3> M;
    M.setAll(0);
    
    // Set individual elements
    M(0, 0) = 1.0f;
    M(0, 1) = 2.0f;
    M(1, 1) = 3.0f;
    M(2, 2) = 4.0f;
    printMatrix("After element assignment", M);
    
    // Row operations using a row vector (1xN matrix)
    Matrix<float, 1, 3> row_vec;
    row_vec(0, 0) = 5; 
    row_vec(0, 1) = 6; 
    row_vec(0, 2) = 7;
    
    // Set row using the row vector (need to transpose for setRow)
    Matrix<float, 3, 1> col_for_row;
    col_for_row(0,0) = 5;
    col_for_row(1,0) = 6;
    col_for_row(2,0) = 7;
    M.setRow(1, col_for_row);
    printMatrix("After setRow(1, [5,6,7]^T)", M);
    
    M.setCol(0, 8.0f);
    printMatrix("After setCol(0, 8)", M);
    
    // ========================================================================
    // 3. Matrix Arithmetic
    // ========================================================================
    printSection("3. Matrix Arithmetic");
    
    Matrix<float, 2, 2> X;
    X(0,0) = 1; X(0,1) = 2;
    X(1,0) = 3; X(1,1) = 4;
    printMatrix("Matrix X", X);
    
    Matrix<float, 2, 2> Y;
    Y(0,0) = 5; Y(0,1) = 6;
    Y(1,0) = 7; Y(1,1) = 8;
    printMatrix("Matrix Y", Y);
    
    // Addition and subtraction
    printMatrix("X + Y", X + Y);
    printMatrix("X - Y", X - Y);
    
    // Scalar operations
    printMatrix("X * 2.5", X * 2.5f);
    printMatrix("X / 2.0", X / 2.0f);
    
    // Matrix multiplication
    printMatrix("X * Y", X * Y);
    
    // Element-wise operations
    printMatrix("X .* Y (emult)", X.emult(Y));
    printMatrix("X ./ Y (edivide)", X.edivide(Y));
    
    // Compound assignments
    Matrix<float, 2, 2> Z_copy = X;
    Z_copy += Y;
    printMatrix("X += Y", Z_copy);
    
    Z_copy = X;
    Z_copy *= 2;
    printMatrix("X *= 2", Z_copy);
    
    // ========================================================================
    // 4. Matrix Transpose and Properties
    // ========================================================================
    printSection("4. Matrix Properties");
    
    Matrix<float, 2, 3> R;
    R(0,0) = 1; R(0,1) = 2; R(0,2) = 3;
    R(1,0) = 4; R(1,1) = 5; R(1,2) = 6;
    printMatrix("Original (2x3)", R);
    printMatrix("Transpose (3x2)", R.T());
    
    // Norm calculations using the slice
    auto slice_view = R.slice<2, 3>(0, 0);
    std::cout << std::left << std::setw(25) << "Frobenius norm" << " = " << slice_view.norm() << std::endl;
    std::cout << std::left << std::setw(25) << "Squared norm" << " = " << slice_view.norm_squared() << std::endl;
    
    // Min/max
    std::cout << std::left << std::setw(25) << "Max element" << " = " << R.max() << std::endl;
    std::cout << std::left << std::setw(25) << "Min element" << " = " << R.min() << std::endl;
    
    // Diagonal extraction - manually since Vector is incomplete
    Matrix<float, 3, 3> Square;
    Square(0,0) = 1; Square(0,1) = 2; Square(0,2) = 3;
    Square(1,0) = 4; Square(1,1) = 5; Square(1,2) = 6;
    Square(2,0) = 7; Square(2,1) = 8; Square(2,2) = 9;
    printMatrix("Square matrix", Square);
    
    // Extract diagonal manually
    Matrix<float, 3, 1> diag_vec;
    for(size_t i = 0; i < 3; i++) {
        diag_vec(i, 0) = Square(i, i);
    }
    printVector("Diagonal elements", diag_vec);
    
    // ========================================================================
    // 5. Slicing and Submatrices
    // ========================================================================
    printSection("5. Matrix Slicing");
    
    Matrix<float, 4, 4> Big;
    // Fill with row-major indices
    for(size_t i = 0; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            Big(i, j) = static_cast<float>(i * 4 + j + 1);
        }
    }
    printMatrix("Original 4x4 matrix", Big);
    
    // Extract a 2x2 submatrix from top-left
    auto sub = Big.slice<2, 2>(0, 0);
    printMatrix("Slice 2x2 at (0,0)", sub);
    
    // Extract a row as 1xN matrix
    Matrix<float, 1, 4> row_mat;
    for(size_t j = 0; j < 4; j++) {
        row_mat(0, j) = Big(2, j);
    }
    printRowVector("Row 2", row_mat);
    
    // Extract a column as Mx1 matrix
    Matrix<float, 4, 1> col_mat;
    for(size_t i = 0; i < 4; i++) {
        col_mat(i, 0) = Big(i, 1);
    }
    printVector("Column 1", col_mat);
    
    // Modify through slice
    sub(0, 0) = 99;
    printMatrix("After modifying slice (top-left element)", Big);
    
    // ========================================================================
    // 6. Vector Operations (using Nx1 matrices)
    // ========================================================================
    printSection("6. Vector Operations");
    
    Matrix<float, 3, 1> v1, v2;
    v1(0,0) = 1; v1(1,0) = 2; v1(2,0) = 3;
    v2(0,0) = 4; v2(1,0) = 5; v2(2,0) = 6;
    
    printVector("Vector v1", v1);
    printVector("Vector v2", v2);
    
    // Vector addition/subtraction (as matrices)
    auto v_sum = v1 + v2;
    auto v_diff = v1 - v2;
    printVector("v1 + v2", v_sum);
    printVector("v1 - v2", v_diff);
    
    // Scalar multiplication
    auto v_scaled = v1 * 2.5f;
    printVector("v1 * 2.5", v_scaled);
    
    // Dot product (using element-wise multiplication and sum)
    float dot = 0;
    for(size_t i = 0; i < 3; i++) {
        dot += v1(i,0) * v2(i,0);
    }
    std::cout << std::left << std::setw(25) << "Dot product v1·v2" << " = " << dot << std::endl;
    
    // Cross product for 3D vectors
    Matrix<float, 3, 1> cross;
    cross(0,0) = v1(1,0) * v2(2,0) - v1(2,0) * v2(1,0);
    cross(1,0) = v1(2,0) * v2(0,0) - v1(0,0) * v2(2,0);
    cross(2,0) = v1(0,0) * v2(1,0) - v1(1,0) * v2(0,0);
    printVector("Cross product v1×v2", cross);
    
    // ========================================================================
    // 7. Element-wise Utility Functions
    // ========================================================================
    printSection("7. Element-wise Utilities");
    
    Matrix<float, 2, 2> values;
    values(0,0) = -5; values(0,1) = 0;
    values(1,0) = 3; values(1,1) = -2;
    printMatrix("Original values", values);
    
    printMatrix("abs()", values.abs());
    printMatrix("min with 0", min(values, 0.0f));
    printMatrix("max with 0", max(values, 0.0f));
    
    // Constrain to range [-2, 2]
    printMatrix("constrain to [-2,2]", constrain(values, -2.0f, 2.0f));
    
    // ========================================================================
    // 8. Angle Utilities (from helper_functions)
    // ========================================================================
    printSection("8. Angle Utilities");
    
    // Wrap angles
    float angles[] = {3.2f, -3.2f, 4.5f, -4.5f, 6.28318f, -6.28318f};
    std::cout << "Angle wrapping examples:" << std::endl;
    for(float a : angles) {
        std::cout << "  " << std::setw(8) << a << " rad -> "
                  << "wrap_pi: " << std::setw(8) << wrap_pi(a) << " rad, "
                  << "wrap_2pi: " << std::setw(8) << wrap_2pi(a) << " rad" 
                  << std::endl;
    }
    
    // Unwrap angles
    float last_angle = 0;
    float new_angles[] = {0.1f, 0.2f, 3.0f, 3.1f, -3.0f, -2.9f};
    std::cout << "\nAngle unwrapping examples:" << std::endl;
    for(float na : new_angles) {
        float unwrapped = unwrap_pi(last_angle, na);
        std::cout << "  last=" << std::setw(6) << last_angle 
                  << ", new=" << std::setw(6) << na
                  << " -> unwrapped=" << std::setw(8) << unwrapped 
                  << " rad" << std::endl;
        last_angle = unwrapped;
    }
    
    // ========================================================================
    // 9. Floating Point Comparison
    // ========================================================================
    printSection("9. Floating Point Comparison");
    
    float a = 1.00001f;
    float b = 1.00002f;
    float c = 1.0001f;
    
    std::cout << std::boolalpha;
    std::cout << "isEqualF(" << a << ", " << b << ", 1e-4) = " 
              << isEqualF(a, b, 1e-4f) << std::endl;
    std::cout << "isEqualF(" << a << ", " << c << ", 1e-4) = " 
              << isEqualF(a, c, 1e-4f) << std::endl;
    std::cout << "isEqualF(NAN, NAN) = " << isEqualF(NAN, NAN) << std::endl;
    std::cout << "isEqualF(INFINITY, INFINITY) = " 
              << isEqualF(INFINITY, INFINITY) << std::endl;
    
    // Matrix comparison
    Matrix<float, 2, 2> M1, M2;
    M1(0,0) = 1.0; M1(0,1) = 2.0;
    M1(1,0) = 3.0; M1(1,1) = 4.0;
    M2 = M1;
    std::cout << "Matrix equality: " << (M1 == M2) << std::endl;
    
    // ========================================================================
    // 10. Real-world Example: 2D Rotation Matrix
    // ========================================================================
    printSection("10. Real-world Example: 2D Rotation");
    
    float angle_rad = M_PI / 4.0f; // 45 degrees
    Matrix<float, 2, 2> Rot;
    Rot(0,0) = cos(angle_rad); Rot(0,1) = -sin(angle_rad);
    Rot(1,0) = sin(angle_rad); Rot(1,1) = cos(angle_rad);
    printMatrix("Rotation matrix (45°)", Rot);
    
    Matrix<float, 2, 1> point;
    point(0,0) = 1.0f;
    point(1,0) = 0.0f;
    printVector("Original point", point);
    
    auto rotated = Rot * point;
    printVector("Rotated point", rotated);
    
    // Verify rotation magnitude
    float original_norm = sqrt(point(0,0)*point(0,0) + point(1,0)*point(1,0));
    float rotated_norm = sqrt(rotated(0,0)*rotated(0,0) + rotated(1,0)*rotated(1,0));
    std::cout << std::left << std::setw(25) << "Original magnitude" << " = " << original_norm << std::endl;
    std::cout << std::left << std::setw(25) << "Rotated magnitude" << " = " << rotated_norm << std::endl;
    
    // ========================================================================
    // 11. Matrix Multiplication Examples
    // ========================================================================
    printSection("11. Matrix Multiplication");
    
    Matrix<float, 4, 4> A4, B4;
    // Initialize with some values
    for(size_t i = 0; i < 4; i++) {
        for(size_t j = 0; j < 4; j++) {
            A4(i,j) = static_cast<float>(i + j);
            B4(i,j) = static_cast<float>(i - j);
        }
    }
    
    auto C4 = A4 * B4;
    printMatrix("4x4 matrix product", C4);
    
    // Check if result is all finite
    std::cout << std::left << std::setw(25) << "Result is all finite" << " = " 
              << std::boolalpha << C4.isAllFinite() << std::endl;
    
    // ========================================================================
    // 12. Efficient A * B^T Multiplication
    // ========================================================================
    printSection("12. Efficient A * B^T Multiplication");
    
    Matrix<float, 2, 3> A2x3;
    A2x3(0,0) = 1; A2x3(0,1) = 2; A2x3(0,2) = 3;
    A2x3(1,0) = 4; A2x3(1,1) = 5; A2x3(1,2) = 6;
    
    Matrix<float, 2, 3> B2x3;
    B2x3(0,0) = 7; B2x3(0,1) = 8; B2x3(0,2) = 9;
    B2x3(1,0) = 10; B2x3(1,1) = 11; B2x3(1,2) = 12;
    
    printMatrix("A (2x3)", A2x3);
    printMatrix("B (2x3)", B2x3);
    
    // Compute A * B^T efficiently
    auto result = A2x3.multiplyByTranspose(B2x3);
    printMatrix("A * B^T (2x2)", result);
    
    // Verify with explicit transpose and multiply
    auto verify = A2x3 * B2x3.T();
    printMatrix("Verification: A * B^T", verify);
    
    // ========================================================================
    // 13. Matrix Inversion Example (2x2)
    // ========================================================================
    printSection("13. 2x2 Matrix Example");
    
    Matrix<float, 2, 2> M2x2;
    M2x2(0,0) = 4; M2x2(0,1) = 7;
    M2x2(1,0) = 2; M2x2(1,1) = 6;
    printMatrix("Original 2x2 matrix", M2x2);
    
    // Calculate determinant
    float det = M2x2(0,0) * M2x2(1,1) - M2x2(0,1) * M2x2(1,0);
    std::cout << std::left << std::setw(25) << "Determinant" << " = " << det << std::endl;
    
    if(std::fabs(det) > 1e-6f) {
        // Calculate inverse: [a b; c d]^-1 = (1/det) * [d -b; -c a]
        Matrix<float, 2, 2> inv;
        inv(0,0) = M2x2(1,1) / det;
        inv(0,1) = -M2x2(0,1) / det;
        inv(1,0) = -M2x2(1,0) / det;
        inv(1,1) = M2x2(0,0) / det;
        printMatrix("Inverse (2x2)", inv);
        
        // Verify: M * inv = I
        auto product = M2x2 * inv;
        printMatrix("M * inv (should be I)", product);
    }
    
    // ========================================================================
    // 14. Using Matrix as Vector in 3D Space
    // ========================================================================
    printSection("14. 3D Space Points");
    
    Matrix<float, 3, 1> p1, p2;
    p1(0,0) = 1.0; p1(1,0) = 2.0; p1(2,0) = 3.0;
    p2(0,0) = 4.0; p2(1,0) = 5.0; p2(2,0) = 6.0;
    
    printVector("Point P1", p1);
    printVector("Point P2", p2);
    
    // Distance between points
    Matrix<float, 3, 1> diff = p2 - p1;
    auto diff_slice = diff.slice<3, 1>(0, 0);
    float distance = diff_slice.norm();
    std::cout << std::left << std::setw(25) << "Distance P1 to P2" << " = " << distance << std::endl;
    
    // Midpoint
    Matrix<float, 3, 1> midpoint = (p1 + p2) * 0.5f;
    printVector("Midpoint", midpoint);
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  Example completed successfully!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}