# Vector Library

A header‑only C++ template library for **column vectors** of fixed size, built on top of the `Matrix` class.  
`Vector<Type, M>` inherits from `Matrix<Type, M, 1>` and adds vector‑specific methods like dot products, norms, normalisation, and convenient element access.

This file is part of the PX4 matrix library and works together with `Matrix.hpp` and `Slice.hpp`.

## Features

- **Column vector** of compile‑time size `M`.
- **Inherits all matrix operations** – addition, scalar multiplication, transpose, etc.
- **Vector‑specific convenience functions**:
  - `dot()` – dot product
  - `norm()` / `norm_squared()` – Euclidean norm
  - `normalize()` – in‑place normalisation
  - `unit()` – return a normalised copy
  - `unit_or_zero()` – safe normalisation with fallback to zero
  - `longerThan()` – efficient norm‑threshold test
  - `sqrt()` – element‑wise square root
- **Constructors** from:
  - C array
  - Base `Matrix<Type, M, 1>`
  - Mutable or const column slices (`M×1`)
  - Mutable or const row slices (`1×M`) – automatically converted to column vector
- **Compact printing** – prints as a transposed row (one line).
- **Stream output** via `operator<<`.

## Requirements

- C++11 or later.
- Depends on `Matrix.hpp` and `Slice.hpp`.
- Standard headers: `<cmath>`, `<cassert>` (included via `Matrix.hpp`).

## Quick Start

```cpp
#include "Vector.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Create vectors
    Vector<float, 3> a;
    a(0) = 1.0f; a(1) = 2.0f; a(2) = 3.0f;

    Vector<float, 3> b;
    b(0) = 4.0f; b(1) = 5.0f; b(2) = 6.0f;

    // Dot product
    float dot = a.dot(b);
    std::cout << "dot = " << dot << "\n";

    // Normalise
    auto unit_a = a.unit();
    std::cout << "unit(a) = " << unit_a << "\n";

    // Vector addition
    auto c = a + b;
    std::cout << "a + b = " << c << "\n";

    // Scalar multiplication
    auto d = a * 2.0f;
    std::cout << "2*a = " << d << "\n";
}
```

## API Reference

### Class `Vector<Type, M>`

**Inherits**: `public Matrix<Type, M, 1>`

**Public Typedefs**:
- `MatrixM1 = Matrix<Type, M, 1>` – base type.

#### Constructors

| Constructor | Description |
|-------------|-------------|
| `Vector()` | Zero‑initialised. |
| `Vector(const MatrixM1 &other)` | Copy from base matrix. |
| `explicit Vector(const Type data[M])` | From C array of length `M`. |
| `template<size_t P,size_t Q> Vector(const Slice<Type,M,1,P,Q>&)` | From mutable column slice. |
| `template<size_t P,size_t Q,size_t DUMMY=1> Vector(const Slice<Type,1,M,P,Q>&)` | From mutable row slice (1×M) – converts to column. |
| `template<size_t P,size_t Q> Vector(const ConstSlice<Type,M,1,P,Q>&)` | From const column slice. |
| `template<size_t P,size_t Q,size_t DUMMY=1> Vector(const ConstSlice<Type,1,M,P,Q>&)` | From const row slice. |

#### Element Access

| Method | Description |
|--------|-------------|
| `const Type& operator()(size_t i) const` | Read‑only access to i‑th element (0‑based). |
| `Type& operator()(size_t i)` | Mutable access. |

#### Vector Operations

| Method | Return | Description |
|--------|--------|-------------|
| `dot(const MatrixM1 &b) const` | `Type` | Dot product `(*this) · b`. |
| `operator*(const MatrixM1 &b) const` | `Type` | Same as `dot()`. |
| `operator*(Type b) const` | `Vector` | Scalar multiplication (returns new vector). |
| `norm() const` | `Type` | Euclidean norm. |
| `norm_squared() const` | `Type` | Squared norm. |
| `length() const` | `Type` | Alias for `norm()`. |
| `normalize()` | `void` | In‑place normalisation (`*this /= norm()`). |
| `unit() const` | `Vector` | Return a normalised copy. |
| `unit_or_zero(Type eps = 1e-5) const` | `Vector` | Normalised or zero if norm ≤ eps. |
| `normalized() const` | `Vector` | Alias for `unit()`. |
| `longerThan(Type testVal) const` | `bool` | `norm() > testVal` (using squared norm). |
| `sqrt() const` | `Vector` | Element‑wise square root. |

#### Utility Methods

| Method | Description |
|--------|-------------|
| `print() const` | Print the vector as a row (transposed). |
| `static size_t size()` | Returns `M` (compile‑time). |

#### Stream Output

```cpp
template<typename OStream, typename Type, size_t M>
OStream& operator<<(OStream& os, const Vector<Type, M>& vector);
```

Prints the vector in a single line (transposed), e.g. `[1, 2, 3]`.

## Examples

### Basic arithmetic and dot product

```cpp
Vector<double, 4> x, y;
// ... fill x and y ...
double dot = x.dot(y);
Vector<double, 4> sum = x + y;
Vector<double, 4> scaled = x * 2.5;
```

### Normalisation

```cpp
Vector<float, 3> v(1.0f, 2.0f, 3.0f);  // using initialisation from array
v.normalize();               // in‑place
auto u = v.unit();           // copy
auto safe = v.unit_or_zero(1e-6f);
```

### Using with slices

```cpp
Matrix<float, 5, 5> M;
// ... fill M ...
auto row_slice = M.row(2);              // Slice<Type,1,5,5,5>
Vector<float, 5> from_row(row_slice);   // constructs column vector from 1x5 slice

auto col_slice = M.col(3);              // Slice<Type,5,1,5,5>
Vector<float, 5> from_col(col_slice);   // constructs column vector from 5x1 slice
```

### Printing

```cpp
Vector<int, 3> v;
v(0) = 10; v(1) = 20; v(2) = 30;
std::cout << v << std::endl;
// Output:
//    10      20      30
```

## Important Notes

1. **Vector is a column vector** – all operations are consistent with matrix algebra.  
   When printed, it is shown as a row for compactness.

2. **Inheritance** – `Vector` inherits all `Matrix` methods, including `T()` (transpose) which would give a `1×M` matrix.

3. **Memory layout** – Same as `Matrix<Type, M, 1>`: contiguous column (no padding).

4. **Performance** – All functions are inlined. No dynamic allocation.

5. **Row‑slice constructor** – Enables convenient creation of a column vector from a matrix row:  
   `Vector<float,5> v = M.row(i);` (requires the dummy template parameter; it works automatically via template argument deduction).

6. **NaN/Inf handling** – The dot product, norm, etc. follow standard floating‑point behaviour. Use `isAllFinite()` from `Matrix` if needed.

## Dependencies

- `Matrix.hpp` – provides the base class and matrix operations.
- `Slice.hpp` – for slice constructors.
- Standard library: `<cmath>` (for `std::sqrt`).

## License

Same as the parent matrix library – **BSD 3‑Clause License**.

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Matrix.hpp` – main matrix class.
- `Slice.hpp` – matrix slice views.
- `helper_functions.hpp` – mathematical utilities (wrapping, comparisons, sign).
```