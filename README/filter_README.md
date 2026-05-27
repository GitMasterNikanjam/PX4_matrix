# Kalman Filter Utilities – Correction Step

A header‑only C++ function that performs the **correction (measurement update) step** of a linear Kalman filter. Given prior state covariance, measurement matrix, measurement noise covariance, and innovation residual, it computes the state correction, covariance update, and normalised innovation squared (Mahalanobis distance).

This function is part of the PX4 matrix library and is designed for real‑time state estimation.

## Features

- **Standard Kalman correction formulas** – computes Kalman gain, state correction, covariance update, and innovation test statistic.
- **Compile‑time dimensions** – state dimension `M` and measurement dimension `N` are template parameters (no heap allocation).
- **Returns beta** – the normalised innovation squared (`χ²`‑distributed) for outlier detection or consistency checking.
- **Efficient** – uses the matrix library’s fast inversion (`SquareMatrix::I()`).

## Requirements

- C++11 or later.
- Depends on `Scalar.hpp` and `SquareMatrix.hpp` (and indirectly on `Matrix.hpp`, `Vector.hpp`).
- Assumes all matrices are properly sized and invertible.

## Quick Start

```cpp
#include "filter.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // 2‑state system, 1‑dimensional measurement
    constexpr size_t M = 2;   // state dimension
    constexpr size_t N = 1;   // measurement dimension

    // Prior state covariance
    SquareMatrix<float, M> P;
    P(0,0)=1; P(0,1)=0;
    P(1,0)=0; P(1,1)=1;

    // Measurement matrix
    Matrix<float, N, M> C;
    C(0,0)=1; C(0,1)=0;

    // Measurement noise covariance
    SquareMatrix<float, N> R;
    R(0,0)=0.1f;

    // Innovation residual (measurement minus expected)
    Vector<float, N> r;
    r(0)=2.0f;

    Vector<float, M> dx;
    SquareMatrix<float, M> dP;
    float beta;

    kalman_correct(P, C, R, r, dx, dP, beta);

    std::cout << "State correction: " << dx << "\n";
    std::cout << "Beta (χ² test): " << beta << "\n";
}
```

## API Reference

### Function `kalman_correct`

```cpp
template<typename Type, size_t M, size_t N>
int kalman_correct(
    const Matrix<Type, M, M>& P,
    const Matrix<Type, N, M>& C,
    const Matrix<Type, N, N>& R,
    const Matrix<Type, N, 1>& r,
    Matrix<Type, M, 1>& dx,
    Matrix<Type, M, M>& dP,
    Type& beta
);
```

**Parameters**:

| Parameter | Description |
|-----------|-------------|
| `P` | Prior state covariance matrix (`M×M`). |
| `C` | Measurement matrix (`N×M`). |
| `R` | Measurement noise covariance matrix (`N×N`). |
| `r` | Innovation residual (`N×1`): `r = z - h(x)` where `z` is actual measurement, `h(x)` is expected measurement. |
| `dx` | Output: state correction (`M×1`). After update, `x_new = x_prior + dx`. |
| `dP` | Output: covariance change (`M×M`). After update, `P_new = P_prior + dP`. |
| `beta` | Output: normalised innovation squared = `rᵀ·S⁻¹·r`. Under Gaussian assumptions, `beta` is `χ²(N)`‑distributed. |

**Returns**:
- Always `0` (success). If the innovation covariance `S` is singular, the result may contain `NaN`.

**Mathematical definition**:
```
S = C·P·Cᵀ + R
K = P·Cᵀ·S⁻¹
dx = K·r
beta = rᵀ·S⁻¹·r
dP = -K·C·P
```

## Usage Examples

### Simple 1‑D position estimate

```cpp
// State: position and velocity
SquareMatrix<float, 2> P = eye<float,2>();
P(0,0)=10; P(1,1)=1;   // high position uncertainty

// Measure only position
Matrix<float,1,2> C; C(0,0)=1; C(0,1)=0;
SquareMatrix<float,1> R; R(0,0)=1.0f;
Vector<float,1> r; r(0)=5.0f;   // measured position = 5

Vector<float,2> dx;
SquareMatrix<float,2> dP;
float beta;
kalman_correct(P, C, R, r, dx, dP, beta);
// dx gives the correction to position and velocity
```

### Outlier detection with beta

```cpp
float chi2_99 = ...; // 99% quantile of χ²(N) distribution
if (beta > chi2_99) {
    // reject measurement or increase R
}
```

## Important Notes

1. **Invertibility** – The function computes `S_I = S⁻¹` using `SquareMatrix<I>()`. If `S` is not invertible, the result will contain `NaN`. The caller must ensure that `R` is positive definite and `C·P·Cᵀ + R` is invertible.

2. **Covariance update** – The function returns `dP` such that `P_new = P + dP`. This is equivalent to the Joseph form `P_new = (I - K·C) · P`.

3. **No state or time propagation** – This file only contains the correction step. The prediction step (time update) must be implemented elsewhere.

4. **No numerical safeguards** – No checks for positive definiteness or singular matrices are performed. Use with caution in production, or wrap with additional checks.

5. **Return value** – The function always returns `0`. Error handling must be done by checking `isfinite(beta)` or the validity of `dx`.

## Dependencies

- `Scalar.hpp` – for converting the quadratic form `rᵀ·S⁻¹·r` to a scalar.
- `SquareMatrix.hpp` – for matrix inversion and multiplication.
- `Matrix.hpp`, `Vector.hpp` – for basic matrix/vector types.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- PX4 Development Team

## See Also

- `SquareMatrix.hpp` – for `inv()` and matrix operations.
- `Scalar.hpp` – for scalar‑matrix conversions.
- `PseudoInverse.hpp` – for pseudo‑inverse in case of singular `S`.
```