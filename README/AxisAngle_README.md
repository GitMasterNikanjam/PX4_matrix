# AxisAngle – Axis‑Angle Rotation Representation

A header‑only C++ class representing a 3D rotation as an **axis‑angle vector** (`θ·u`), where `θ` is the rotation angle (radians) and `u` is a unit vector along the rotation axis.  
The class inherits from `Vector3<Type>` and provides conversions to/from quaternions, direction cosine matrices (DCM), and Euler angles.

This header is part of the PX4 matrix library and is useful for compact rotation representation, interpolation, and small‑angle approximations.

## Features

- **Axis‑angle storage** – vector of length = angle, direction = axis.
- **Conversions** – from quaternion, DCM, Euler angles, or from axis + angle separately.
- **Accessors** – `axis()` returns a unit vector along the rotation axis; `angle()` returns the rotation magnitude (radians).
- **Constructors** – from three scalar components (x,y,z) or from (axis, angle) pair.
- **Type aliases** – `AxisAnglef` (float) and `AxisAngled` (double).

## Requirements

- C++11 or later.
- Depends on `Vector3.hpp`, `Quaternion.hpp`, `Dcm.hpp`, `Euler.hpp`.
- Standard headers: `<cmath>`.

## Quick Start

```cpp
#include "AxisAngle.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Rotation of 90° around Z axis
    AxisAnglef aa(0.0f, 0.0f, M_PI/2.0f);   // vector (0,0,π/2)

    // Convert to quaternion
    Quaternionf q(aa);

    // Convert to DCM
    Dcmf R(aa);

    // Extract axis and angle
    Vector3f axis = aa.axis();
    float angle = aa.angle();

    // Construct from axis + angle
    Vector3f axis_z(0,0,1);
    AxisAnglef aa2(axis_z, M_PI/2.0f);   // same rotation

    std::cout << "Axis: " << axis << ", Angle: " << angle << "\n";
}
```

## API Reference

### Class `AxisAngle<Type>`

Inherits from `Vector3<Type>`.

**Constructors**

| Constructor | Description |
|-------------|-------------|
| `AxisAngle()` | Zero vector (angle = 0, axis arbitrary). |
| `explicit AxisAngle(const Type data[3])` | From 3‑element array (x,y,z) = (θ·u). |
| `AxisAngle(const Matrix<Type,3,1>&)` | Copy from 3×1 matrix. |
| `AxisAngle(const Quaternion<Type>&)` | Convert quaternion to axis‑angle. |
| `AxisAngle(const Dcm<Type>&)` | Convert DCM to axis‑angle (via quaternion). |
| `AxisAngle(const Euler<Type>&)` | Convert Euler angles to axis‑angle (via quaternion). |
| `AxisAngle(Type x, Type y, Type z)` | Direct from components. |
| `AxisAngle(const Matrix<Type,3,1>& axis, Type angle)` | From unit axis vector and angle (radians). |

**Methods**

| Method | Description |
|--------|-------------|
| `Vector3<Type> axis()` | Returns a **unit vector** along the rotation axis. If the angle is zero, returns `(1,0,0)`. |
| `Type angle()` | Returns the rotation angle (norm of the vector) in radians. |

**Type Aliases**

| Alias | Definition |
|-------|------------|
| `AxisAnglef` | `AxisAngle<float>` |
| `AxisAngled` | `AxisAngle<double>` |

## Usage Examples

### Convert from quaternion

```cpp
Quaternionf q = ...;                // some rotation
AxisAnglef aa(q);
float angle = aa.angle();           // magnitude
Vector3f axis = aa.axis();          // direction
```

### Convert from DCM

```cpp
Dcmf R = ...;
AxisAnglef aa(R);
```

### Interpolate rotations (slerp via quaternion)

```cpp
AxisAnglef aa1(axis1, angle1);
AxisAnglef aa2(axis2, angle2);
Quaternionf q1(aa1), q2(aa2);
Quaternionf q_interp = q1.slerp(q2, t);
AxisAnglef aa_interp(q_interp);
```

### Small angle approximation

```cpp
// For small angles, the axis‑angle vector approximates the rotation vector
Vector3f delta = aa;   // direct use as a rotation vector (e.g., for integration)
```

## Mathematical Background

An axis‑angle rotation `(u, θ)` is stored as the vector `v = θ·u`, where:
- `u` is a unit vector (rotation axis)
- `θ` is the rotation angle (right‑hand rule, radians)

This representation is minimal (3 parameters) and singular only at `θ = 0` (where the axis is undefined).  
Conversion to quaternion:

```
q = (cos(θ/2), u·sin(θ/2))
```

Conversion to DCM is performed via quaternion intermediate.

## Important Notes

- **Angle zero** – When the angle is zero, `axis()` returns `(1,0,0)`. This is an arbitrary but consistent choice.
- **Normalisation** – The constructor from `(axis, angle)` normalises the axis to unit length automatically.
- **Performance** – Conversions involve trigonometric functions and square roots. For repeated use, cache the result.
- **Range** – The angle is typically in `[0, π]` (the shortest rotation). For angles larger than `π`, the representation is not unique; the library keeps the input value but normalisation may change it.

## Dependencies

- `Vector3.hpp` – base class and axis accessor.
- `Quaternion.hpp` – for conversion to/from quaternion.
- `Dcm.hpp` – for conversion to/from DCM.
- `Euler.hpp` – for conversion to/from Euler angles.
- `Matrix.hpp` – underlying matrix types.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Quaternion.hpp` – quaternion rotations.
- `Dcm.hpp` – direction cosine matrix.
- `Euler.hpp` – Euler angles (3‑2‑1 intrinsic).
- `Vector3.hpp` – 3‑element vector.
```