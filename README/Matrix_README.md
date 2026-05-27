# Matrix Library

A lightweight, header-only C++ template library for fixed-size matrix operations with compile-time dimension checking. Designed for embedded systems, robotics, and real-time applications where dynamic memory allocation must be avoided.

## Features

- **Compile‑time dimension checking** – Dimension mismatches are caught at compile time.
- **No dynamic memory allocation** – All matrices are stack‑allocated (no `new`/`delete`).
- **Zero runtime overhead** – Functions are inlined and optimized by the compiler.
- **Type‑safe operations** – Full template type checking.
- **Slice/View support** – Non‑copying submatrix views for efficient operations.
- **Comprehensive operations** – Addition, subtraction, multiplication, element‑wise operations, transpose, norm, min/max, etc.
- **Row‑major storage** – Standard C layout, easy to interface with external code.
- **Floating point utilities** – Tolerance‑based equality, NaN/Inf handling, angle wrapping (via `helper_functions.hpp`).
- **Printable** – Built‑in `print()` and `operator<<` for easy debugging.

## Requirements

- C++11 or later compiler (tested with GCC, Clang, MSVC).
- Standard C++ library headers: `<cmath>`, `<cstdio>`, `<cstring>`.
- The library is header‑only; no separate compilation or linking is required.

## Dependencies

This header relies on two other headers from the same library:

- `helper_functions.hpp` – provides `isEqualF()`, `wrap_pi()`, `unwrap_pi()`, etc.
- `Slice.hpp` – provides `SliceT`, `Slice`, `ConstSlice` for submatrix views.

Make sure all three files are in your include path.

## Installation

Simply copy the headers into your project and include them:

```cpp
#include "Matrix.hpp"
```

No build steps or configuration are needed.

## Quick Start

```cpp
#include <iostream>
#include "Matrix.hpp"

using namespace matrix;

int main() {
    // Create a 2x3 matrix from a 2D array
    float data[2][3] = {{1, 2, 3}, {4, 5, 6}};
    Matrix<float, 2, 3> A(data);

    // Create a 3x2 matrix
    Matrix<float, 3, 2> B;
    B(0,0) = 7; B(0,1) = 8;
    B(1,0) = 9; B(1,1) = 10;
    B(2,0) = 11; B(2,1) = 12;

    // Matrix multiplication (compile‑time checked)
    auto C = A * B;  // results in 2x2 matrix

    // Print using overloaded operator<<
    std::cout << "A * B =\n" << C << std::endl;

    // Identity matrix
    Matrix<float, 3, 3> I;
    I.setIdentity();
    std::cout << "I =\n" << I << std::endl;

    return 0;
}
```

## Usage Examples

### Construction

```cpp
// Default constructor – zero‑initialized
Matrix<double, 4, 4> Z;

// From a flat row‑major array
double flat[] = {1,2,3,4,5,6};
Matrix<double, 2, 3> Mflat(flat);

// From a 2D array
double arr2d[2][3] = {{1,2,3},{4,5,6}};
Matrix<double, 2, 3> M2d(arr2d);

// Copy constructor
auto M2 = M2d;

// Type conversion (int → float)
Matrix<int, 2, 3> Mi(...);
Matrix<float, 2, 3> Mf(Mi);   // static_cast each element
```

### Element Access

```cpp
Matrix<float, 3, 3> M;
M(0,1) = 3.14f;            // write
float x = M(1,2);          // read
```

### Basic Arithmetic

```cpp
Matrix<float, 2, 2> A, B;
auto C = A + B;             // element‑wise addition
auto D = A - B;             // element‑wise subtraction
auto E = A * B;             // matrix multiplication (dimensions must match)
auto F = A * 2.5f;          // scalar multiplication
auto G = A / 2.0f;          // scalar division
auto H = A.emult(B);        // Hadamard (element‑wise) product
auto I = A.edivide(B);      // element‑wise division
```

### Compound Assignments

```cpp
A += B;
A -= B;
A *= 2.0f;
A /= 2.0f;
A *= B;                     // matrix multiplication (may change dimensions)
```

### Transpose and Properties

```cpp
Matrix<float, 2, 3> A;
auto At = A.T();            // or A.transpose(), result is 3x2

float norm = A.slice<2,3>(0,0).norm();   // Frobenius norm
float maxVal = A.max();
float minVal = A.min();
```

### Slicing (Submatrices)

```cpp
Matrix<float, 5, 5> Big;
// Create a view on the top‑left 3x3 block
auto block = Big.slice<3,3>(0,0);
block(0,0) = 99;           // modifies Big

// Extract a row (1xN) or column (Mx1)
auto row2 = Big.row(2);
auto col3 = Big.col(3);
```

### Utility Functions

```cpp
// Fill with zeros / ones / identity
M.setZero();   M.zero();      // same
M.setOne();
M.setIdentity(); M.identity();

// Set all elements to a constant
M.setAll(5.0f);

// Swap rows / columns
M.swapRows(0,1);
M.swapCols(2,3);

// Check for NaN / finite
if (M.isAllNan()) ...
if (M.isAllFinite()) ...
```

### Global Factory Functions

```cpp
auto Z = zeros<float, 3, 4>();    // 3x4 zero matrix
auto O = ones<double, 2, 2>();    // 2x2 matrix of ones
auto N = nans<3,3>();             // 3x3 matrix of NaN (float only)
```

### Element‑wise Minimum / Maximum / Constrain

```cpp
Matrix<float, 2,2> X;
auto Y = min(X, 0.0f);               // element‑wise min with scalar
auto Z = max(X, -1.0f);
auto W = constrain(X, -1.0f, 1.0f);  // clamp each element
```

### Floating Point Comparison

```cpp
if (isEqualF(a, b, 1e-5)) { ... }

Matrix<float, 3,3> A, B;
if (isEqual(A, B, 1e-4)) { ... }   // matrix equality with tolerance
if (A == B) { ... }                // uses isEqual() with default tolerance
```

### Angle Wrapping (from `helper_functions.hpp`)

```cpp
float angle = 3.5f;
float wrapped = wrap_pi(angle);      // -> [-π, π)
float wrapped2pi = wrap_2pi(angle);  // -> [0, 2π)
float continuous = unwrap_pi(last, new);
```

## Important Notes

1. **No dynamic memory** – The matrix size is a template parameter, so very large matrices (e.g., 100×100) will consume significant stack space. For large matrices, consider dynamic allocation or increase stack size. The library is intended for small‑ to medium‑sized matrices (e.g., 3×3, 6×6, 12×12).

2. **Matrix multiplication** – Uses a naive triple loop. For very large matrices, you may want to replace it with a blocked algorithm, but the library focuses on small matrices where simplicity and inlining are more important.

3. **Bounds checking** – `operator()` uses `assert()` in debug builds. In release builds (with `NDEBUG` defined), no bounds checking is performed.

4. **Vector types** – The library forward‑declares a `Vector` template, but it is **not defined** in this header. Use `Matrix<Type, N, 1>` or `Matrix<Type, 1, N>` for column/row vectors instead.

5. **Slicing** – The `slice()` method returns a view that holds a pointer to the original matrix. Ensure the original matrix outlives the slice.

6. **Performance** – All operations are inlined and optimized, making the library suitable for real‑time loops.

## API Reference

### Class `Matrix<Type, M, N>`

**Public Typedefs** – none.

**Constructors**  
- `Matrix()` – zero‑initializes all elements.  
- `explicit Matrix(const Type data[M*N])` – from flat row‑major array.  
- `explicit Matrix(const Type data[M][N])` – from 2D array.  
- `Matrix(const Matrix&)` – copy constructor.  
- `template<typename S> Matrix(const Matrix<S, M, N>&)` – type conversion.  
- `template<size_t P, size_t Q> Matrix(const Slice<...>&)` – from mutable slice.  
- `template<size_t P, size_t Q> Matrix(const ConstSlice<...>&)` – from const slice.

**Element Access**  
- `const Type& operator()(size_t i, size_t j) const`  
- `Type& operator()(size_t i, size_t j)`

**Assignment**  
- `Matrix& operator=(const Matrix&)`  
- `void copyTo(Type dst[M*N]) const` – row‑major copy.  
- `void copyToColumnMajor(Type dst[M*N]) const`

**Arithmetic Operators**  
- `Matrix<Type,M,P> operator*(const Matrix<Type,N,P>&) const` – matrix multiplication.  
- `Matrix<Type,M,M> multiplyByTranspose(const Matrix<Type,P,N>&) const` – efficient `A * B^T`.  
- `Matrix<Type,M,N> emult(const Matrix<Type,M,N>&) const`  
- `Matrix<Type,M,N> edivide(const Matrix<Type,M,N>&) const`  
- `Matrix<Type,M,N> operator+(const Matrix&) const`  
- `Matrix<Type,M,N> operator-(const Matrix&) const`  
- `Matrix<Type,M,N> operator-() const` (unary minus)  
- `Matrix<Type,M,N> operator*(Type) const`  
- `Matrix<Type,M,N> operator/(Type) const`  
- `Matrix<Type,M,N> operator+(Type) const`  
- `Matrix<Type,M,N> operator-(Type) const`  
- `void operator+=(const Matrix&)`  
- `void operator-=(const Matrix&)`  
- `template<size_t P> void operator*=(const Matrix<Type,N,P>&)`  
- `void operator*=(Type)`  
- `void operator/=(Type)`  
- `void operator+=(Type)`  
- `void operator-=(Type)`

**Comparison**  
- `bool operator==(const Matrix&) const`  
- `bool operator!=(const Matrix&) const`

**Utility Functions**  
- `void write_string(char* buf, size_t n) const`  
- `void print(float eps = 1e-9) const`  
- `Matrix<Type,N,M> transpose() const`  
- `Matrix<Type,N,M> T() const`  
- `template<size_t P,size_t Q> ConstSlice<Type,P,Q,M,N> slice(size_t x0,size_t y0) const`  
- `template<size_t P,size_t Q> Slice<Type,P,Q,M,N> slice(size_t x0,size_t y0)`  
- `ConstSlice<Type,1,N,M,N> row(size_t i) const`  
- `Slice<Type,1,N,M,N> row(size_t i)`  
- `ConstSlice<Type,M,1,M,N> col(size_t j) const`  
- `Slice<Type,M,1,M,N> col(size_t j)`  
- `void setRow(size_t i, const Matrix<Type,N,1>&)`  
- `void setRow(size_t i, Type val)`  
- `void setCol(size_t j, const Matrix<Type,M,1>&)`  
- `void setCol(size_t j, Type val)`  
- `void setZero()` / `void zero()`  
- `void setAll(Type val)`  
- `void setOne()`  
- `void setNaN()` (only for floating types)  
- `void setIdentity()` / `void identity()`  
- `void swapRows(size_t a, size_t b)`  
- `void swapCols(size_t a, size_t b)`  
- `Matrix<Type,M,N> abs() const`  
- `Type max() const`  
- `Type min() const`  
- `bool isAllNan() const`  
- `bool isAllFinite() const`

**Global Functions**  
- `zeros<Type,M,N>()`  
- `ones<Type,M,N>()`  
- `nans<M,N>()` – returns `Matrix<float,M,N>` filled with NaN.  
- `operator*(Type, const Matrix&)`  
- `isEqual(const Matrix&, const Matrix&, Type eps)` – element‑wise tolerance comparison.  
- `min(const Matrix&, Type)`, `min(Type, const Matrix&)`, `min(const Matrix&, const Matrix&)`  
- `max(...)` – analogous.  
- `constrain(const Matrix&, Type, Type)` and `constrain(const Matrix&, const Matrix&, const Matrix&)`  
- `operator<<(OStream&, const Matrix&)` – formatted output.

## License

This library is part of the PX4/Firmware matrix library and is distributed under the **BSD 3‑Clause License**. See the copyright notice at the top of the `Matrix.hpp` file.

## Authors

- James Goppert <james.goppert@gmail.com>  
- Julian Kent (for Slice and related improvements)  
- PX4 Development Team
- Mohammad Nikanjam

## Related Headers

- `helper_functions.hpp` – contains `isEqualF`, `wrap_pi`, `unwrap_pi`, etc.  
- `Slice.hpp` – provides the slice/view implementation.

For more advanced linear algebra (e.g., LU decomposition, eigenvalues), consider combining this library with custom algorithms or using a dedicated library like Eigen. This library focuses on small, fixed‑size matrices with zero overhead.
```