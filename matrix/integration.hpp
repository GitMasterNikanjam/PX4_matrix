/**
 * @file integration.hpp
 * @brief Numerical integration utilities for ordinary differential equations (ODEs)
 * 
 * This file provides a Runge‑Kutta 4th order (RK4) integrator for systems of
 * the form:
 *   dy/dt = f(t, y, u)
 * where y is the state vector of dimension M, u is a constant input vector of
 * dimension N, and t is time.
 * 
 * The RK4 method is a one‑step method with fourth‑order accuracy. It evaluates
 * the derivative four times per step and is suitable for moderately stiff
 * problems when small step sizes are used.
 * 
 * @author PX4 Development Team
 * @ingroup matrix
 */

#pragma once

#include "Vector.hpp"

namespace matrix
{

/**
 * @brief Integrate an ODE from t0 to tf using the 4th order Runge‑Kutta method
 * 
 * This function integrates the system:
 *   dy/dt = f(t, y, u)
 * from initial time t0 to final time tf, starting from initial state y0,
 * with constant input u. The integration is performed with a fixed step size
 * h0, except for the last step which may be truncated to exactly reach tf.
 * 
 * @tparam Type Floating‑point type (float, double)
 * @tparam M Dimension of the state vector y
 * @tparam N Dimension of the input vector u
 * 
 * @param f Function pointer to the ODE right‑hand side.
 *          Signature: Vector<Type,M> f(Type t, const Matrix<Type,M,1>& y, const Matrix<Type,N,1>& u)
 * @param y0 Initial state vector at time t0 (size M×1)
 * @param u Constant input vector (size N×1) – assumed constant over the whole interval
 * @param t0 Initial time
 * @param tf Final time (must be > t0)
 * @param h0 Fixed step size (the actual step will be min(h0, remaining time))
 * @param[out] y1 Final state vector at time tf (size M×1)
 * @return 0 on success, -1 if tf < t0 (invalid time interval)
 * 
 * @note The function uses a simple while‑loop and applies the RK4 formula:
 *       k1 = f(t, y, u)
 *       k2 = f(t + h/2, y + h/2 * k1, u)
 *       k3 = f(t + h/2, y + h/2 * k2, u)
 *       k4 = f(t + h, y + h * k3, u)
 *       y_next = y + h/6 * (k1 + 2*k2 + 2*k3 + k4)
 * 
 * @warning The input u is assumed constant. For time‑varying inputs, you need
 *          to extend the function or pass u as a function of t.
 * 
 * @example
 * @code
 * // Example: integrate a simple harmonic oscillator
 * // y = [position; velocity], u = 0 (no input)
 * Vector<float,2> f(float t, const Matrix<float,2,1>& y, const Matrix<float,1,1>& u) {
 *     Vector<float,2> dydt;
 *     dydt(0) = y(1);
 *     dydt(1) = -y(0);   // omega = 1
 *     return dydt;
 * }
 * 
 * Vector<float,2> y0(1, 0);   // start at position 1, velocity 0
 * Vector<float,1> u(0);
 * Vector<float,2> y1;
 * integrate_rk4(f, y0, u, 0.0f, 2*M_PI, 0.01f, y1);
 * // y1 should be approximately (1, 0) after one full period
 * @endcode
 */
template<typename Type, size_t M, size_t N>
int integrate_rk4(
	Vector<Type, M> (*f)(Type, const Matrix<Type, M, 1> &x, const Matrix<Type, N, 1> &u),
	const Matrix<Type, M, 1> &y0,
	const Matrix<Type, N, 1> &u,
	Type t0,
	Type tf,
	Type h0,
	Matrix<Type, M, 1> &y1
)
{
	// https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
	Type t1 = t0;
	y1 = y0;
	Type h = h0;
	Vector<Type, M> k1, k2, k3, k4;

	if (tf < t0) { return -1; } // make sure t1 > t0

	while (t1 < tf) {
		if (t1 + h0 < tf) {
			h = h0;

		} else {
			h = tf - t1;
		}

		k1 = f(t1, y1, u);
		k2 = f(t1 + h / 2, y1 + k1 * h / 2, u);
		k3 = f(t1 + h / 2, y1 + k2 * h / 2, u);
		k4 = f(t1 + h, y1 + k3 * h, u);
		y1 += (k1 + k2 * 2 + k3 * 2 + k4) * (h / 6);
		t1 += h;
	}

	return 0;
}

} // namespace matrix
