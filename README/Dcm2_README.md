# Dcm2 – 2D Direction Cosine Matrix (Rotation Matrix)

A header‑only C++ class representing **2×2 rotation matrices** for planar rotations.  
`Dcm2` inherits from `SquareMatrix<Type, 2>` and provides a simple way to create rotation matrices from an angle, as well as utilities to maintain orthogonality.

This header is part of the PX4 matrix library and is used for 2D attitude transformations, e.g., in planar navigation or as a building block for higher‑dimensional rotations.

## Features

- **2×2 orthogonal matrix** – represents a rotation in the plane.
- **From angle** – constructor that directly takes a rotation angle (radians).
- **Renormalisation** – `renormalize()` corrects small numerical errors to maintain orthogonality.
- **Inherits all `SquareMatrix` operations** – inversion, trace, multiplication, etc.
- **Type aliases** – `Dcm2f` (float) and `Dcm2d` (double).

## Requirements

- C++11 or later.
- Depends on `SquareMatrix.hpp` and `Vector2.hpp`.
- Standard headers: `<cmath>`.

## Quick Start

```cpp
#include "Dcm2.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Create rotation of 45° (π/4 rad)
    Dcm2f R(M_PI / 4.0f);

    // Rotate a 2D vector
    Vector2f v(1.0f, 0.0f);
    Vector2f rotated = R * v;   // ~(0.707, 0.707)

    // Access elements
    float cos_angle = R(0,0);
    float sin_angle = R(1,0);

    std::cout << rotated << "\n";
}
```

## API Reference

### Class `Dcm2<Type>`

Inherits from `SquareMatrix<Type, 2>` (which inherits from `Matrix<Type,2,2`).

**Constructors**

| Constructor | Description |
|-------------|-------------|
| `Dcm2()` | Identity matrix (zero rotation). |
| `explicit Dcm2(const Type data[2][2])` | From 2×2 C array. |
| `explicit Dcm2(const Type data[4])` | From flat row‑major array of 4 elements. |
| `Dcm2(const Matrix<Type,2,2>&)` | Copy from any 2×2 matrix. |
| `Dcm2(const Type angle)` | From rotation angle (radians). The matrix becomes `[[cosθ, -sinθ], [sinθ, cosθ]]`. |

**Methods**

| Method | Description |
|--------|-------------|
| `void renormalize()` | Orthogonalises the matrix (makes rows unit length and orthogonal) to correct numerical drift. |

**Type Aliases**

| Alias | Definition |
|-------|------------|
| `Dcm2f` | `Dcm2<float>` |
| `Dcm2d` | `Dcm2<double>` |

## Usage Examples

### Rotate a vector

```cpp
Dcm2d R(0.5);          // rotation by 0.5 rad
Vector2d v(1,0);
Vector2d result = R * v;
```

### Compose rotations

```cpp
Dcm2f R1(0.1f);
Dcm2f R2(0.2f);
Dcm2f R_total = R2 * R1;   // first R1, then R2
```

### Extract rotation angle

```cpp
Dcm2f R(0.3f);
float cos_theta = R(0,0);
float sin_theta = R(1,0);
float theta = std::atan2(sin_theta, cos_theta);
```

### Invert a rotation

```cpp
Dcm2f R(0.7f);
Dcm2f R_inv = R.T();   // for orthogonal matrices, transpose equals inverse
```

## Mathematical Background

A 2D rotation matrix (right‑hand rule, positive counter‑clockwise) is:

```
R(θ) = [ cosθ  -sinθ ]
       [ sinθ   cosθ ]
```

It rotates a vector by angle `θ` in the counter‑clockwise direction. The inverse rotation is `R(-θ) = Rᵀ`.

The `renormalize()` method corrects small deviations from orthogonality caused by repeated floating‑point operations. It makes each row a unit vector and ensures orthogonality.

## Important Notes

- **Convention** – `Dcm2` premultiplies a column vector: `v_out = R * v_in`. This follows the standard linear algebra convention.
- **Right‑hand rule** – Positive angles rotate counter‑clockwise.
- **Orthogonality** – Arithmetic operations may break orthogonality. Use `renormalize()` periodically.
- **Performance** – All methods are inlined; the class adds little overhead over a raw 2×2 matrix.

## Dependencies

- `SquareMatrix.hpp` – base class.
- `Vector2.hpp` – for row normalisation.
- `Matrix.hpp` – underlying matrix types.
- `<cmath>` – for `std::sin` and `std::cos`.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- Matthias Grob <maetugr@gmail.com>
- PX4 Development Team

## See Also

- `Dcm.hpp` – 3D direction cosine matrix.
- `SquareMatrix.hpp` – generic square matrix operations.
- `Vector2.hpp` – 2‑element column vector.
```