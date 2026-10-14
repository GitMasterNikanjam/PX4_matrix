# PX4 Matrix guides

This folder contains a separate theory and usage guide for each public header.
The examples use the library's `matrix` namespace and fixed-size types. Include
only the headers an application needs when compile time or flash size matters.

## Core linear algebra

| Header | Guide |
|---|---|
| `Matrix.hpp` | [Matrix fundamentals, arithmetic, and utilities](Matrix_README.md) |
| `Vector.hpp` | [Dense column vectors](Vector_README.md) |
| `Vector2.hpp` | [Two-dimensional vectors](Vector2_README.md) |
| `Vector3.hpp` | [Three-dimensional vectors and cross products](Vector3_README.md) |
| `Vector4.hpp` | [Four-dimensional vectors](Vector4_README.md) |
| `SquareMatrix.hpp` | [Square matrices, inverse, Cholesky, and expm](SquareMatrix_README.md) |
| `Slice.hpp` | [Non-owning matrix views](Slice_README.md) |
| `SparseVector.hpp` | [Compile-time sparse vectors](SparseVector_README.md) |
| `Scalar.hpp` | [Scalar and 1x1 matrix conversions](Scalar_README.md) |

## Rotation representations

| Header | Guide |
|---|---|
| `Quaternion.hpp` | [Hamilton quaternions and rotation operations](Quaternion_README.md) |
| `Dcm.hpp` | [Three-dimensional direction cosine matrices](Dcm_README.md) |
| `Dcm2.hpp` | [Planar direction cosine matrices](Dcm2_README.md) |
| `Euler.hpp` | [3-2-1 intrinsic Euler angles](Euler_README.md) |
| `AxisAngle.hpp` | [Axis-angle rotation vectors](AxisAngle_README.md) |

## Estimation and numerical methods

| Header | Guide |
|---|---|
| `Dual.hpp` | [Forward-mode automatic differentiation](Dual_README.md) |
| `LeastSquaresSolver.hpp` | [Householder QR least squares](LeastSquaresSolver_README.md) |
| `PseudoInverse.hpp` | [Moore-Penrose pseudoinverse](PseudoInverse_README.md) |
| `filter.hpp` | [Kalman measurement correction](filter_README.md) |
| `integration.hpp` | [Fixed-step RK4 integration](Integration_README.md) |
| `helper_functions.hpp` | [Comparison, wrapping, and sign helpers](helper_functions_README.md) |
| `math.hpp` | [Umbrella include and header selection](math_README.md) |

## Theory only

For a code-free overview of the mathematics behind the library, see the
[English guide](../README_concepts.md) or the [Persian guide](../README_concepts_fa.md).
They cover linear algebra, 3D rotations, automatic differentiation, numerical
integration, Kalman estimation, and numerical precision, with external reading
and video resources.

## Suggested learning path

Start with `Matrix` and `Vector`, then read `SquareMatrix` and `Slice` for the
operations used in estimation. For attitude code, read `Quaternion` together
with the representation needed by the application. Read the numerical method
guides before using QR least squares, the pseudoinverse, the Kalman correction,
or RK4 in a control loop.

## STM32 notes

- Prefer `float` unless the application's accuracy requirements call for
  `double` and the target can afford its execution cost.
- Matrix dimensions are fixed at compile time and data is stored inline. Check
  stack use for large matrices and algorithms that create temporary matrices.
- Keep arrays passed to constructors large enough for the complete matrix or
  vector; C++ function parameters do not enforce array bounds.
- Build the examples with the same compiler, floating-point options, and
  standard library configuration as the firmware.
