# Quaternion – Hamilton Quaternion Library for Rotations

A header‑only C++ class implementing **Hamilton quaternions** for 3D rotations, following the right‑hand rule.  
The library provides robust conversion between quaternions, direction cosine matrices (DCM), Euler angles, axis‑angle, and two‑vector rotations. It also includes quaternion exponentials and derivatives for kinematic integration.

This header is part of the PX4 matrix library and works with `Vector3.hpp`, `Vector4.hpp`, `Dcm.hpp`, `Euler.hpp`, and `AxisAngle.hpp`.

## Features

- **Hamilton convention** – quaternion `q` with real part first: `q = (q₀, q₁, q₂, q₃)`.
- **Multiplication** – `z = q2 * q1` corresponds to first rotating by `q1`, then by `q2`.
- **Inverse** – `q⁻¹ = conj(q) / |q|²` (real part unchanged, imaginary parts negated).
- **Constructors** – from DCM, Euler angles, axis‑angle, two vectors, or scalar components.
- **Rotate vectors** – `rotateVector()` rotates a vector from frame 1 to frame 2 using `v₂ = q * v₁ * q⁻¹`.
- **Derivatives** – kinematic derivatives with angular velocity expressed in either frame.
- **Quaternion exponential** – `expq(u)` for updating quaternions from body rates.
- **Right Jacobian inverse** – `inv_r_jacobian(u)` for higher‑order integration (RK4 on quaternion logarithm).
- **Canonical form** – ensures the real part is non‑negative.
- **Type aliases** – `Quaternionf` / `Quatf` (float), `Quaterniond` / `Quatd` (double).

## Requirements

- C++11 or later.
- Depends on `Vector3.hpp`, `Vector4.hpp`, `Dcm.hpp`, `Euler.hpp`, `AxisAngle.hpp`.
- Standard headers: `<float.h>` (for `FLT_EPSILON`), `<cmath>`.

## Quick Start

```cpp
#include "Quaternion.hpp"
#include <iostream>

using namespace matrix;

int main() {
    // Identity quaternion
    Quaternionf q;

    // Rotation of 90° around Z axis
    Quaternionf r(0.7071f, 0.0f, 0.0f, 0.7071f);  // (cos45°, 0, 0, sin45°)

    // Rotate a vector
    Vector3f v(1.0f, 0.0f, 0.0f);
    Vector3f rotated = r.rotateVector(v);   // ~ (0,1,0)

    // Quaternion multiplication
    auto q2 = r * q;

    // Inverse
    auto inv = r.inversed();

    // Convert to DCM
    Dcm<float> R(r);

    std::cout << rotated << "\n";
}
```

## API Reference

### Class `Quaternion<Type>`

Inherits from `Vector4<Type>` (real part first).

**Constructors**

| Constructor | Description |
|-------------|-------------|
| `Quaternion()` | Identity quaternion `(1,0,0,0)`. |
| `explicit Quaternion(const Type data[4])` | From C array. |
| `Quaternion(const Matrix41&)` | From 4×1 matrix. |
| `Quaternion(const Dcm<Type>&)` | From direction cosine matrix (rotation matrix). |
| `Quaternion(const Euler<Type>&)` | From Euler angles (3‑2‑1 intrinsic sequence). |
| `Quaternion(const AxisAngle<Type>&)` | From axis‑angle vector. |
| `Quaternion(const Vector3<Type>& src, const Vector3<Type>& dst, Type eps=1e-5)` | Shortest rotation from `src` to `dst`. |
| `Quaternion(Type a, Type b, Type c, Type d)` | From components `(a,b,c,d)`. |

**Operators**

| Operator | Description |
|----------|-------------|
| `Quaternion operator*(const Quaternion&) const` | Hamilton product (rotation composition). |
| `void operator*=(const Quaternion&)` | In‑place multiplication. |
| `Quaternion operator*(Type) const` | Scalar multiplication. |
| `void operator*=(Type)` | In‑place scalar multiplication. |

**Key Methods**

| Method | Description |
|--------|-------------|
| `Quaternion inversed() const` | Returns the inverse (conjugate divided by squared norm). |
| `void invert()` | In‑place inversion. |
| `void canonicalize()` | Bring to canonical form (real part ≥ 0). |
| `Quaternion canonical() const` | Return canonical version. |
| `Vector3<Type> imag() const` | Returns imaginary part `(q₁,q₂,q₃)`. |
| `Vector3<Type> rotateVector(const Vector3<Type>&) const` | Rotate vector from frame 1 to frame 2: `v₂ = q * v₁ * q⁻¹`. |
| `Vector3<Type> rotateVectorInverse(const Vector3<Type>&) const` | Inverse rotation: `v₁ = q⁻¹ * v₂ * q`. |
| `Matrix41 derivative1(const Vector3<Type>& w) const` | Derivative when angular velocity `w` is expressed in the **rotating frame** (body). `dq/dt = 0.5 * q * ω_body` |
| `Matrix41 derivative2(const Vector3<Type>& w) const` | Derivative when angular velocity `w` is expressed in the **inertial frame**. `dq/dt = 0.5 * ω_inertial * q` |
| `Vector3<Type> dcm_z() const` | Returns the last column of the equivalent DCM (body z‑axis in inertial frame), computed efficiently. |

**Static Methods**

| Method | Description |
|--------|-------------|
| `static Quaternion expq(const Vector3<Type>& u)` | Quaternion exponential: `expq(u) = (cos‖u‖, sinc‖u‖ * u)`. Used for robust quaternion updates from body rates: `q_{k+1} = q_k * expq(dt * ω/2)`. |
| `static Dcm<Type> inv_r_jacobian(const Vector3<Type>& u)` | Inverse right Jacobian of the quaternion logarithm, for higher‑order integration (e.g., RK4) on the logarithm `u`. |

**Type Aliases**

| Alias | Definition |
|-------|------------|
| `Quatf`, `Quaternionf` | `Quaternion<float>` |
| `Quatd`, `Quaterniond` | `Quaternion<double>` |

## Usage Examples

### Convert between representations

```cpp
// DCM → Quaternion
Dcm<float> R = ...;
Quaternionf q(R);

// Euler (321) → Quaternion
Eulerf euler(0.1f, 0.2f, 0.3f);
Quaternionf qe(euler);

// Axis‑angle → Quaternion
AxisAnglef aa(Vector3f(1,0,0), 1.57f);
Quaternionf qaa(aa);

// Two vectors – shortest rotation from source to destination
Vector3f src(1,0,0);
Vector3f dst(0,1,0);
Quaternionf qvec(src, dst);   // rotates around Z by 90°
```

### Rotate a vector

```cpp
Quaternionf q(0.7071f, 0, 0, 0.7071f);  // 90° around Z
Vector3f v(1,0,0);
Vector3f v_rot = q.rotateVector(v);      // (0,1,0)
```

### Compose rotations

```cpp
Quaternionf q1 = ...;  // first rotation
Quaternionf q2 = ...;  // second rotation
Quaternionf total = q2 * q1;  // apply q1 then q2
```

### Kinematics – derivative and exponential update

```cpp
Quaternionf q;
Vector3f omega_body(0.1f, 0.2f, 0.3f);
float dt = 0.01f;

// Method 1: derivative (Euler integration)
auto dq = q.derivative1(omega_body);
q += dq * dt;
q.normalize();

// Method 2: exponential (more robust)
Quaternionf delta = Quaternionf::expq(omega_body * dt / 2.0f);
q = delta * q;
q.normalize();  // still recommended
```

### Obtain body z‑axis (often needed for attitude control)

```cpp
Quaternionf q;
Vector3f body_z = q.dcm_z();   // points from body frame to inertial z
```

### Canonical form

```cpp
Quaternionf q( -0.5f, 0.5f, 0.5f, 0.5f );
q.canonicalize();   // makes real part positive: (0.5, -0.5, -0.5, -0.5)
```

## Important Notes

- **Hamilton convention** – This library uses the Hamilton product. Many other libraries (e.g., Eigen) also use it, but some (like JPL) use a different convention. Ensure consistency when integrating with external code.
- **Real part first** – Quaternion stored as `(w, x, y, z)`.
- **Rotation formula** – To rotate a vector from frame A to frame B using a quaternion `q_BA` (rotation from A to B):  
  `v_B = q_BA * (0, v_A) * q_BA⁻¹`. This library's `rotateVector` implements exactly that.
- **Multiplication order** – `z = q2 * q1` applies `q1` **first**, then `q2`. This is consistent with rotation matrix multiplication.
- **Normalisation** – Many operations assume unit quaternions. Use `normalize()` (inherited from `Vector4`) when needed.
- **Tolerance** – The two‑vector constructor uses a small epsilon to detect opposite vectors. For 180° rotations, a perpendicular axis is chosen.
- **Performance** – All methods are inlined; the class adds little overhead over the underlying `Vector4`.

## Dependencies

- `Vector3.hpp`, `Vector4.hpp` – for vector operations.
- `Dcm.hpp` – for conversion to/from direction cosine matrices.
- `Euler.hpp` – for conversion to/from Euler angles.
- `AxisAngle.hpp` – for axis‑angle representation.
- `Matrix.hpp` – base matrix class (indirectly).

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- James Goppert <james.goppert@gmail.com>
- PX4 Development Team

## See Also

- `Dcm.hpp` – direction cosine matrix (rotation matrix).
- `Euler.hpp` – Euler angles (3‑2‑1 intrinsic).
- `AxisAngle.hpp` – axis‑angle representation.
- `Vector3.hpp`, `Vector4.hpp` – base vector classes.
```