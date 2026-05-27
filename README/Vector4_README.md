# Vector4 – 4D Vector Library

A lightweight, header‑only C++ class for **4‑dimensional column vectors**, built on top of the matrix library’s `Vector<Type,4>`.  
`Vector4` adds convenience constructors and ensures that arithmetic operators return `Vector4` instead of the base `Matrix` type.

This header is part of the PX4 matrix library and works with `Vector.hpp`, `Matrix.hpp`, and `Slice.hpp`.

## Features

- **4‑element column vector** – inherits all operations from `Vector<Type,4>` (dot product, norm, normalisation, etc.).
- **Convenient constructors** – from four scalar components, from a C array, and from column/row slices.
- **Return type correctness** – arithmetic operators return `Vector4` instead of the base `Matrix` type.
- **Type alias** – `Vector4f` for `float` components.

## Requirements

- C++11 or later.
- Depends on `Vector.hpp` (which in turn depends on `Matrix.hpp` and `Slice.hpp`).
- Standard headers: `<cmath>` (included via `Vector.hpp`).

## Quick Start

```cpp
#include "Vector4.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Construction
    Vector4f a(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4f b(5.0f, 6.0f, 7.0f, 8.0f);

    // Dot product
    float dot = a * b;               // 1*5 + 2*6 + 3*7 + 4*8 = 70

    // Vector arithmetic
    auto sum = a + b;
    auto diff = a - b;
    auto scaled = a * 2.0f;

    // Norm
    float len = a.norm();

    std::cout << "a + b = " << sum << "\n";
}
```

## API Reference

### Class `Vector4<Type>`

**Inherits**: `public Vector<Type, 4>` (which inherits `Matrix<Type,4,1>`)

**Public Typedefs**:
- `Matrix41 = Matrix<Type,4,1>` – base matrix type.

#### Constructors

| Constructor | Description |
|-------------|-------------|
| `Vector4()` | Zero‑initialised (x1=0, x2=0, x3=0, x4=0). |
| `Vector4(const Matrix41 &other)` | Copy from a 4×1 matrix. |
| `explicit Vector4(const Type data[3])` | **Note:** parameter declared as `[3]` in the original code, but expects 4 elements – treat array as having at least 4 elements. |
| `Vector4(Type x1, Type x2, Type x3, Type x4)` | From four scalar components. |
| `template<size_t P,size_t Q> Vector4(const Slice<Type,4,1,P,Q>&)` | From mutable column slice (4×1). |
| `template<size_t P,size_t Q> Vector4(const Slice<Type,1,4,P,Q>&)` | From mutable row slice (1×4) – converts to column vector. |

All constructors from the base `Vector<Type,4>` are also available (inherited).

#### Vector Operations

| Method / Operator | Return | Description |
|------------------|--------|-------------|
| `operator+(Vector4) const` | `Vector4` | Element‑wise addition. |
| `operator+(Type) const` | `Vector4` | Scalar addition (adds to all four components). |
| `operator-(Vector4) const` | `Vector4` | Element‑wise subtraction. |
| `operator-(Type) const` | `Vector4` | Scalar subtraction. |
| `operator-() const` | `Vector4` | Unary negation. |
| `operator*(Type) const` | `Vector4` | Scalar multiplication. |
| `operator*(Vector4) const` | `Type` | Dot product. |

All other methods from `Vector<Type,4>` (e.g., `norm()`, `normalize()`, `dot()`, `unit()`, `unit_or_zero()`, `longerThan()`, `sqrt()`) are also available.

#### Type Alias

| Alias | Definition |
|-------|------------|
| `Vector4f` | `Vector4<float>` |

## Usage Examples

### Basic arithmetic and dot product

```cpp
Vector4d a(1, 2, 3, 4);
Vector4d b(5, 6, 7, 8);

Vector4d sum = a + b;          // (6,8,10,12)
Vector4d diff = a - b;         // (-4,-4,-4,-4)
Vector4d scaled = a * 2.0;     // (2,4,6,8)
Vector4d neg = -a;             // (-1,-2,-3,-4)

double dot = a * b;            // 70
```

### Norm and normalisation

```cpp
Vector4f v(1.0f, 2.0f, 2.0f, 4.0f);
float len = v.norm();          // sqrt(1+4+4+16)=5
v.normalize();                 // in‑place: (0.2, 0.4, 0.4, 0.8)
auto u = v.unit();             // copy
```

### Constructing from slices

```cpp
Matrix<float, 4, 4> M;
// ... fill M ...
auto col_slice = M.col(2);          // Slice<Type,4,1,4,4>
Vector4f v_col(col_slice);          // column vector

auto row_slice = M.row(1);          // Slice<Type,1,4,4,4>
Vector4f v_row(row_slice);          // converts row to column vector
```

## Important Notes

1. **Column vector** – all operations follow matrix algebra.
2. **Arithmetic operators return `Vector4`** – not the base `Matrix` type, so you can chain operations without manual casting.
3. **Array constructor quirk** – the parameter is declared as `const Type data_[3]` in the original code, but the constructor expects 4 elements. This is likely a typo; treat the array as having at least 4 elements.
4. **Inherited methods** – all methods from `Vector<Type,4>` (and `Matrix<Type,4,1>`) are available, including:
   - `norm()`, `norm_squared()`, `longerThan()`
   - `dot()`, `unit()`, `unit_or_zero()`
   - `T()` (transpose – gives a 1×4 matrix)
   - `isAllFinite()`, `isAllNan()`, etc.
5. **Performance** – All functions are inlined; no dynamic allocation.

## Dependencies

- `Vector.hpp` – provides the base `Vector<Type,4>` class.
- `Matrix.hpp` – provides the underlying matrix operations.
- `Slice.hpp` – for the slice constructors.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see copyright notice in the file header).

## Authors

- Matthias Grob <maetugr@gmail.com>
- PX4 Development Team

## See Also

- `Vector.hpp` – generic column vector class.
- `Vector2.hpp` – 2D vector with scalar cross product.
- `Vector3.hpp` – 3D vector with vector cross product and hat matrix.
- `Matrix.hpp` – full matrix class.
- `helper_functions.hpp` – mathematical utilities (wrapping, comparisons).
```