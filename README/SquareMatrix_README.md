# Square Matrix Library

A header‑only C++ template library for **square matrices** of fixed size, built on top of `Matrix<Type, M, M>`.  
`SquareMatrix` adds specialised functionality for square matrices, including matrix inversion, Cholesky decomposition, covariance matrix manipulation, trace extraction, and matrix exponential approximation.

This header is part of the PX4 matrix library and works with `Matrix.hpp`, `Vector.hpp`, and `Slice.hpp`.

## Features

- **Square matrix operations** – inversion (LU with partial pivoting, optimised 1x1/2x2/3x3), trace, diagonal extraction.
- **Cholesky decomposition** – lower‑triangular factor L such that `A = L * L^T` (for positive‑definite matrices).
- **Cholesky inversion** – compute `A⁻¹` via Cholesky (more stable for symmetric positive‑definite matrices).
- **Covariance matrix utilities**:
  - Keep/isolate diagonal blocks (`uncorrelateCovarianceBlock`, `uncorrelateCovariance`)
  - Set block variances (`uncorrelateCovarianceSetVariance`)
  - Enforce symmetry on blocks or whole matrix (`makeBlockSymmetric`, `makeRowColSymmetric`, `copyLowerToUpperTriangle`)
  - Check symmetry (`isBlockSymmetric`, `isRowColSymmetric`)
- **Matrix exponential** – truncated Taylor series approximation (`expm`).
- **Upper‑right triangle packing** – store the upper triangular part (including diagonal) as a vector.
- **Partial trace** – trace of a contiguous diagonal block.
- **Type aliases** – `SquareMatrix2f`, `SquareMatrix3f`, `SquareMatrix3d`, plus backward‑compatible `Matrix2f`, `Matrix3f`, `Matrix3d`.

## Requirements

- C++11 or later.
- Depends on `Matrix.hpp`, `Vector.hpp`, `Slice.hpp`.
- Standard headers: `<float.h>` (for `FLT_EPSILON`), `<cmath>` (indirectly through `Matrix.hpp`).

## Quick Start

```cpp
#include "SquareMatrix.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Create a 3x3 matrix
    SquareMatrix3f A;
    A(0,0) = 4; A(0,1) = 1; A(0,2) = 0;
    A(1,0) = 1; A(1,1) = 3; A(1,2) = 2;
    A(2,0) = 0; A(2,1) = 2; A(2,2) = 5;

    // Inverse
    auto A_inv = A.I();
    std::cout << "Inverse:\n" << A_inv << "\n";

    // Trace
    float tr = A.trace();
    std::cout << "Trace = " << tr << "\n";

    // Cholesky (for symmetric positive‑definite)
    SquareMatrix3f S = A * A.T();   // make positive‑definite
    auto L = cholesky(S);
    std::cout << "Cholesky factor L:\n" << L << "\n";

    // Matrix exponential
    auto expA = expm(A, 5);
}
```

## API Reference

### Class `SquareMatrix<Type, M>`

**Inherits**: `public Matrix<Type, M, M>`

All constructors and assignment operators from `Matrix` are inherited. Additional methods:

| Method | Description |
|--------|-------------|
| `I() const` | Returns the inverse (or zero matrix if singular). |
| `bool I(SquareMatrix& i) const` | Computes inverse into `i`; returns `true` on success. |
| `Vector<Type, M> diag() const` | Returns a column vector of diagonal elements. |
| `Vector<Type, M*(M+1)/2> upper_right_triangle() const` | Packs upper triangle (including diagonal) into a vector (row‑major order). |
| `trace<Width>(size_t first) const` | Trace of a contiguous diagonal block of size `Width` starting at `first`. |
| `trace() const` | Full trace (sum of all diagonal elements). |
| `uncorrelateCovarianceBlock<Width>(size_t first)` | Preserves the `Width×Width` block at `[first,first]` and zeros out the corresponding rows/columns outside it. |
| `uncorrelateCovariance<Width>(size_t first)` | Keeps only the diagonal of the block (sets off‑diagonals inside and outside to zero). |
| `uncorrelateCovarianceSetVariance<Width>(size_t first, const Vector<Type,Width>&)` | Sets the block to a diagonal matrix with given variances. |
| `uncorrelateCovarianceSetVariance<Width>(size_t first, Type val)` | Sets the block to `val * I`. |
| `makeBlockSymmetric<Width>(size_t first)` | Averages off‑diagonals inside the block to enforce symmetry. |
| `makeRowColSymmetric<Width>(size_t first)` | Averages off‑diagonals between the block and the rest of the matrix (makes the whole intersection symmetric). |
| `isBlockSymmetric<Width>(size_t first, Type eps=1e-8)` | Checks symmetry of the block within tolerance. |
| `isRowColSymmetric<Width>(size_t first, Type eps=1e-8)` | Checks symmetry of the rows/columns intersecting the block. |
| `copyLowerToUpperTriangle()` | Copies lower triangle to upper triangle (makes matrix symmetric). |
| `copyUpperToLowerTriangle()` | Copies upper triangle to lower triangle. |

### Global Factory Functions

| Function | Description |
|----------|-------------|
| `eye<Type,M>()` | Returns an identity matrix of size `M×M`. |
| `diag<Type,M>(Vector<Type,M> d)` | Returns a diagonal matrix with the given vector on the diagonal. |
| `expm<Type,M>(const Matrix<Type,M,M>& A, size_t order=5)` | Approximates `exp(A)` using Taylor series up to `order` (excluding identity). |

### Matrix Inversion

Overloaded `inv()` functions:

| Signature | Description |
|-----------|-------------|
| `bool inv(const SquareMatrix<Type,1>&, SquareMatrix<Type,1>&, size_t rank=1)` | 1x1 inversion. |
| `bool inv(const SquareMatrix<Type,2>&, SquareMatrix<Type,2>&)` | Optimised 2x2 inversion using determinant. |
| `bool inv(const SquareMatrix<Type,3>&, SquareMatrix<Type,3>&)` | Optimised 3x3 inversion using adjugate. |
| `bool inv(const SquareMatrix<Type,M>&, SquareMatrix<Type,M>&, size_t rank=M)` | General M×M inversion via LU decomposition with partial pivoting. |
| `SquareMatrix<Type,M> inv(const SquareMatrix<Type,M>&)` | Returns inverse (or zero matrix if singular). |

### Cholesky

| Function | Description |
|----------|-------------|
| `cholesky<Type,M>(const SquareMatrix<Type,M>& A)` | Returns lower‑triangular `L` such that `A = L * L^T`. If `A` is not positive‑definite, the result may contain zeros. |
| `choleskyInv<Type,M>(const SquareMatrix<Type,M>& A)` | Returns `A⁻¹` using Cholesky decomposition (more stable for symmetric positive‑definite matrices). |

### Type Aliases

| Alias | Actual Type |
|-------|-------------|
| `SquareMatrix2f` | `SquareMatrix<float,2>` |
| `SquareMatrix3f` | `SquareMatrix<float,3>` |
| `SquareMatrix3d` | `SquareMatrix<double,3>` |
| `Matrix2f` | `SquareMatrix<float,2>` (backward compatibility) |
| `Matrix3f` | `SquareMatrix<float,3>` |
| `Matrix3d` | `SquareMatrix<double,3>` |

## Usage Examples

### Matrix inversion

```cpp
SquareMatrix3d A;
A(0,0)=1; A(0,1)=2; A(0,2)=3;
A(1,0)=0; A(1,1)=1; A(1,2)=4;
A(2,0)=5; A(2,1)=6; A(2,2)=0;

auto invA = A.I();                // returns inverse
bool ok = A.I(invA);              // same but with output parameter
if (!ok) std::cerr << "Singular matrix!\n";
```

### Cholesky and Cholesky inverse

```cpp
SquareMatrix3f S;
S.setIdentity();
S(0,1) = 0.5; S(1,0) = 0.5;     // make symmetric positive‑definite

auto L = cholesky(S);             // lower triangular
auto Sinv = choleskyInv(S);       // inverse via Cholesky
```

### Covariance matrix operations

```cpp
SquareMatrix<float, 6> cov;
// ... fill with covariance values ...

// Keep only the 3x3 block at rows/cols 2..4 (index 2), zero the rest
cov.uncorrelateCovarianceBlock<3>(2);

// Keep only diagonal of that block
cov.uncorrelateCovariance<3>(2);

// Set the block to diagonal [1,2,3]
Vector<float,3> vars(1,2,3);
cov.uncorrelateCovarianceSetVariance<3>(2, vars);

// Enforce symmetry on the entire matrix
cov.copyLowerToUpperTriangle();
```

### Matrix exponential

```cpp
SquareMatrix2f A;
A(0,0)=0; A(0,1)= -M_PI/4;
A(1,0)= M_PI/4; A(1,1)=0;
auto expA = expm(A, 6);           // approximate rotation matrix
```

## Important Notes

1. **Singular matrices** – `inv()` returns `false` and leaves the output matrix unchanged (or returns a zero matrix when used as value). Always check the return value when using the reference version.
2. **Cholesky** – The implementation does not check for positive definiteness; if the matrix is indefinite, the result may contain zeros or NaNs.
3. **Covariance utilities** – These are designed for square covariance matrices. They zero out selected rows/columns while preserving symmetry assumptions.
4. **Upper‑right triangle packing** – Stores elements in row‑major order, i.e. `(0,0), (0,1), …, (0,M-1), (1,1), (1,2), …`.
5. **Performance** – Inversion and Cholesky are O(M³) algorithms; they are intended for small matrices (M ≤ 10 typically). For larger matrices, consider a dedicated linear algebra library.
6. **Dependencies** – You must include `Vector.hpp` and `Slice.hpp` (they are included via `SquareMatrix.hpp` but may require them to be in the include path).

## Dependencies

- `Matrix.hpp`
- `Vector.hpp`
- `Slice.hpp`
- `<float.h>` (for `FLT_EPSILON`)
- `<cmath>` (indirectly)

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Matrix.hpp` – base matrix class.
- `Vector.hpp` – column vector class.
- `Slice.hpp` – matrix slice views.
- `helper_functions.hpp` – mathematical utilities (comparisons, wrapping).
```