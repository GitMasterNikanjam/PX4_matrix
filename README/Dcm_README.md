# Dcm – Direction Cosine Matrix (Rotation Matrix)

A header‑only C++ class representing **3×3 rotation matrices** (direction cosine matrices) following the right‑hand rule.  
`Dcm` inherits from `SquareMatrix<Type, 3>` and provides conversions to/from quaternions, Euler angles, axis‑angle, and a `vee()` operator (inverse of `Vector3::hat()`).

This header is part of the PX4 matrix library and is widely used for attitude representation and transformation.

## Features

- **3×3 orthogonal matrix** – represents a rotation between two coordinate frames.
- **Conversions** – from quaternion, Euler angles (3‑2‑1 intrinsic), axis‑angle, and raw arrays.
- **`vee()` operator** – extracts the skew‑symmetric vector such that `hat(a).vee() = a`.
- **Renormalisation** – `renormalize()` corrects small numerical errors to maintain orthogonality.
- **Inherits all `SquareMatrix` operations** – inversion, trace, multiplication, etc.
- **Type aliases** – `Dcmf` (float) and `Dcmd` (double).

## Requirements

- C++11 or later.
- Depends on `SquareMatrix.hpp`, `Vector3.hpp`, `Quaternion.hpp`, `Euler.hpp`, `AxisAngle.hpp`.
- Standard headers: `<cmath>`.

## Quick Start

```cpp
#include "Dcm.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Identity rotation
    Dcmf R;

    // From Euler angles (roll, pitch, yaw) in radians
    Eulerf euler(0.1f, 0.2f, 0.3f);
    Dcmf R_euler(euler);

    // From quaternion
    Quaternionf q(1,0,0,0);
    Dcmf R_q(q);

    // Rotate a vector
    Vector3f v(1,0,0);
    Vector3f rotated = R_euler * v;

    // Extract the associated rotation vector (vee)
    Vector3f vec = R_q.vee();

    std::cout << rotated << "\n";
}
```

## API Reference

### Class `Dcm<Type>`

Inherits from `SquareMatrix<Type, 3>` (which inherits from `Matrix<Type,3,3>`).

**Constructors**

| Constructor | Description |
|-------------|-------------|
| `Dcm()` | Identity matrix. |
| `explicit Dcm(const Type data[3][3])` | From 3×3 C array. |
| `explicit Dcm(const Type data[9])` | From flat row‑major array of 9 elements. |
| `Dcm(const Matrix<Type,3,3>&)` | Copy from any 3×3 matrix. |
| `Dcm(const Quaternion<Type>&)` | From quaternion (converts `q` to DCM). |
| `Dcm(const Euler<Type>&)` | From Euler angles (3‑2‑1 intrinsic). |
| `Dcm(const AxisAngle<Type>&)` | From axis‑angle (converted via quaternion). |

**Methods**

| Method | Description |
|--------|-------------|
| `Vector3<Type> vee() const` | Inverse of `Vector3::hat()`. Given a skew‑symmetric matrix `A`, returns the vector `v` such that `hat(v) = A`. |
| `void renormalize()` | Orthogonalises the matrix (makes rows unit length and orthogonal) to correct numerical drift. |

**Type Aliases**

| Alias | Definition |
|-------|------------|
| `Dcmf` | `Dcm<float>` |
| `Dcmd` | `Dcm<double>` |

## Usage Examples

### Convert between representations

```cpp
// Quaternion → DCM
Quaterniond q(0.5, 0.5, 0.5, 0.5);
Dcmd R(q);

// DCM → Euler angles
Eulerd euler(R);

// DCM → Axis angle
AxisAngle<double> aa(R);
```

### Rotate vectors

```cpp
Dcmf R = ...;          // some rotation
Vector3f v_body(1,0,0);
Vector3f v_world = R * v_body;   // rotate from body to world frame
```

### Extract rotation vector (for small angles)

```cpp
Dcmf R = ...;
Vector3f rot_vec = R.vee();   // for near‑identity matrices, approximates (ϕ,θ,ψ)
```

### Maintain numerical orthogonality

```cpp
Dcmf R;
// after many operations
R.renormalize();   // ensure R is still a proper rotation matrix
```

## Mathematical Background

A direction cosine matrix `R` satisfies `R·Rᵀ = I` and `det(R) = +1`. It rotates a vector from frame A to frame B as:

```
v_B = R_BA · v_A
```

Conversely, the matrix `R_AB = R_BAᵀ` rotates from B to A.

The `vee()` operator extracts the unique vector `v = [vₓ, vᵧ, v₂]` from a skew‑symmetric matrix `A`:

```
        [ 0   -v_z  v_y ]
A =     [ v_z   0   -v_x ]
        [ -v_y  v_x   0  ]
```

This is the inverse of the `hat()` operator defined in `Vector3`.

## Important Notes

- **Convention** – `Dcm` premultiplies a column vector: `v_out = R * v_in`. This matches the standard linear algebra convention.
- **Orthogonality** – Not all 3×3 matrices are valid rotation matrices. The constructors from angles/quaternions guarantee orthogonality, but arithmetic operations may break it. Use `renormalize()` to correct drift.
- **Performance** – All methods are inlined; the class adds little overhead over a raw 3×3 matrix.
- **Singularities** – Euler‑angle conversion has gimbal lock at pitch = ±90°. The `Dcm(Euler)` constructor handles it correctly but the resulting DCM is still valid.

## Dependencies

- `SquareMatrix.hpp` – base class.
- `Vector3.hpp` – for `vee()` return type.
- `Quaternion.hpp`, `Euler.hpp`, `AxisAngle.hpp` – for conversion constructors.
- `Matrix.hpp` – underlying matrix types.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Quaternion.hpp` – quaternion rotations.
- `Euler.hpp` – Euler angles (3‑2‑1 intrinsic).
- `AxisAngle.hpp` – axis‑angle representation.
- `SquareMatrix.hpp` – generic square matrix operations.
```