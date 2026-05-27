# Euler – 3‑2‑1 Intrinsic Tait‑Bryan Angles

A header‑only C++ class representing **Euler angles** (roll‑pitch‑yaw) following the aerospace convention: a 3‑2‑1 intrinsic Tait‑Bryan rotation sequence.  
The class inherits from `Vector<Type, 3>` and provides convenient accessors for the three angles.

This header is part of the PX4 matrix library and works with `Dcm.hpp`, `Quaternion.hpp`, and `Vector.hpp`.

## Features

- **3‑2‑1 intrinsic sequence** – Rotations are applied in the order:  
  1. Yaw (`ψ`) about the original Z axis,  
  2. Pitch (`θ`) about the new Y' axis,  
  3. Roll (`ϕ`) about the new X'' axis.  
  This is the standard aircraft convention (yaw‑pitch‑roll).
- **Conversion from DCM** – constructs Euler angles from a direction cosine matrix (rotation matrix).
- **Conversion from Quaternion** – constructs Euler angles from a Hamilton quaternion.
- **Named accessors** – `.phi()`, `.theta()`, `.psi()` for both reading and writing.
- **Type aliases** – `Eulerf` (float) and `Eulerd` (double).

## Requirements

- C++11 or later.
- Depends on `Vector.hpp`, `Dcm.hpp`, `Quaternion.hpp`.
- Standard headers: `<cmath>` (indirectly).

## Quick Start

```cpp
#include "Euler.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Create from angles (roll, pitch, yaw) in radians
    Eulerf euler(0.1f, 0.2f, 0.3f);

    // Access individual angles
    float roll  = euler.phi();
    float pitch = euler.theta();
    float yaw   = euler.psi();

    // Modify
    euler.phi() = 0.2f;

    // Convert to DCM
    Dcm<float> R(euler);

    // Convert to Quaternion
    Quaternionf q(euler);

    std::cout << "Roll: " << roll << ", Pitch: " << pitch << ", Yaw: " << yaw << "\n";
}
```

## API Reference

### Class `Euler<Type>`

Inherits from `Vector<Type, 3>`.

**Constructors**

| Constructor | Description |
|-------------|-------------|
| `Euler()` | Zero‑initialised (all angles zero). |
| `Euler(const Vector<Type,3>&)` | Copy from a 3‑element vector. |
| `Euler(const Matrix<Type,3,1>&)` | Copy from a 3×1 matrix. |
| `Euler(Type phi, Type theta, Type psi)` | Direct from roll, pitch, yaw (in radians). |
| `Euler(const Dcm<Type>&)` | Compute Euler angles from a direction cosine matrix (rotation matrix). |
| `Euler(const Quaternion<Type>&)` | Compute Euler angles from a quaternion (converted via DCM). |

**Accessors**

| Method | Description |
|--------|-------------|
| `phi() const` | Returns roll angle (radians). |
| `theta() const` | Returns pitch angle (radians). |
| `psi() const` | Returns yaw angle (radians). |
| `phi()` | Mutable reference to roll angle. |
| `theta()` | Mutable reference to pitch angle. |
| `psi()` | Mutable reference to yaw angle. |

**Type Aliases**

| Alias | Definition |
|-------|------------|
| `Eulerf` | `Euler<float>` |
| `Eulerd` | `Euler<double>` |

## Usage Examples

### Convert from DCM

```cpp
Dcm<double> R = ...;                     // some rotation matrix
Eulerd euler(R);                         // extract 3‑2‑1 angles
```

### Convert from Quaternion

```cpp
Quaternionf q = ...;
Eulerf euler(q);
```

### Use in control logic

```cpp
Eulerf attitude;
attitude.phi()   = 0.05f;   // 5 deg roll
attitude.theta() = -0.02f;  // -2 deg pitch
attitude.psi()   = 1.57f;   // 90 deg yaw
```

## Mathematical Definition

Given the 3‑2‑1 intrinsic rotation sequence (yaw → pitch → roll), the direction cosine matrix (from frame 1 to frame 2) is:

```
R = R_z(ψ) · R_y(θ) · R_x(ϕ)
```

where:

```
R_x(ϕ) = [1   0    0; 0  cosϕ  sinϕ; 0 -sinϕ  cosϕ]
R_y(θ) = [cosθ  0 -sinθ; 0   1    0; sinθ  0   cosθ]
R_z(ψ) = [cosψ  sinψ  0; -sinψ cosψ 0; 0     0    1]
```

The conversion from DCM to Euler angles is:

```
θ = asin(-R_31)
ϕ = atan2(R_32, R_33)
ψ = atan2(R_21, R_11)
```

with special handling for the gimbal‑lock singularities at `θ = ±90°`.

## Important Notes

- **Gimbal lock** – When `θ = ±90°` (pitch ±90°), the first and third rotations become degenerate. In these cases, the implementation sets `ϕ = 0` and computes `ψ` from other matrix elements, which yields a valid but non‑unique representation.
- **Range** – Roll (`ϕ`) and yaw (`ψ`) are typically in the range `[−π, π)`; pitch (`θ`) is in `[−π/2, π/2]`.
- **Convention** – This is the **3‑2‑1 intrinsic Tait‑Bryan** sequence, often used in aerospace (yaw‑pitch‑roll). Different fields use different conventions; be consistent when interfacing.
- **Aliasing** – The class inherits from `Vector<Type,3>`, so all vector operations (`+`, `-`, scalar multiplication, norm, etc.) are available.

## Dependencies

- `Vector.hpp` – base vector class.
- `Dcm.hpp` – for conversion to/from direction cosine matrices.
- `Quaternion.hpp` – for conversion to/from quaternions.
- `Matrix.hpp` – underlying matrix types.

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Dcm.hpp` – direction cosine matrix (rotation matrix).
- `Quaternion.hpp` – quaternion rotations.
- `AxisAngle.hpp` – axis‑angle representation.
- `Vector3.hpp` – 3‑element vector.
```