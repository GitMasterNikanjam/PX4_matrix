# Vector2 – 2D Vector Library

A lightweight, header‑only C++ class for **2‑dimensional column vectors**, built on top of the matrix library’s `Vector<Type,2>`.  
`Vector2` adds 2D‑specific operations like the scalar cross product, convenient constructors from `(x,y)`, and automatic conversion from 3D vectors.

This header is part of the PX4 matrix library and works with `Vector.hpp`, `Matrix.hpp`, and `Slice.hpp`.

## Features

- **2‑element column vector** – inherits all operations from `Vector<Type,2>` (dot product, norm, normalisation, etc.).
- **Scalar cross product** – `cross()` or `%` operator returns the signed area.
- **Convenient constructors** – from `(x,y)`, from C array, from a 3D vector (ignores z).
- **Type aliases** – `Vector2f` (float) and `Vector2d` (double).
- **Return type correctness** – arithmetic operators return `Vector2` instead of the base `Matrix` type.

## Requirements

- C++11 or later.
- Depends on `Vector.hpp` (which in turn depends on `Matrix.hpp` and `Slice.hpp`).
- Standard headers: `<cmath>` (included via `Vector.hpp`).

## Quick Start

```cpp
#include "Vector2.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Construction
    Vector2f a(1.0f, 2.0f);        // from x,y
    Vector2f b(3.0f, 4.0f);

    // Dot product
    float dot = a * b;             // same as a.dot(b)
    std::cout << "dot = " << dot << "\n";

    // Cross product (scalar)
    float cross = a.cross(b);      // or a % b
    std::cout << "cross = " << cross << "\n";

    // Vector operations (return Vector2)
    auto sum = a + b;
    auto scaled = a * 2.0f;

    // Norm
    float len = a.norm();

    std::cout << "a + b = " << sum << "\n";
}
```

## API Reference

### Class `Vector2<Type>`

**Inherits**: `public Vector<Type, 2>` (which inherits `Matrix<Type,2,1>`)

**Public Typedefs**:
- `Matrix21 = Matrix<Type,2,1>` – base matrix type.
- `Vector3 = Vector<Type,3>` – 3D vector type (for conversion).

#### Constructors

| Constructor | Description |
|-------------|-------------|
| `Vector2()` | Zero‑initialised (x=0, y=0). |
| `Vector2(const Matrix21 &other)` | Copy from a 2×1 matrix. |
| `explicit Vector2(const Type data[2])` | From C array `[x, y]`. |
| `Vector2(Type x, Type y)` | From x and y components. |
| `explicit Vector2(const Vector3 &other)` | From 3D vector – uses first two components, ignores z. |

Additionally, all constructors from the base `Vector<Type,2>` are inherited via `using base::base`.

#### Vector Operations

| Method / Operator | Return | Description |
|------------------|--------|-------------|
| `cross(const Matrix21 &b) const` | `Type` | 2D cross product: `a.x*b.y - a.y*b.x`. |
| `operator%(const Matrix21 &b) const` | `Type` | Alternative syntax for `cross()`. |
| `operator*(Vector2 b) const` | `Type` | Dot product. |
| `operator+(Vector2) const` | `Vector2` | Element‑wise addition. |
| `operator+(Type) const` | `Vector2` | Scalar addition (adds to both components). |
| `operator-(Vector2) const` | `Vector2` | Element‑wise subtraction. |
| `operator-(Type) const` | `Vector2` | Scalar subtraction. |
| `operator-() const` | `Vector2` | Unary negation. |
| `operator*(Type) const` | `Vector2` | Scalar multiplication. |

All other methods from `Vector<Type,2>` (e.g., `norm()`, `normalize()`, `dot()`, `unit()`) are also available.

#### Type Aliases

| Alias | Definition |
|-------|------------|
| `Vector2f` | `Vector2<float>` |
| `Vector2d` | `Vector2<double>` |

## Usage Examples

### Basic vector arithmetic

```cpp
Vector2d u(1.0, 2.0);
Vector2d v(3.0, 5.0);

Vector2d w = u + v;          // (4.0, 7.0)
Vector2d w2 = u - v;         // (-2.0, -3.0)
Vector2d scaled = u * 2.0;   // (2.0, 4.0)
Vector2d neg = -u;           // (-1.0, -2.0)
```

### Dot and cross products

```cpp
Vector2f a(1, 0);
Vector2f b(0, 1);

float dot = a * b;           // 0
float cross = a.cross(b);    // 1
float cross_alt = a % b;     // also 1
```

### Geometry: angle between vectors

```cpp
Vector2d p(1, 0);
Vector2d q(1, 1);

double dot = p.dot(q);
double cross = p.cross(q);
double angle = atan2(cross, dot);   // 45° = π/4
```

### Normalisation

```cpp
Vector2f v(3.0f, 4.0f);
v.normalize();                     // in‑place: (0.6, 0.8)
Vector2f u = v.unit();             // copy, also (0.6, 0.8)
```

### Conversion from 3D vector

```cpp
Vector3d v3(1.0, 2.0, 3.0);
Vector2d v2(v3);                   // (1.0, 2.0) – z ignored
```

### Using with matrices and slices

```cpp
Matrix<float, 3, 3> M;
// ... fill M ...
auto col0 = M.col(0);              // Slice<Type,3,1,...>
Vector3f v(col0);                  // convert slice to 3D vector

Vector2f v2(v);                    // from 3D to 2D (first two components)
```

## Important Notes

1. **Vector2 is a column vector** – all operations follow matrix algebra.
2. **Cross product returns a scalar** – it is the signed area (z‑component of the 3D cross product).
3. **Arithmetic operators return `Vector2`** – not the base `Matrix` type, so you can chain operations without manual casting.
4. **Inheritance** – `Vector2` inherits all methods from `Vector<Type,2>` and `Matrix<Type,2,1>`, including:
   - `norm()`, `norm_squared()`, `longerThan()`
   - `dot()`, `unit()`, `unit_or_zero()`
   - `T()` (transpose – gives a 1×2 matrix)
   - `isAllFinite()`, `isAllNan()`, etc.
5. **Performance** – All functions are inlined; no dynamic allocation.
6. **Printing** – Inherits `operator<<` from `Vector`, which prints the vector as a transposed row, e.g. `[1, 2]`.

## Dependencies

- `Vector.hpp` – provides the base `Vector<Type,2>` class.
- `Matrix.hpp` – provides the underlying matrix operations.
- `Slice.hpp` – for slice constructors (indirectly).

## License

Same as the parent matrix library – **BSD 3‑Clause License**.

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Vector.hpp` – generic column vector class.
- `Vector3.hpp` – 3D vector with 3D cross product (returns vector).
- `Matrix.hpp` – full matrix class.
- `helper_functions.hpp` – mathematical utilities (wrapping, comparisons).
```