# Scalar – Scalar Wrapper for Matrix Algebra

A lightweight, header‑only C++ class that wraps a single numeric value and provides seamless interoperability between primitive scalars, `Matrix<Type,1,1>`, and `Vector<Type,1>`.  
`Scalar` is useful for writing generic matrix code that must handle both scalars and matrices uniformly.

This header is part of the PX4 matrix library and works with `Matrix.hpp` and `Vector.hpp`.

## Features

- **Immutable value** – a `Scalar` object holds a constant value (no accidental modifications).
- **Implicit conversions** – construct from a primitive type or a 1×1 matrix.
- **Conversion operators** – to primitive type, to `Matrix<Type,1,1>`, and to `Vector<Type,1>`.
- **Equality comparison** – with `float` using tolerance‑based comparison (handles NaN/Inf).
- **Type aliases** – `Scalarf` (float) and `Scalard` (double).

## Requirements

- C++11 or later.
- Depends on `Matrix.hpp` (and indirectly `Vector.hpp`).
- Standard headers: none directly; `Matrix.hpp` includes `<cmath>`, `<cstdio>`, `<cstring>`.

## Quick Start

```cpp
#include "Scalar.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Construction from primitive
    Scalarf s1(3.14f);

    // Construction from 1x1 matrix
    Matrix<float,1,1> m;
    m(0,0) = 2.71f;
    Scalarf s2(m);

    // Implicit conversion to float
    float f = s1;                // f = 3.14

    // Convert to 1x1 matrix
    Matrix<float,1,1> m2 = s1;

    // Convert to 1‑element vector
    Vector<float,1> v = s1;

    // Equality comparison
    if (s1 == 3.14f) {
        std::cout << "Equal\n";
    }
}
```

## API Reference

### Class `Scalar<Type>`

**Template Parameter**:
- `Type` – numeric type (e.g., `float`, `double`, `int`).

**Constructors**:

| Constructor | Description |
|-------------|-------------|
| `Scalar() = delete` | No default constructor – a value must be provided. |
| `Scalar(const Matrix<Type,1,1>& other)` | Construct from a 1×1 matrix. |
| `Scalar(Type other)` | Construct from a primitive value. |

**Conversion Operators**:

| Operator | Description |
|----------|-------------|
| `operator const Type&()` | Implicit conversion to the underlying primitive type (read‑only). |
| `operator Matrix<Type,1,1>() const` | Convert to a 1×1 matrix. |
| `operator Vector<Type,1>() const` | Convert to a 1‑dimensional column vector. |

**Comparison**:

| Method | Description |
|--------|-------------|
| `bool operator==(const float other) const` | Equality with a `float` using tolerance (`isEqualF`). |

**Private Member**:
- `const Type _value` – the wrapped constant value.

### Type Aliases

| Alias | Definition |
|-------|------------|
| `Scalarf` | `Scalar<float>` |
| `Scalard` | `Scalar<double>` |

## Usage Examples

### Generic function that accepts scalar or matrix

```cpp
template<typename T>
void genericMultiply(const T& x, const T& y) {
    auto z = x * y;   // works if T is Scalar, Matrix, or Vector
}
```

### Working with 1×1 matrices and scalars interchangeably

```cpp
Matrix<float,1,1> mat;
mat(0,0) = 2.0f;

Scalarf s = mat;       // implicit construction
float val = s;         // implicit conversion

// Now val == 2.0
```

### Use in expressions with vectors

```cpp
Vector<float,3> v;
v(0)=1; v(1)=2; v(2)=3;

Scalarf scalar(2.0f);
auto scaled = v * scalar;   // scalar converts to Matrix<float,1,1>? Actually operator* for Matrix expects scalar right-hand side.
                            // But Scalar converts to Type via operator const Type&, so multiplication works.
```

### Equality comparison

```cpp
Scalarf s(1.0000001f);
if (s == 1.0f) {            // uses isEqualF with default tolerance (1e-4)
    // This will be true because 0.0000001 < 1e-4
}
```

## Important Notes

1. **Immutability** – `Scalar` holds a `const` value. This prevents accidental modifications that could break expression templates or cause confusion in generic code.

2. **No default constructor** – you must always initialise a `Scalar` with a value. This ensures it is never in an undefined state.

3. **Tolerance‑based equality** – `operator==` uses `isEqualF` from `helper_functions.hpp` (included via `Matrix.hpp`). The default tolerance is `1e-4`. Two NaN values are considered equal; positive infinity equals positive infinity but not negative infinity.

4. **Implicit conversions** – The class provides implicit conversion *to* primitive type, but not *from* primitive type in the reverse direction (except via constructor). This means you can write `float f = s;` but not `Scalar s = 3.0f;`? Actually the constructor `Scalar(Type other)` is not explicit, so `Scalar s = 3.0f;` works. However, the implicit conversion from `Scalar` to `float` is also provided. So both directions are implicit.

5. **Usage in matrix arithmetic** – The conversion operators to `Matrix<Type,1,1>` and `Vector<Type,1>` allow `Scalar` to be used wherever a 1×1 matrix or a vector of length 1 is expected.

## Dependencies

- `Matrix.hpp` – for `Matrix` and `Vector` types, and for `isEqualF`.
- Indirectly includes `helper_functions.hpp` and `Slice.hpp`.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in `Matrix.hpp`).

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Matrix.hpp` – dense matrix class.
- `Vector.hpp` – dense vector class.
- `helper_functions.hpp` – floating point utilities (`isEqualF`).
```