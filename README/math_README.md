# `math.hpp` – Complete Matrix API Include

`src/math.hpp` includes the library's public matrix, vector, rotation,
automatic differentiation, and numerical algorithm headers. It defines no
separate matrix operation; it is a convenience entry point for code that uses
many parts of the library.

## Theory

An umbrella header trades simpler source includes for a larger set of
transitive declarations and more parsing work for the compiler. The generated
program only contains instantiated code that is used, but the compiler still
processes all included declarations. On STM32 projects, include individual
headers when practical to keep compile times and dependencies focused.

## Requirements

- C++11 or later.
- The include path must contain the library's `matrix` directory.
- The header-only library does not require a separate matrix binary.

## Usage examples

### Include the full API

```cpp
#include <src/math.hpp>

matrix::Vector3f gravity(0.0f, 0.0f, 9.81f);
matrix::Quaternionf attitude;
matrix::Vector3f body_gravity = attitude.rotateVectorInverse(gravity);
```

### Include only the pieces needed

```cpp
#include <src/Vector3.hpp>
#include <src/Quaternion.hpp>

matrix::Quaternionf q;
matrix::Vector3f v(1.0f, 0.0f, 0.0f);
matrix::Vector3f rotated = q.rotateVector(v);
```

### Select types for a small embedded module

```cpp
#include <src/SquareMatrix.hpp>
#include <src/Vector.hpp>

using Covariance = matrix::SquareMatrix<float, 6>;
using State = matrix::Vector<float, 6>;
```

The types above allocate fixed-size storage when instantiated. Their dimensions
and any temporary matrices should fit the target's stack and memory budget.

## See also

- [Guide index](README.md)
- [Matrix fundamentals](Matrix_README.md)
- [Vector operations](Vector_README.md)
- [Square matrix operations](SquareMatrix_README.md)
