# PseudoInverse – Moore‑Penrose Pseudoinverse via Full‑Rank Cholesky

A header‑only C++ implementation of the **Moore‑Penrose pseudoinverse** for matrices of any size, using the fast full‑rank Cholesky factorisation algorithm described by Courrieu (2008).  
It avoids Singular Value Decomposition (SVD) and is well suited for control systems, robotics, and real‑time least‑squares problems.

## Features

- **Optimised for tall or wide matrices** – automatically selects the smaller of `G·Gᵀ` or `Gᵀ·G` to minimise work.
- **Numerical rank determination** – uses a tolerance based on machine epsilon and the largest diagonal element.
- **No SVD** – the algorithm relies on Cholesky factorisation, which is often faster for moderate‑sized matrices.
- **Returns success status** – `false` if the rank-sized inversion fails numerically.
- **Header‑only** – no separate compilation; just include and use.

## Requirements

- C++11 or later.
- Depends on `SquareMatrix.hpp` and `Vector.hpp` (part of the same matrix library).
- Standard headers: `<cmath>`, `<float.h>` (indirectly via `SquareMatrix.hpp`).

## Quick Start

```cpp
#include "PseudoInverse.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Example: solve least squares G * x = y
    Matrix<float, 4, 2> G;
    G(0,0)=1; G(0,1)=1;
    G(1,0)=1; G(1,1)=2;
    G(2,0)=1; G(2,1)=3;
    G(3,0)=1; G(3,1)=4;

    Vector<float, 4> y;
    y(0)=2; y(1)=3; y(2)=4; y(3)=5;

    Matrix<float, 2, 4> Gpinv;
    if (geninv(G, Gpinv)) {
        Vector<float, 2> x = Gpinv * y;
        std::cout << "Least squares solution: " << x << "\n";
    } else {
        std::cout << "Matrix is singular\n";
    }
}
```

## Theory

The Moore-Penrose pseudoinverse `G⁺` gives the least-squares solution for a
full-column-rank tall matrix and the minimum-norm solution for a full-row-rank
wide matrix. This implementation forms the smaller Gram matrix (`G Gᵀ` for
`M <= N`, otherwise `Gᵀ G`), factors its numerically independent columns with a
rank-aware Cholesky process, and reconstructs the pseudoinverse. Forming a Gram
matrix squares the condition number, so this method is fast for small, well
scaled systems but is less robust than SVD for ill-conditioned inputs.

## API Reference

### `geninv()` – Compute Pseudoinverse

```cpp
template<typename Type, size_t M, size_t N>
bool geninv(const Matrix<Type, M, N>& G, Matrix<Type, N, M>& res);
```

Computes the Moore‑Penrose pseudoinverse `res = G⁺` (size `N×M`).

| Parameter | Description |
|-----------|-------------|
| `G` | Input matrix of size `M×N` |
| `res` | Output pseudoinverse of size `N×M` |
| **Returns** | `true` on success, including a zero result for a zero matrix; `false` if the rank-sized inversion fails. |

**Algorithm**  
- If `M ≤ N` (wide or square): factorise `A = G·Gᵀ` (size `M×M`)  
- If `M > N` (tall): factorise `A = Gᵀ·G` (size `N×N`)  
- Performs full‑rank Cholesky factorisation, inverts the resulting symmetric positive‑definite matrix, and reconstructs `G⁺`.

### `fullRankCholesky()` – Full‑Rank Cholesky Factorisation

```cpp
template<typename Type, size_t N>
SquareMatrix<Type, N> fullRankCholesky(const SquareMatrix<Type, N>& A, size_t& rank);
```

Computes a lower‑triangular matrix `L` such that `A ≈ L·Lᵀ` up to the numerical rank.

| Parameter | Description |
|-----------|-------------|
| `A` | Symmetric positive‑semidefinite input matrix (`N×N`). |
| `rank` | Output: numerical rank of `A` (number of accepted columns). |
| **Returns** | `L` – a square matrix where only the first `rank` columns are significant (lower triangular). |

The algorithm stops adding columns when the diagonal element falls below:
`tol = N * epsilon * max(diag(A))`.

### `typeEpsilon()` – Machine Epsilon

```cpp
template<typename Type> Type typeEpsilon();
```

Returns machine epsilon for the supported floating-point types: `float`
(`FLT_EPSILON`) and `double` (`DBL_EPSILON`).

## Usage Examples

### Least‑squares fitting

Fit a line `y = a·x + b` to points `(0,1), (1,2), (2,3), (3,4)`:

```cpp
Matrix<float, 4, 2> G;  // each row: [x_i, 1]
G(0,0)=0; G(0,1)=1;
G(1,0)=1; G(1,1)=1;
G(2,0)=2; G(2,1)=1;
G(3,0)=3; G(3,1)=1;

Vector<float,4> y;
y(0)=1; y(1)=2; y(2)=3; y(3)=4;

Matrix<float,2,4> Gpinv;
if (geninv(G, Gpinv)) {
    Vector<float,2> params = Gpinv * y;
    // params should be approx (1, 1) => y = 1·x + 1
}
```

### Under‑determined system (minimum norm solution)

```cpp
Matrix<float, 2, 4> G;   // more columns than rows
// ... fill G ...
Vector<float,2> y;

Matrix<float,4,2> Gpinv;
if (geninv(G, Gpinv)) {
    Vector<float,4> x = Gpinv * y;   // minimum norm solution
}
```

### Check numerical rank

```cpp
SquareMatrix<float, 3> A;
// ... fill A ...
size_t rank;
fullRankCholesky(A, rank);
std::cout << "Numerical rank: " << rank << "\n";
```

## Important Notes

- **Tolerance** – The rank decision uses `tol = N * epsilon * max(diag(A))`. This may be too aggressive for very ill‑conditioned matrices; consider scaling your data beforehand.
- **Type support** – `typeEpsilon` is specialized for `float` and `double`. Other scalar types do not have a specialization.
- **SquareMatrix dependency** – The inversion inside `geninv` calls `inv(A, X, rank)`, which expects a `SquareMatrix`. That function (from `SquareMatrix.hpp`) uses the rank parameter to invert only the principal submatrix of size `rank`.
- **Rank-deficient input** – `fullRankCholesky` reports the accepted numerical rank. `geninv` may produce a pseudoinverse for rank-deficient inputs; it returns `false` only if the rank-sized inversion fails numerically. For example, the zero matrix yields a zero result and a successful status.
- **Performance** – Complexity is roughly `O(M³)` when `M ≤ N` or `O(N³)` otherwise, due to matrix multiplications and inversion. For large matrices, SVD may be more robust, but for small‑to‑medium sizes (≤ 12x12) this method is very fast.

## Dependencies

- `SquareMatrix.hpp` – provides `SquareMatrix`, `inv()`, and matrix multiplication.
- `Vector.hpp` – provides `Vector` (used for diagonal extraction).
- `Matrix.hpp` – base matrix class.
- `<cmath>` – for `std::sqrt`.
- `<float.h>` – for `FLT_EPSILON` (via `SquareMatrix.hpp`).

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- Julien Lecoeur <julien.lecoeur@gmail.com>
- Julian Kent <julian@auterion.com>
- PX4 Development Team

## See Also

- `SquareMatrix.hpp` – matrix inversion and Cholesky decomposition.
- `helper_functions.hpp` – general utilities (not directly used here).
- `Matrix.hpp` – base matrix class.
