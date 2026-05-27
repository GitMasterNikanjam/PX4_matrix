# Matrix Slice / View Library

A header‑only C++ template library for **non‑owning matrix slices (views)**.  
Slices allow you to work with submatrices without copying data, making operations efficient and memory‑friendly.

This file is part of the PX4 matrix library and works together with `Matrix.hpp`.

## Features

- **Zero‑copy submatrix views** – a slice holds only a pointer and offsets.
- **Compile‑time slice dimensions** – `P` (rows) and `Q` (cols) are template parameters.
- **Full arithmetic support** – addition, subtraction, scalar operations, etc.
- **Norm, min, max, diagonal extraction** – same as `Matrix`.
- **Nested slices** – you can slice a slice.
- **Const and non‑const views** – via `Slice` and `ConstSlice` aliases.
- **No dynamic allocation** – all views are stack‑based.

## Requirements

- C++11 or later.
- Headers: `<cassert>`, `<cstdio>`, `<cmath>`.
- Depends on `Matrix.hpp` (forward declaration only, but full definition is needed for operations that create matrices).

## Quick Start

```cpp
#include "Matrix.hpp"
#include "Slice.hpp"

using namespace matrix;

int main() {
    // Create a 4x4 matrix
    Matrix<float, 4, 4> A;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            A(i, j) = i * 4 + j;

    // Create a slice (view) on the top‑left 3x3 block
    auto block = A.slice<3, 3>(0, 0);

    // Modify the original matrix through the slice
    block(0, 0) = 99;   // A(0,0) becomes 99

    // Extract a row slice
    auto row1 = A.row(1);   // Slice<Type,1,N,M,N>

    // Compute norm of the slice
    float n = block.norm();

    // Create a sub‑slice
    auto sub = block.slice<2, 2>(1, 1); // rows 1‑2, cols 1‑2 of the original
}
```

## Core Concepts

### `SliceT<MatrixT, Type, P, Q, M, N>`

The main template class.

- `MatrixT` – type of the parent matrix (`Matrix<Type,M,N>` or `const Matrix<Type,M,N>`).
- `Type` – element type.
- `P`, `Q` – **slice dimensions** (rows, columns).
- `M`, `N` – dimensions of the **backing matrix**.

A `SliceT` object stores:
- `_x0`, `_y0` – starting row/column in the parent.
- `_data` – pointer to the parent matrix.

All operations forward to the parent matrix with added offsets.

### Type Aliases

For convenience, two aliases are provided:

```cpp
template<typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

template<typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<const Matrix<Type, M, N>, Type, P, Q, M, N>;
```

Use `Slice` for mutable views, `ConstSlice` for read‑only views.

## Construction

You normally do not create slices directly. Instead, use the `slice()` method of `Matrix`:

```cpp
Matrix<float, 6, 6> M;
auto my_slice = M.slice<2, 3>(1, 2);   // 2 rows, 3 cols starting at (1,2)
```

You can also create a slice from an existing slice:

```cpp
auto sub = my_slice.slice<1, 2>(0, 1);
```

## Supported Operations

### Element Access

```cpp
const Type& operator()(size_t i, size_t j) const;
Type& operator()(size_t i, size_t j);
```

### Assignment

- Copy from another slice, matrix, scalar, or (for row slices) from a `Vector`.

```cpp
slice = other_slice;
slice = matrix;
slice = scalar;
row_slice = vector;   // when P == 1
```

### Arithmetic (return new `Matrix<P,Q>`)

```cpp
slice + other_slice
slice - matrix
slice * scalar
slice / scalar
// etc.
```

### Compound Assignments (modify the slice in‑place)

```cpp
slice += other_slice;
slice -= scalar;
slice *= scalar;
slice /= scalar;
// etc.
```

### Properties

```cpp
Type norm() const;
Type norm_squared() const;
bool longerThan(Type testVal) const;
Type max() const;
Type min() const;
Vector<Type, min(P,Q)> diag() const;   // returns a Vector (forward declared)
```

### Copy to C Arrays

```cpp
void copyTo(Type dst[P*Q]) const;               // row‑major
void copyToColumnMajor(Type dst[P*Q]) const;    // column‑major
```

### Slicing

```cpp
template<size_t R, size_t S>
SliceT<...> slice(size_t x0, size_t y0);        // mutable

template<size_t R, size_t S>
const SliceT<...> slice(size_t x0, size_t y0) const;   // const
```

## Important Notes

1. **Lifetime** – The slice holds a pointer to the parent matrix. Ensure the parent outlives any slice.
2. **Bounds checking** – Only `assert()` in debug builds. No checks in release.
3. **Vector type** – `Vector` is forward‑declared but not defined in this header; it is intended to be `Matrix<Type, N, 1>`. The `diag()` method returns a `Vector`; you may need to convert it to a column matrix if `Vector` is incomplete.
4. **Performance** – All operations are inlined. Slices have zero runtime overhead compared to direct matrix access.

## Example: Using Slices to Update a Submatrix

```cpp
Matrix<float, 5, 5> A;
A.setZero();

// Create a slice for the bottom‑right 2x2 block
auto block = A.slice<2, 2>(3, 3);
block = 5.0f;                     // sets all four elements to 5

// Copy a matrix into the block
Matrix<float, 2, 2> B;
B.setIdentity();
block = B;

// Now A(3,3)=1, A(3,4)=0, A(4,3)=0, A(4,4)=1
```

## Example: Reading a Row

```cpp
Matrix<double, 4, 4> M;
// ... fill M ...
auto row = M.row(2);   // Slice<double,1,4,4,4>
for (size_t j = 0; j < 4; ++j)
    std::cout << row(0, j) << " ";
```

## Dependencies

- **`Matrix.hpp`** – needed for the `Matrix` type and for operations that create new matrices.
- **Standard headers** – `<cassert>`, `<cstdio>`, `<cmath>`.

## License

Same as the parent matrix library – **BSD 3‑Clause License**.

## Authors

- Julian Kent <julian@auterion.com>
- PX4 Development Team

## See Also

- `Matrix.hpp` – the main matrix class.
- `helper_functions.hpp` – mathematical utilities (wrapping, comparisons, sign).
```