# LeastSquaresSolver – QR‑Based Linear Least Squares Solver

A header‑only C++ class that solves **linear least squares problems** `A·x = b` using Householder QR decomposition.  
It is designed for over‑determined systems (`M ≥ N`) where a unique least‑squares solution exists.

## Features

- **Householder QR factorisation** – numerically stable, works well for moderate‑sized matrices.
- **Solves `Ax = b` in the least‑squares sense** when `M > N`, or exactly when `M = N`.
- **Access to `Qᵀ·b`** – the `qtb()` method returns the transformed right‑hand side, which can be reused for multiple `b` vectors.
- **Compile‑time dimensions** – matrix and vector sizes are template parameters (no dynamic allocation).
- **No external dependencies** – uses only the matrix library’s `Matrix`, `Vector`, and `isEqualF`.

## Requirements

- C++11 or later.
- Depends on `Vector.hpp` and `Matrix.hpp` (which in turn include other headers).
- Standard headers: `<cmath>` (indirectly).

## Quick Start

```cpp
#include "LeastSquaresSolver.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Over‑determined system: 3 equations, 2 unknowns
    Matrix<float, 3, 2> A;
    A(0,0)=1; A(0,1)=1;
    A(1,0)=1; A(1,1)=2;
    A(2,0)=1; A(2,1)=3;

    Vector<float, 3> b;
    b(0)=2; b(1)=3; b(2)=4;

    // Create solver (performs QR decomposition)
    LeastSquaresSolver<float, 3, 2> solver(A);

    // Solve for x
    Vector<float, 2> x = solver.solve(b);
    std::cout << "Solution: " << x << "\n";
}
```

## API Reference

### Class `LeastSquaresSolver<Type, M, N>`

**Template Parameters**:
- `Type` – floating‑point type (`float`, `double`).
- `M` – number of rows of matrix `A` (must be ≥ `N`).
- `N` – number of columns of matrix `A` (number of unknowns).

#### Constructor

```cpp
LeastSquaresSolver(const Matrix<Type, M, N>& A);
```

- Performs Householder QR decomposition on `A`.
- Stores the upper triangular factor `R` in the upper triangle of `_A` (including diagonal) and the Householder reflectors in the lower triangle.
- Stores the `_tau` coefficients.
- **Precondition:** `M >= N` (enforced by `static_assert`). The matrix must have full column rank; otherwise, the solution may be unreliable.

#### Methods

| Method | Description |
|--------|-------------|
| `Vector<Type, M> qtb(const Vector<Type, M>& b)` | Computes `Qᵀ·b` using the stored Householder reflectors. Useful when solving for multiple right‑hand sides. |
| `Vector<Type, N> solve(const Vector<Type, M>& b)` | Solves `R·x = Qᵀ·b` by back‑substitution. Returns the least‑squares solution `x`. |

## Usage Examples

### Basic least‑squares fitting

Fit a line `y = c0 + c1·x` to points `(0,1), (1,2), (2,2.5), (3,4)`:

```cpp
Matrix<float, 4, 2> A;  // [1, x_i] for each point
A(0,0)=1; A(0,1)=0;
A(1,0)=1; A(1,1)=1;
A(2,0)=1; A(2,1)=2;
A(3,0)=1; A(3,1)=3;

Vector<float, 4> b;     // y_i
b(0)=1; b(1)=2; b(2)=2.5; b(3)=4;

LeastSquaresSolver<float,4,2> solver(A);
Vector<float,2> coeff = solver.solve(b);
// coeff[0] ≈ 0.95, coeff[1] ≈ 0.95   (line: y = 0.95 + 0.95x)
```

### Solve with multiple right‑hand sides (reuse QR factorisation)

```cpp
LeastSquaresSolver<double, 5, 3> solver(A);
Vector<double,5> b1, b2;
// ... fill b1, b2 ...
Vector<double,3> x1 = solver.solve(b1);
Vector<double,3> x2 = solver.solve(b2);
```

### Access `Qᵀ·b` for custom processing

```cpp
auto Qtb = solver.qtb(b);
// Use Qtb for something else (e.g., to compute residual norms)
```

### Check for rank deficiency

The solver does not explicitly check the rank. If `A` is rank‑deficient, the back‑substitution may divide by zero. The code includes a division‑by‑zero safeguard: if `|A(i,i)| < 1e-8`, it returns a zero vector. For robust rank handling, consider using the pseudoinverse (`PseudoInverse.hpp`) or SVD.

## Important Notes

- **Dimensions** – `M` must be ≥ `N`. The class uses a `static_assert` to enforce this at compile time.
- **Full column rank** – The algorithm assumes `rank(A) = N`. If the matrix is rank‑deficient, the solution may be incorrect (the solver returns a zero vector when a near‑zero pivot is encountered).
- **Performance** – The decomposition is performed in the constructor (O(M·N²) flops). Solving for a new `b` is O(N²) for back‑substitution plus O(M·N) for applying `Qᵀ`.
- **Numerical stability** – Householder reflections are more stable than normal equations (`AᵀA x = Aᵀb`), especially for ill‑conditioned problems.
- **No memory allocation** – All data is stored in fixed‑size arrays (template parameters). Stack usage depends on `M` and `N`.
- **Printing** – The `solve` method includes a `printf` statement for debugging (line `printf("i %d\n", static_cast<int>(i));`). This may be undesirable in production; consider removing or guarding with a macro.

## Dependencies

- `Matrix.hpp` – base matrix class.
- `Vector.hpp` – base vector class.
- `helper_functions.hpp` – for `isEqualF` (floating‑point comparison).

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see copyright notice in the file header).

## Authors

- Bart Slinger <bartslinger@gmail.com>
- PX4 Development Team

## See Also

- `PseudoInverse.hpp` – for Moore‑Penrose pseudoinverse based on Cholesky.
- `helper_functions.hpp` – utilities like `isEqualF`.
- `Matrix.hpp` – dense matrix class.
```