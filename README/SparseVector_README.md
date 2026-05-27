# SparseVector – Compile‑Time Sparse Vector Library

A header‑only C++ template class for **sparse vectors** where the positions of non‑zero elements are specified at compile time as a template parameter pack.  
`SparseVector` stores only those elements, saving memory and enabling efficient operations when most entries are zero.

This header is part of the PX4 matrix library and works with `Vector.hpp`, `Matrix.hpp`, and `SquareMatrix.hpp`.

## Features

- **Compile‑time index list** – non‑zero indices are given as a template parameter pack (`Idxs...`).
- **Zero overhead** – only the specified elements are stored; all other indices are implicitly zero.
- **Compile‑time checks** – uniqueness of indices, non‑emptiness, and index bounds are verified at compile time.
- **Access methods** –  
  - Compile‑time access: `at<i>()`  
  - Runtime access by compressed index: `atCompressedIndex(i)`
- **Operations** –  
  - Dot product with dense vectors (`dot()`)  
  - Addition to dense vectors (`operator+`)  
  - In‑place scalar addition (`operator+=`)  
  - Norm and squared norm  
  - Threshold test (`longerThan`)
- **Matrix‑vector product** – global `operator*` for multiplying a dense matrix by a sparse vector.
- **Quadratic form** – efficient `xᵀ A x` evaluation using only non‑zero indices (global `quadraticForm`).
- **Type alias** – `SparseVectorf` for `float` elements.

## Requirements

- C++11 or later.
- Depends on `Vector.hpp`, `Matrix.hpp`, `SquareMatrix.hpp`.
- Standard headers: `<cstring>` (for `memcpy`), `<cmath>` (for `sqrt`), `<cassert>`.

## Quick Start

```cpp
#include "SparseVector.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Sparse vector of length 5 with non‑zeros only at indices 0, 2, 4
    SparseVector<float, 5, 0, 2, 4> sv;

    sv.at<0>() = 1.0f;
    sv.at<2>() = 3.0f;
    sv.at<4>() = 5.0f;

    // Dot product with a dense vector
    Vector<float, 5> dense;
    dense(0) = 2; dense(1) = 0; dense(2) = 4; dense(3) = 0; dense(4) = 6;
    float dot = sv.dot(dense);   // 1*2 + 3*4 + 5*6 = 2 + 12 + 30 = 44

    // Add sparse to dense
    auto result = sv + dense;    // dense vector with sparse values added

    // Matrix‑vector product
    Matrix<float, 3, 5> mat;
    // ... fill mat ...
    auto vec3 = mat * sv;        // 3‑dimensional dense result

    // Quadratic form
    SquareMatrix<float, 5> A;
    // ... fill A ...
    float q = quadraticForm(A, sv);
}
```

## API Reference

### Class `SparseVector<Type, M, Idxs...>`

**Template Parameters**:
- `Type` – element type (e.g., `float`, `double`).
- `M` – total length of the vector (including implicit zeros).
- `Idxs` – pack of indices (0‑based) that are stored. Must be unique, strictly increasing order is not required but recommended for clarity.

**Compile‑time constants**:
- `non_zeros()` – returns `sizeof...(Idxs)`.

**Public Methods**:

| Method | Description |
|--------|-------------|
| `SparseVector()` | Default constructor – zero‑initialises stored elements. |
| `SparseVector(const Vector<Type,M>& data)` | Copy only the specified indices from a dense vector. |
| `explicit SparseVector(const Type data[N])` | Copy from a C array of length `N` (order must match the index pack). |
| `size_t non_zeros() const` | Number of stored elements (compile‑time constant). |
| `size_t index(size_t i) const` | Original index of the i‑th stored element (0 ≤ i < N). |
| `template<size_t i> Type at() const` | Read value at compile‑time index `i` (must be in `Idxs`). |
| `template<size_t i> Type& at()` | Reference to value at compile‑time index `i`. |
| `Type atCompressedIndex(size_t i) const` | Read value by compressed index (runtime, asserts `i < N`). |
| `Type& atCompressedIndex(size_t i)` | Reference by compressed index. |
| `void setZero()` | Set all stored elements to zero. |
| `Type dot(const Vector<Type,M>& other) const` | Dot product with dense vector. |
| `Vector<Type,M> operator+(const Vector<Type,M>& other) const` | Add sparse vector to a dense vector (returns new dense vector). |
| `SparseVector& operator+=(Type t)` | Add scalar `t` to every stored element. |
| `Type norm_squared() const` | Sum of squares of stored elements. |
| `Type norm() const` | Euclidean norm (`√norm_squared()`). |
| `bool longerThan(Type testVal) const` | `norm() > testVal` (uses squared norm). |

### Global Operators & Functions

| Function | Description |
|----------|-------------|
| `Vector<Type,Q> operator*(const Matrix<Type,Q,M>&, const SparseVector<Type,M,Idxs...>&)` | Dense matrix × sparse vector (result is dense). |
| `Type quadraticForm(const SquareMatrix<Type,M>&, const SparseVector<Type,M,Idxs...>&)` | Computes `xᵀ A x` efficiently using only non‑zero indices. Complexity O(N²) with N = number of non‑zeros. |

### Type Aliases

| Alias | Definition |
|-------|------------|
| `SparseVectorf<M, Idxs...>` | `SparseVector<float, M, Idxs...>` |

## Usage Examples

### Construction and element access

```cpp
// Sparse vector with non‑zeros at indices 1, 3, 4 (total length 10)
SparseVector<double, 10, 1, 3, 4> sv;

sv.at<1>() = 2.5;
sv.at<3>() = -1.0;
sv.at<4>() = 0.5;

// Error: at<2>() would cause a static_assert (index 2 is not stored)
// sv.at<2>() = 7.0;   // compile error

// Access by compressed index (0 → index 1, 1 → index 3, 2 → index 4)
sv.atCompressedIndex(1) = 2.0;   // changes element at original index 3
```

### Dot product and addition to dense vector

```cpp
Vector<float, 5> dense;
dense(0) = 1; dense(1) = 2; dense(2) = 3; dense(3) = 4; dense(4) = 5;

SparseVector<float, 5, 0, 2, 4> sv;
sv.at<0>() = 10;
sv.at<2>() = 20;
sv.at<4>() = 30;

float dot = sv.dot(dense);      // 10*1 + 20*3 + 30*5 = 10 + 60 + 150 = 220

auto sum = sv + dense;          // dense vector = [11, 2, 23, 4, 35]
```

### Matrix‑vector product

```cpp
Matrix<float, 2, 4> mat;
mat(0,0)=1; mat(0,1)=2; mat(0,2)=3; mat(0,3)=4;
mat(1,0)=5; mat(1,1)=6; mat(1,2)=7; mat(1,3)=8;

SparseVector<float, 4, 1, 3> sv;
sv.at<1>() = 2;
sv.at<3>() = 3;

auto result = mat * sv;   // result is Vector<float,2>
// result(0) = 2*2 + 4*3 = 4+12=16
// result(1) = 6*2 + 8*3 = 12+24=36
```

### Quadratic form

```cpp
SquareMatrix<float, 4> A;
A(0,0)=1; A(0,1)=2; A(0,2)=3; A(0,3)=4;
A(1,0)=5; A(1,1)=6; A(1,2)=7; A(1,3)=8;
A(2,0)=9; A(2,1)=10;A(2,2)=11;A(2,3)=12;
A(3,0)=13;A(3,1)=14;A(3,2)=15;A(3,3)=16;

SparseVector<float, 4, 0, 2> x;
x.at<0>() = 1;
x.at<2>() = 2;

float q = quadraticForm(A, x);
// Only indices 0 and 2 are used:
// q = x0²·A00 + 2·x0·x2·A02 + x2²·A22
//   = 1²·1 + 2·1·2·3 + 2²·11 = 1 + 12 + 44 = 57
```

### Norm and threshold

```cpp
SparseVector<double, 3, 0, 1> sv;
sv.at<0>() = 3.0;
sv.at<1>() = 4.0;
double n = sv.norm();          // 5.0
bool isLong = sv.longerThan(4.9);  // true
```

## Important Notes

1. **Compile‑time index validation** – The class uses `static_assert` to ensure:
   - No duplicate indices.
   - At least one non‑zero element (`N > 0`).
   - All indices are less than `M`.
   - `N < M` (otherwise a dense vector would be more efficient).

2. **Index order** – The order of the parameter pack `Idxs...` determines the compressed storage order. Access with `atCompressedIndex(i)` uses that order. The `index(i)` method returns the original index for the i‑th stored element.

3. **Performance** – Operations scale with `N` (number of non‑zeros) rather than `M`. This is especially beneficial for large `M` with few non‑zeros.

4. **Memory layout** – The stored values are in a plain C array of size `N`. No heap allocation.

5. **Limitations** – Indices must be known at compile time. For runtime‑sparse vectors, a different data structure would be needed.

6. **Dense fallback** – If `N == M`, the vector is effectively dense; consider using `Vector<Type, M>` directly.

## Dependencies

- `Vector.hpp` – for dense vectors and dot product.
- `Matrix.hpp` – for dense matrices (used in `operator*`).
- `SquareMatrix.hpp` – for the quadratic form.
- Standard headers: `<cstring>`, `<cmath>`, `<cassert>`.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- Kamil Ritz <kritz@ethz.ch>
- Julian Kent <julian@auterion.com>
- PX4 Development Team

## See Also

- `Vector.hpp` – dense vector class.
- `Matrix.hpp` – dense matrix class.
- `SquareMatrix.hpp` – square matrix with inversion, Cholesky, etc.
```