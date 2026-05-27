# Dual – Automatic Differentiation with Dual Numbers

A header‑only C++ template class for **automatic differentiation** using dual numbers.  
It computes both the value and the derivative of a function in a single pass, with no symbolic manipulation or finite‑difference approximations.

This class is inspired by Ceres Solver’s `Jet` type and the techniques described in “Automatic Differentiation, C++ Templates and Photogrammetry” by Dan Piponi.

## Features

- **Dual number arithmetic** – stores a value and a vector of partial derivatives (size `N`).
- **Operator overloading** – supports `+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`, etc.
- **Standard mathematical functions** – `sqrt`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`, `abs`, `ceil`, `floor`, `fmod`, `min`, `max`.
- **Jacobian extraction** – `collectDerivatives()` turns a vector of dual numbers into a matrix of derivatives.
- **Real value extraction** – `collectReals()` strips derivatives from a matrix of duals.
- **Compile‑time derivative dimension** – `N` is a template parameter (static size).
- **Zero‑overhead** – all operations are inlined, suitable for real‑time use.

## Requirements

- C++11 or later.
- Depends on `Scalar.hpp` and `Vector.hpp` from the same library.
- Standard headers: `<cmath>`.

## Quick Start

```cpp
#include "Dual.hpp"
#include <iostream>

using namespace matrix;

// Define a function that returns a dual number
template<size_t N>
Dual<float, N> myFunction(const Dual<float, N>& x) {
    return sin(x) * cos(x);
}

int main() {
    // Evaluate at x = 0.5, derivative with respect to a single variable
    Dual<float, 1> x(0.5f, 0);   // derivative index 0 = d/dx
    auto y = myFunction(x);
    std::cout << "f(0.5) = " << y.value << "\n";
    std::cout << "f'(0.5) = " << y.derivative(0) << "\n";
}
```

## API Reference

### Class `Dual<Scalar, N>`

**Template Parameters**

| Parameter | Description |
|-----------|-------------|
| `Scalar`  | The underlying numeric type (`float`, `double`, etc.). |
| `N`       | Number of independent variables (size of derivative vector). |

**Public Members**

| Member | Type | Description |
|--------|------|-------------|
| `value` | `Scalar` | The real (primal) value. |
| `derivative` | `Vector<Scalar, N>` | Vector of partial derivatives. |

**Constructors**

| Constructor | Description |
|-------------|-------------|
| `Dual()` | Default – value and derivative are zero‑initialised. |
| `explicit Dual(Scalar v, size_t idx = 65535)` | Sets `value = v` and the derivative at `idx` to 1 (if `idx < N`). |
| `Dual(Scalar v, const Vector<Scalar,N>& d)` | Direct initialisation of value and derivative vector. |

**Assignment Operators**

- `operator=(Scalar)` – sets value and zeroes derivative.
- `operator+=`, `-=`, `*=`, `/=` with another `Dual` or a scalar.

**Comparison Operators**

- `operator==` and `operator!=` compare both value and derivative (using `isEqualF` for tolerance).

### Global Functions

#### Arithmetic Operators
All standard binary and unary operators are overloaded, with the correct chain rule applied.

#### Math Functions

| Function | Description |
|----------|-------------|
| `sqrt(a)` | Square root (derivative = `0.5 * a.derivative / sqrt(a.value)`). |
| `sin(a)`, `cos(a)`, `tan(a)` | Trigonometric functions. |
| `asin(a)`, `acos(a)`, `atan(a)` | Inverse trigonometric functions. |
| `atan2(a,b)` | Two‑argument arctangent. |
| `abs(a)` | Absolute value (derivative = sign of value, with special case at zero). |
| `ceil(a)`, `floor(a)` | Ceiling and floor (derivative zero). |
| `fmod(a, mod)` | Floating‑point remainder (derivative unchanged). |
| `min(a,b)`, `max(a,b)` | Returns the dual with the larger value. |

#### Utilities

| Function | Description |
|----------|-------------|
| `IsNan(a)`, `IsFinite(a)`, `IsInf(a)` | Check properties of the real part. |
| `collectDerivatives<Scalar,M,N>(const Matrix<Dual<Scalar,N>,M,1>&)` | Returns an `M×N` matrix where each row contains the derivative vector of the corresponding dual. |
| `collectReals<Scalar,M,N,D>(const Matrix<Dual<Scalar,D>,M,N>&)` | Returns a matrix of the same dimensions containing only the real parts. |
| `operator<<` | Stream output – prints value and derivative vector. |

## Usage Examples

### Single‑variable derivative

```cpp
// Compute derivative of f(x) = x^3 at x = 2
Dual<double, 1> x(2.0, 0);
auto y = x * x * x;   // dual arithmetic
std::cout << "f(2) = " << y.value << std::endl;        // 8
std::cout << "f'(2) = " << y.derivative(0) << std::endl; // 12
```

### Multivariate function (gradient)

```cpp
// f(x,y) = x*sin(y)
constexpr size_t N = 2;
Dual<float, N> x(3.0f, 0);   // derivative w.r.t x (index 0)
Dual<float, N> y(1.5f, 1);   // derivative w.r.t y (index 1)
auto f = x * sin(y);
float fx = f.derivative(0);   // ∂f/∂x = sin(y)
float fy = f.derivative(1);   // ∂f/∂y = x·cos(y)
```

### Jacobian of a vector function

```cpp
// f1(x,y) = x+y, f2(x,y) = x*y
Matrix<Dual<float,2>, 2, 1> input;
input(0,0) = Dual<float,2>(1.0f, 0);   // x
input(1,0) = Dual<float,2>(2.0f, 1);   // y

Matrix<Dual<float,2>, 2, 1> output;
output(0,0) = input(0,0) + input(1,0);          // f1
output(1,0) = input(0,0) * input(1,0);          // f2

auto J = collectDerivatives(output);   // 2x2 Jacobian matrix
```

### Extract real parts from a matrix of duals

```cpp
Matrix<Dual<float,3>, 2, 2> dualMat;
// ... fill dualMat ...
Matrix<float,2,2> realMat = collectReals(dualMat);
```

## Important Notes

- **Derivative index** – When constructing a `Dual` with `(value, idx)`, the derivative vector has a `1` at position `idx`. This allows you to identify independent variables.
- **Chain rule** – All overloaded operators and functions automatically propagate derivatives correctly.
- **Performance** – The derivative vector is of fixed size `N`. For large `N`, the class may be heavy; use only the necessary number of variables.
- **Zero‑dimensional** – If `N == 0`, the class degenerates to a simple scalar (no derivatives).
- **Tolerance** – Equality comparison uses `isEqualF` with a default tolerance of `1e-4`.
- **Special functions** – `atan2` is correctly implemented with the derivative formula for two variables.

## Dependencies

- `Scalar.hpp` – for `Scalar` type alias and `isEqualF`.
- `Vector.hpp` – for the `derivative` vector.
- `Matrix.hpp` – for `collectDerivatives` and `collectReals`.
- `<cmath>` – for all standard math functions.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- Julian Kent <julian@auterion.com>
- PX4 Development Team

## See Also

- `Jet` class in Ceres Solver (inspiration).
- `Vector.hpp` – for the derivative vector type.
- `Matrix.hpp` – for building Jacobians.
```