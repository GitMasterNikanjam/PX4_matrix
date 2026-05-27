# Integration Utilities – Runge‑Kutta 4th Order (RK4)

A header‑only C++ function for numerically integrating ordinary differential equations (ODEs) using the classical **4th order Runge‑Kutta method**. It is designed for systems of the form:

```
dy/dt = f(t, y, u)
```

where `y` is a state vector of dimension `M`, `u` is a constant input vector of dimension `N`, and `t` is time.

## Features

- **RK4 integration** – fourth‑order accuracy, low memory overhead.
- **Fixed step size** – user‑supplied step `h0`; the last step is truncated to match the final time exactly.
- **Constant inputs** – the input `u` is assumed constant over the whole integration interval.
- **Header‑only** – no separate compilation; just include and use.
- **Compile‑time dimensions** – all vector dimensions are template parameters (no heap allocation).

## Requirements

- C++11 or later.
- Depends on `Vector.hpp` (and indirectly `Matrix.hpp`).
- Standard headers: none beyond those included by `Vector.hpp`.

## Quick Start

```cpp
#include "integration.hpp"
#include <iostream>

using namespace matrix;

// Define the ODE: simple harmonic oscillator
// y = [position; velocity]
Vector<float,2> oscillator(float t, const Matrix<float,2,1>& y, const Matrix<float,1,1>& u) {
    Vector<float,2> dydt;
    dydt(0) = y(1);
    dydt(1) = -y(0);   // omega = 1
    return dydt;
}

int main() {
    Vector<float,2> y0;   // initial state
    y0(0) = 1.0f;
    y0(1) = 0.0f;

    Vector<float,1> u;    // no input
    u(0) = 0.0f;

    Vector<float,2> yf;
    int ret = integrate_rk4(oscillator, y0, u, 0.0f, 2*M_PI, 0.01f, yf);

    if (ret == 0) {
        std::cout << "Final state: " << yf << "\n";
    }
}
```

## API Reference

### Function `integrate_rk4`

```cpp
template<typename Type, size_t M, size_t N>
int integrate_rk4(
    Vector<Type, M> (*f)(Type, const Matrix<Type, M, 1>&, const Matrix<Type, N, 1>&),
    const Matrix<Type, M, 1>& y0,
    const Matrix<Type, N, 1>& u,
    Type t0,
    Type tf,
    Type h0,
    Matrix<Type, M, 1>& y1
);
```

**Parameters**:

| Parameter | Description |
|-----------|-------------|
| `f` | Function pointer to the ODE right‑hand side: `dy/dt = f(t, y, u)`. Must return a `Vector<Type, M>`. |
| `y0` | Initial state vector at time `t0` (size `M×1`). |
| `u` | Constant input vector (size `N×1`). Assumed constant over the whole interval. |
| `t0` | Initial time. |
| `tf` | Final time (must be > `t0`). |
| `h0` | Fixed step size. The actual step is `min(h0, remaining time)`. |
| `y1` | Output: final state vector at time `tf`. |

**Returns**:
- `0` on success.
- `-1` if `tf < t0` (invalid time interval).

**Algorithm** (per step of size `h`):
```
k1 = f(t, y, u)
k2 = f(t + h/2, y + h/2·k1, u)
k3 = f(t + h/2, y + h/2·k2, u)
k4 = f(t + h, y + h·k3, u)
y_next = y + h/6 · (k1 + 2·k2 + 2·k3 + k4)
```

## Usage Examples

### Integrate a damped harmonic oscillator

```cpp
// y = [x, v], with damping coefficient c
Vector<double,2> damped_osc(double t, const Matrix<double,2,1>& y, const Matrix<double,1,1>& u) {
    double c = u(0);   // damping coefficient
    Vector<double,2> dydt;
    dydt(0) = y(1);
    dydt(1) = -y(0) - c * y(1);
    return dydt;
}

int main() {
    Vector<double,2> y0(1.0, 0.0);
    Vector<double,1> u; u(0) = 0.1;   // damping
    Vector<double,2> yf;
    integrate_rk4(damped_osc, y0, u, 0.0, 10.0, 0.01, yf);
}
```

### Use with time‑varying input (workaround)

Since `integrate_rk4` assumes `u` is constant, wrap the time‑varying input inside the ODE function:

```cpp
Vector<float,2> my_ode(float t, const Matrix<float,2,1>& y, const Matrix<float,1,1>& u_dummy) {
    float u = sin(t);   // time‑varying input
    Vector<float,2> dydt;
    dydt(0) = y(1);
    dydt(1) = -y(0) + u;
    return dydt;
}
```

## Important Notes

- **Constant input** – The function signature includes `u` but the integrator does not modify it; it simply passes the same `u` to every evaluation of `f`. This is efficient for systems where the input is truly constant over the integration interval.
- **Step size** – The integrator uses a fixed step `h0` except for the last step, which is shortened to land exactly on `tf`. Adaptive step size is not provided.
- **Accuracy** – RK4 has local truncation error `O(h⁵)` and global error `O(h⁴)`. For many problems it is sufficiently accurate; for stiff ODEs you may need implicit methods.
- **Performance** – Each step evaluates `f` four times. For high‑dimensional systems or very many steps, consider alternative integrators.
- **Return value** – The function returns `-1` only if `tf < t0`. It does **not** check for numerical issues (e.g., NaN, Inf) inside `f`.

## Dependencies

- `Vector.hpp` – provides `Vector` and `Matrix` types.
- `Matrix.hpp` – indirectly required for `Matrix<Type, M, 1>` (the state and input types).

## License

Same as the parent matrix library – **BSD 3‑Clause License** (see the copyright notice in the file header).

## Authors

- PX4 Development Team

## See Also

- `Vector.hpp` – dense vector class.
- `Matrix.hpp` – dense matrix class (used for state and input).
- `helper_functions.hpp` – utilities like `isEqualF` (not used here).
```