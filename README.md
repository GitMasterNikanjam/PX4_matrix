# PX4 Matrix Library

A lightweight, header‑only C++ template library for **fixed‑size matrix and vector algebra**, **rotations (quaternions, DCM, Euler angles)**, **automatic differentiation**, **Kalman filtering**, and **numerical integration**.  
Designed for embedded and real‑time systems – no dynamic memory allocation, all dimensions known at compile time.

## Overview

This library is part of the [PX4 Autopilot](https://px4.io/) and provides the core linear algebra infrastructure used in attitude estimation, control, navigation, and many other onboard algorithms. It is written in modern C++11 with a strong emphasis on:

- **Performance** – inlined operations, stack allocation.
- **Type safety** – dimensions are checked at compile time.
- **Real‑time suitability** – no exceptions, no dynamic memory.
- **Readability** – intuitive syntax similar to MATLAB or Eigen.

## Repository Structure

The library consists of the following headers (included as needed):

| File | Description |
|------|-------------|
| `Matrix.hpp` | Base matrix class (M×N, row‑major) with arithmetic, slicing, printing. |
| `Vector.hpp` | Column vector specialization (N×1). |
| `Vector2.hpp`, `Vector3.hpp`, `Vector4.hpp` | Convenience aliases and 2D/3D/4D specific methods (cross, xy, hat, etc.). |
| `SquareMatrix.hpp` | Square matrix utilities: inversion, Cholesky, trace, covariance manipulation. |
| `Slice.hpp` | Zero‑copy submatrix views. |
| `helper_functions.hpp` | Floating‑point equality, angle wrapping, sign. |
| `Quaternion.hpp` | Hamilton quaternion rotations. |
| `Dcm.hpp`, `Dcm2.hpp` | Direction cosine matrices (3D and 2D). |
| `Euler.hpp` | 3‑2‑1 intrinsic Euler angles. |
| `AxisAngle.hpp` | Axis‑angle rotation representation. |
| `Dual.hpp` | Dual numbers for automatic differentiation. |
| `PseudoInverse.hpp` | Moore‑Penrose pseudoinverse via Cholesky. |
| `LeastSquaresSolver.hpp` | Linear least squares using QR decomposition. |
| `integration.hpp` | Runge‑Kutta 4 (RK4) integrator. |
| `filter.hpp` | Kalman filter correction step. |
| `Scalar.hpp` | Wrapper for scalar <-> 1×1 matrix conversion. |
| `SparseVector.hpp` | Compile‑time sparse vector. |

## Quick Start

Include the desired headers and use the `matrix` namespace.

```cpp
#include "Matrix.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // 3x3 identity matrix
    Matrix<float, 3, 3> I = eye<float, 3>();
    
    // 3D vector
    Vector3f v(1, 0, 0);
    
    // 90° rotation around Z axis as quaternion
    Quaternionf q(0.7071f, 0, 0, 0.7071f);  // cos(45°), 0,0, sin(45°)
    
    // Rotate vector
    Vector3f rotated = q.rotateVector(v);
    std::cout << rotated << std::endl;
    
    // DCM from quaternion
    Dcmf R(q);
    
    // Euler angles from DCM
    Eulerf euler(R);
    std::cout << "roll: " << euler.phi() << ", pitch: " << euler.theta() << ", yaw: " << euler.psi() << std::endl;
}
```

## Core Features

### 1. Matrix and Vector Algebra

- **Fixed‑size matrices** `Matrix<Type, M, N>` (zero overhead).
- **Arithmetic**: `+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`, unary minus.
- **Matrix multiplication** with compile‑time dimension checking.
- **Element‑wise operations**: `emult()`, `edivide()`.
- **Slicing**: `slice<P,Q>(x,y)` returns a view without copying.
- **Transpose**: `T()`.
- **Norm, min, max, diagonal extraction**.

```cpp
Matrix<float, 2, 3> A;
Matrix<float, 3, 4> B;
auto C = A * B;            // 2x4 result
float norm = C.slice<2,4>(0,0).norm();
```

### 2. Square Matrix Utilities

- **Inversion** – `inv()` for 1x1, 2x2, 3x3, and general LU with partial pivoting.
- **Cholesky decomposition** – `cholesky()` and `choleskyInv()` for positive‑definite matrices.
- **Covariance handling** – `uncorrelateCovarianceBlock()`, `makeBlockSymmetric()`, etc.
- **Trace** – `trace()` and partial trace.
- **Matrix exponential** – `expm()` (Taylor series).

### 3. Rotations and Attitude Representations

All use the **right‑hand rule** and **Hamilton quaternion convention** (real part first).

| Class | Description |
|-------|-------------|
| `Quaternion<Type>` | Quaternion rotation, `rotateVector()`, `inversed()`, `expq()` for integrating body rates. |
| `Dcm<Type>` | 3×3 direction cosine matrix, conversions from/to quaternion, Euler, axis‑angle. |
| `Dcm2<Type>` | 2×2 planar rotation matrix. |
| `Euler<Type>` | 3‑2‑1 intrinsic Tait‑Bryan angles (roll, pitch, yaw). |
| `AxisAngle<Type>` | Rotation as `angle * unit_axis`. |

```cpp
Quaternionf q = Quaternionf::expq(Vector3f(0,0,0.1f)); // small rotation
Vector3f v_rot = q.rotateVector(v);
```

### 4. Automatic Differentiation

The `Dual<Scalar, N>` type computes value + derivative vector using dual numbers.

```cpp
Dual<float,2> x(3,0);  // derivative w.r.t x (index 0)
Dual<float,2> y(2,1);  // derivative w.r.t y (index 1)
auto f = x * x + sin(y);
float dfdx = f.derivative(0);
float dfdy = f.derivative(1);
```

### 5. Numerical Utilities

- **`kalman_correct()`** – Kalman filter correction step (innovation, gain, covariance update).
- **`integrate_rk4()`** – 4th order Runge‑Kutta integrator.
- **`geninv()`** – Moore‑Penrose pseudoinverse (full‑rank Cholesky method).
- **`LeastSquaresSolver`** – QR‑based solver for over‑determined systems.
- **Angle wrapping** – `wrap_pi()`, `wrap_2pi()`, `unwrap_pi()`.

## Usage Examples

### Kalman Filter Correction

```cpp
// State dimension 6, measurement dimension 3
Matrix<float,6,6> P;          // prior covariance
Matrix<float,3,6> C;          // measurement matrix
Matrix<float,3,3> R;          // measurement noise
Vector<float,3> r;            // innovation

Vector<float,6> dx;
Matrix<float,6,6> dP;
float beta;
kalman_correct(P, C, R, r, dx, dP, beta);
// x_new = x_prior + dx
// P_new = P_prior + dP
```

### Least Squares

```cpp
Matrix<float, 4, 2> A;        // 4 equations, 2 unknowns
Vector<float,4> b;
LeastSquaresSolver<float,4,2> solver(A);
Vector<float,2> x = solver.solve(b);
```

### Automatic Differentiation Jacobian

```cpp
Matrix<Dual<float,2>, 2, 1> input;
input(0,0) = Dual<float,2>(1.0f, 0);   // x
input(1,0) = Dual<float,2>(2.0f, 1);   // y

Matrix<Dual<float,2>, 2, 1> output;
output(0,0) = input(0,0) * input(1,0); // f1 = x*y
output(1,0) = input(0,0) + input(1,0); // f2 = x+y

auto J = collectDerivatives(output);   // 2x2 Jacobian
```

## Performance Considerations

- All matrices are stored as plain arrays inside the class (no `std::vector`).
- Operations are inlined – compiler can aggressively optimize.
- Suitable for **small to medium sized matrices** (≤ 12×12 typical in control). For larger matrices, consider a BLAS library.
- No virtual functions, no RTTI.

## Integration with PX4

This library is the standard linear algebra backend for PX4. You will find it used in:

- `EKF2` (extended Kalman filter)
- `AttitudeControl` (quaternion attitude control)
- `LandingTargetEstimator`
- `WindEstimator`
- Many other modules.

To use it in your own PX4 module, simply add:

```cmake
DEPENDS
    matrix
```

to your `CMakeLists.txt`.

## Building and Testing

The library is header‑only and has no separate build step.  
Unit tests are located in the `test/` subdirectory of the original repository.  
To run tests (outside PX4), compile with a C++11 compiler and link to the test files.

## License

**BSD 3‑Clause License** – see [LICENSE](https://github.com/PX4/Matrix/blob/master/LICENSE) for details.

## Credits

Developed and maintained by the **PX4 Development Team** with significant contributions from:

- James Goppert
- Julian Kent
- Matthias Grob
- Kamil Ritz
- Julien Lecoeur
- Bart Slinger
- Many others

## Further Reading

- [PX4 Developer Guide – Matrix Library](https://docs.px4.io/main/en/contributions/matrix.html)
- [Original GitHub Repository](https://github.com/PX4/Matrix)

## See Also

- `Eigen` – a more feature‑rich but heavier library.
- `Ceres Solver` – non‑linear least squares with automatic differentiation (inspiration for `Dual`).
```