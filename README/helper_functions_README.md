# Helper Functions 

A collection of lightweight, header‑only utility functions for numerical operations, focusing on:

- Robust floating‑point comparisons (with NaN/Inf handling)
- Angle wrapping and unwrapping (`[-π, π)` and `[0, 2π)`)
- Generic range wrapping (floating point and integer)
- Type‑safe sign extraction

This header is part of the PX4 matrix library and is designed to be used together with `Matrix.hpp` and `Slice.hpp`, but it can also be used independently.

## Features

- **Floating point equality** with tolerance, handling NaN and infinity correctly.
- **Range wrapping** for floating point and integer types – keep any value inside `[low, high)`.
- **Angle normalization** – `wrap_pi()` and `wrap_2pi()`.
- **Angle unwrapping** – reconstruct continuous angles from wrapped values.
- **Sign function** – branchless, type‑safe `sign()`.

## Requirements

- C++11 or later compiler.
- Standard C++ library header `<cmath>`.
- No external dependencies.

## Installation

Simply copy `helper_functions.hpp` into your project and include it:

```cpp
#include "helper_functions.hpp"
```

All functions are inside the namespace `matrix`.

## Function Reference

### `isEqualF()`

```cpp
template<typename Type>
bool isEqualF(Type x, Type y, Type eps = Type(1e-4f));
```

Compares two floating‑point numbers with absolute tolerance.

- `x`, `y` – values to compare.
- `eps` – maximum absolute difference considered equal (default `1e-4`).

**Special handling:**
- `NaN` is considered equal to `NaN` (and `-NaN`).
- `+Inf` equals `+Inf`, but `+Inf` does **not** equal `-Inf`.

Returns `true` if the numbers are considered equal.

### `wrap()` – Floating point (float/double)

```cpp
float  wrap(float x, float low, float high);
double wrap(double x, double low, double high);
```

Wraps a floating‑point value into the half‑open interval `[low, high)` using floor‑modulo semantics.

- `x` – input value (may be outside the range).
- `low` – lower bound (inclusive).
- `high` – upper bound (exclusive).

**Precondition:** `low < high`

**Example:** `wrap(3.5, 0.0, 2.0)` → `1.5`

### `wrap()` – Integer

```cpp
template<typename Integer>
Integer wrap(Integer x, Integer low, Integer high);
```

Wraps an integer value into `[low, high)`. Handles negative values correctly.

**Example:** `wrap(-1, 0, 5)` → `4`

### Angle Wrapping

```cpp
template<typename Type>
Type wrap_pi(Type x);      // wrap to [-π, π)

template<typename Type>
Type wrap_2pi(Type x);     // wrap to [0, 2π)
```

Normalise any angle (in radians) to the standard ranges.

### Unwrapping

```cpp
template<typename Type>
Type unwrap(Type last_x, Type new_x, Type low, Type high);

template<typename Type>
Type unwrap_pi(Type last_angle, Type new_angle);
```

Reconstruct a continuous sequence from wrapped values.

- `last_x` – previous unwrapped value.
- `new_x` – next value, assumed to be wrapped inside `[low, high)`.

Returns the new unwrapped value. For angles, `unwrap_pi` uses the range `[-π, π)`.

### `sign()`

```cpp
template<typename T>
int sign(T val);
```

Returns `-1` if `val < 0`, `0` if `val == 0`, `1` if `val > 0`.

- Branchless implementation.
- For floating point, `-0.0` returns `0`.
- Behaviour for `NaN` is unspecified (returns `0` due to comparisons).

## Usage Examples

### Basic comparisons

```cpp
#include "helper_functions.hpp"
#include <iostream>

int main() {
    double a = 1.000001;
    double b = 1.000002;
    if (matrix::isEqualF(a, b, 1e-5)) {
        std::cout << "Almost equal\n";
    }

    // NaN handling
    bool eq = matrix::isEqualF(NAN, NAN); // true
}
```

### Angle wrapping

```cpp
double angle = 5.0;               // rad
double norm = matrix::wrap_pi(angle);   // -> 5.0 - 2π ≈ -1.283
double norm2 = matrix::wrap_2pi(angle); // -> 5.0 - 2π ≈ 5.0 - 6.283 = 5.0? Actually 5.0 - 2π ≈ -1.283, then +2π = 5.0? No, wrap_2pi would give 5.0 - 2π? Let's compute: wrap_2pi(5.0) = wrap(5.0, 0, 2π) = 5.0 - 2π ≈ -1.283, then +2π = 5.0? Wait wrap uses floor, so (5.0-0)/2π = 0.795, floor=0 → no wrap, returns 5.0? Actually 5.0 is already < 2π and ≥0, so wrap_2pi(5.0)=5.0. Better example: wrap_2pi(7.0) = 7.0 - 2π ≈ 0.716.
```

### Unwrapping a sequence

```cpp
double last = 0.0;
double angles[] = {0.1, 3.0, -2.9, 3.2};
for (double a : angles) {
    last = matrix::unwrap_pi(last, a);
    std::cout << last << "\n";
}
// Output: 0.1, 3.0, 3.283, 6.483 (approx)
```

### Integer wrapping

```cpp
int wrapped = matrix::wrap(10, 0, 5);      // 0 (since 10 % 5 = 0)
int wrapped_neg = matrix::wrap(-2, 0, 5);  // 3
```

### Sign function

```cpp
int s1 = matrix::sign(-5);   // -1
int s2 = matrix::sign(0);    // 0
int s3 = matrix::sign(3.7);  // 1
```

## Dependencies

- **`<cmath>`** – for `std::fabs`, `std::isnan`, `std::isinf`, `std::floor`, `M_PI`.
- The header defines `M_PI` if your system’s `<cmath>` does not (e.g., on some embedded toolchains).

## Integration with Matrix Library

This header is automatically included by `Matrix.hpp`. If you use `Matrix`, you get all these functions as well. The `wrap_2pi`, `wrap_pi`, `unwrap_pi` functions are especially useful for state estimation and control systems.

## Notes

- All functions are `inline` or defined in the header – no separate compilation unit needed.
- The integer `wrap` uses modular arithmetic with correction for negative numbers, guaranteeing `[low, high)`.
- For floating‑point range wrapping, the implementation uses `std::floor`; values exactly equal to `high` will be wrapped to `low` because the interval is half‑open.

## License

Same as the parent matrix library – **BSD 3‑Clause License**.

## Authors

- PX4 Development Team
- Mohammad Nikanjam

## See Also

- `Matrix.hpp` – main matrix class
- `Slice.hpp` – matrix views

```