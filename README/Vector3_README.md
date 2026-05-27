# Vector3 – 3D Vector Library

A lightweight, header‑only C++ class for **3‑dimensional column vectors**, built on top of the matrix library’s `Vector<Type,3>`.  
`Vector3` adds 3D‑specific operations like the vector cross product, the skew‑symmetric “hat” matrix, and a view for the `xy` components.

This header is part of the PX4 matrix library and works with `Vector.hpp`, `Matrix.hpp`, `Slice.hpp`, and `Dcm.hpp` (for the `hat()` matrix).

## Features

- **3‑element column vector** – inherits all operations from `Vector<Type,3>` (dot product, norm, normalisation, etc.).
- **Vector cross product** – `cross()` or `%` operator returns a `Vector3` (true 3D cross product).
- **Skew‑symmetric “hat” matrix** – `hat()` returns a 3×3 skew‑symmetric matrix such that `hat(a) * v = a × v`. Useful for Lie algebra so(3) and linearising cross products.
- **`xy()` view** – provides a slice of the first two components as a `Vector2` view (zero copy).
- **Return type correctness** – arithmetic operators return `Vector3` instead of the base `Matrix` type.
- **Type aliases** – `Vector3f` (float) and `Vector3d` (double).

## Requirements

- C++11 or later.
- Depends on `Vector.hpp` (which in turn depends on `Matrix.hpp` and `Slice.hpp`).
- Forward declaration of `Dcm` – the `hat()` method returns a `Dcm<Type>` (direction cosine matrix). You must include `Dcm.hpp` if you use `hat()`.
- Standard headers: `<cmath>` (included via `Vector.hpp`).

## Quick Start

```cpp
#include "Vector3.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Construction
    Vector3f a(1.0f, 0.0f, 0.0f);
    Vector3f b(0.0f, 1.0f, 0.0f);

    // Dot product
    float dot = a * b;               // 0

    // Cross product
    Vector3f c = a.cross(b);         // (0,0,1)
    Vector3f d = a % b;              // same

    // Norm and normalisation
    float len = a.norm();            // 1
    a.normalize();                   // in‑place (no effect here)
    auto u = a.unit();               // copy

    // Hat matrix
    Vector3f omega(0.1f, 0.2f, 0.3f);
    auto Omega = omega.hat();        // 3x3 skew‑symmetric matrix

    // xy view
    auto xy = c.xy();                // view of first two components (0,0)
    std::cout << "xy = " << xy << "\n";
}
```

## API Reference

### Class `Vector3<Type>`

**Inherits**: `public Vector<Type, 3>` (which inherits `Matrix<Type,3,1>`)

**Public Typedefs**:
- `Matrix31 = Matrix<Type,3,1>` – base matrix type.

#### Constructors

| Constructor | Description |
|-------------|-------------|
| `Vector3()` | Zero‑initialised (x=0, y=0, z=0). |
| `Vector3(const Matrix31 &other)` | Copy from a 3×1 matrix. |
| `explicit Vector3(const Type data[3])` | From C array `[x, y, z]`. |
| `Vector3(Type x, Type y, Type z)` | From x, y, z components. |

All constructors from the base `Vector<Type,3>` are inherited via `using base::base`.

#### Vector Operations

| Method / Operator | Return | Description |
|------------------|--------|-------------|
| `cross(const Matrix31 &b) const` | `Vector3` | 3D cross product: `a × b`. |
| `operator%(const Matrix31 &b) const` | `Vector3` | Alternative syntax for `cross()`. |
| `operator*(Vector3 b) const` | `Type` | Dot product. |
| `operator+(Vector3) const` | `Vector3` | Element‑wise addition. |
| `operator+(Type) const` | `Vector3` | Scalar addition (adds to all three components). |
| `operator-(Vector3) const` | `Vector3` | Element‑wise subtraction. |
| `operator-(Type) const` | `Vector3` | Scalar subtraction. |
| `operator-() const` | `Vector3` | Unary negation. |
| `operator*(Type) const` | `Vector3` | Scalar multiplication. |

All other methods from `Vector<Type,3>` (e.g., `norm()`, `normalize()`, `dot()`, `unit()`, `unit_or_zero()`, `longerThan()`, `sqrt()`) are also available.

#### 3D‑Specific Utilities

| Method | Return | Description |
|--------|--------|-------------|
| `xy() const` | `ConstSlice<Type,2,1,3,1>` | Read‑only view of the first two components (x,y). |
| `xy()` | `Slice<Type,2,1,3,1>` | Mutable view of the first two components. |
| `hat() const` | `Dcm<Type>` | Skew‑symmetric matrix representation (Lie algebra so(3)). |

#### Type Aliases

| Alias | Definition |
|-------|------------|
| `Vector3f` | `Vector3<float>` |
| `Vector3d` | `Vector3<double>` |

## Usage Examples

### Basic arithmetic and cross product

```cpp
Vector3d a(1, 2, 3);
Vector3d b(4, 5, 6);

Vector3d sum = a + b;          // (5,7,9)
Vector3d diff = a - b;         // (-3,-3,-3)
Vector3d scaled = a * 2.0;     // (2,4,6)
Vector3d neg = -a;             // (-1,-2,-3)

double dot = a * b;            // 1*4 + 2*5 + 3*6 = 32
Vector3d cross = a.cross(b);   // (2*6 - 3*5, 3*4 - 1*6, 1*5 - 2*4) = (-3, 6, -3)
```

### Geometry: angle between vectors

```cpp
Vector3d u(1, 0, 0);
Vector3d v(0, 1, 0);

double dot = u.dot(v);
Vector3d cross = u.cross(v);
double angle = atan2(cross.norm(), dot);   // 90° = π/2
```

### Using the hat matrix

The hat matrix satisfies `hat(a) * b = a × b`.

```cpp
Vector3f a(1, 2, 3);
Vector3f b(4, 5, 6);

auto A_hat = a.hat();           // 3x3 skew‑symmetric matrix
Vector3f cross1 = a.cross(b);
Vector3f cross2 = A_hat * b;    // same as cross1

// Useful for time derivatives of rotation matrices:
// dR/dt = R * hat(omega)
```

### Extracting xy components

```cpp
Vector3f v(1.0f, 2.0f, 3.0f);
auto xy_view = v.xy();           // view of (1,2)
xy_view(0) = 99;                 // modifies v(0)
// v is now (99, 2, 3)
```

### Conversion from a 3x1 matrix slice

```cpp
Matrix<float, 3, 3> M;
// ... fill M ...
auto col_slice = M.col(2);       // Slice<Type,3,1,...>
Vector3f v(col_slice);           // convert slice to Vector3
```

## Important Notes

1. **Column vector** – all operations follow matrix algebra.
2. **Cross product returns a vector** – unlike `Vector2` (which returns a scalar).
3. **Arithmetic operators return `Vector3`** – not the base `Matrix` type, so you can chain operations without manual casting.
4. **`hat()` requires `Dcm.hpp`** – if you use `hat()`, you must include `Dcm.hpp` (or `Dcm` must be fully defined before use). The method returns a `Dcm<Type>` matrix.
5. **`xy()` is a view** – it does not copy data; modifications through the view affect the original vector.
6. **Inherited methods** – all methods from `Vector<Type,3>` (and `Matrix<Type,3,1>`) are available, including:
   - `norm()`, `norm_squared()`, `longerThan()`
   - `dot()`, `unit()`, `unit_or_zero()`
   - `T()` (transpose – gives a 1×3 matrix)
   - `isAllFinite()`, `isAllNan()`, etc.
7. **Performance** – All functions are inlined; no dynamic allocation.

## Dependencies

- `Vector.hpp` – provides the base `Vector<Type,3>` class.
- `Matrix.hpp` – provides the underlying matrix operations.
- `Slice.hpp` – for the `xy()` view and slice constructors.
- `Dcm.hpp` – required only if you call `hat()` (forward declaration is enough for the header, but the definition must be included before use).

## License

Same as the parent matrix library – **BSD 3‑Clause License**.

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Vector.hpp` – generic column vector class.
- `Vector2.hpp` – 2D vector with scalar cross product.
- `Dcm.hpp` – direction cosine matrix (rotation matrix) with `vee()` inverse of `hat()`.
- `Matrix.hpp` – full matrix class.
- `helper_functions.hpp` – mathematical utilities (wrapping, comparisons).
```