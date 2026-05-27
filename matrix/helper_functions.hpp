/**
 * @file utility.hpp
 * @brief Mathematical utility functions for floating point and integer operations
 * 
 * This file provides a collection of utility functions for numerical operations
 * including floating point comparisons, angle wrapping, and sign calculations.
 * 
 * @author PX4 Development Team
 * @author Mohammad Nikanjam
 * @copyright BSD 3-Clause License
 */

// #######################################################################################

#pragma once

// ######################################################################################
// Include Libraries

#include <cmath>

// #include <px4_platform_common/defines.h>

// Define M_PI if not defined by cmath
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ######################################################################################

namespace matrix
{

/**
 * @brief Compare if two floating point numbers are equal within tolerance
 * 
 * This function provides a robust comparison of floating point values that
 * handles special cases like NaN and infinity appropriately.
 * 
 * Special handling includes:
 * - NAN is considered equal to NAN and -NAN
 * - INFINITY is considered equal to INFINITY but not -INFINITY
 * - Normal numbers compared using absolute tolerance
 * 
 * @tparam Type Floating point type (float, double, etc.)
 * @param x Right side of equality check
 * @param y Left side of equality check
 * @param eps Numerical tolerance for the check (default: 1e-4f)
 * 
 * @return true if the two values are considered equal, false otherwise
 * 
 * @note For values very close to zero, the absolute tolerance may be insufficient.
 *       Consider using relative tolerance for such cases.
 * 
 * @see std::abs, std::isnan, std::isinf
 */
template<typename Type>
bool isEqualF(const Type x, const Type y, const Type eps = Type(1e-4f))
{
	return (std::fabs(x - y) <= eps)
	       || (std::isnan(x) && std::isnan(y))
	       || (std::isinf(x) && std::isinf(y) && std::isnan(x - y));
}

namespace detail
{

/**
 * @brief Internal helper function for floating point range wrapping
 * 
 * This function performs the core wrapping logic for floating point values
 * to keep them within a specified range [low, high).
 * 
 * @tparam Floating Floating point type (float or double)
 * @param x Input value possibly outside the desired range
 * @param low Lower bound of the target range (inclusive)
 * @param high Upper bound of the target range (exclusive)
 * @return Value wrapped into the range [low, high)
 * 
 * @note This is an internal implementation detail and should not be called directly.
 *       Use the public wrap() functions instead.
 */
template<typename Floating>
Floating wrap_floating(Floating x, Floating low, Floating high)
{
	// already in range
	if (low <= x && x < high) {
		return x;
	}

	const auto range = high - low;
	const auto inv_range = Floating(1) / range; // should evaluate at compile time, multiplies below at runtime
	const auto num_wraps = std::floor((x - low) * inv_range);
	return x - range * num_wraps;
}

} // namespace detail

/**
 * @brief Wrap single precision floating point value into range [low, high)
 * 
 * Takes an input value and wraps it into the specified half-open interval
 * using modulo arithmetic with floor semantics.
 * 
 * @param x Input value possibly outside of the range
 * @param low Lower limit of the allowed range (inclusive)
 * @param high Upper limit of the allowed range (exclusive)
 * @return Wrapped value guaranteed to be in [low, high)
 * 
 * @pre low < high
 * @post result >= low && result < high
 * 
 * @see wrap(double, double, double) for double precision version
 */
inline float wrap(float x, float low, float high)
{
	return matrix::detail::wrap_floating(x, low, high);
}

/**
 * @brief Wrap double precision floating point value into range [low, high)
 * 
 * Takes an input value and wraps it into the specified half-open interval
 * using modulo arithmetic with floor semantics.
 * 
 * @param x Input value possibly outside of the range
 * @param low Lower limit of the allowed range (inclusive)
 * @param high Upper limit of the allowed range (exclusive)
 * @return Wrapped value guaranteed to be in [low, high)
 * 
 * @pre low < high
 * @post result >= low && result < high
 * 
 * @see wrap(float, float, float) for single precision version
 */
inline double wrap(double x, double low, double high)
{
	return matrix::detail::wrap_floating(x, low, high);
}

/**
 * @brief Wrap integer value into range [low, high)
 * 
 * Takes an integer input value and wraps it into the specified half-open
 * interval using modulo arithmetic.
 * 
 * @tparam Integer Integer type (int, long, etc.)
 * @param x Input value possibly outside of the range
 * @param low Lower limit of the allowed range (inclusive)
 * @param high Upper limit of the allowed range (exclusive)
 * @return Wrapped value guaranteed to be in [low, high)
 * 
 * @pre low < high
 * @post result >= low && result < high
 * 
 * @note This implementation handles negative values correctly using floor division semantics
 */
template<typename Integer>
Integer wrap(Integer x, Integer low, Integer high)
{
	const auto range = high - low;

	if (x < low) {
		x += range * ((low - x) / range + 1);
	}

	return low + (x - low) % range;
}

/**
 * @brief Wrap angle to range [-π, π) radians
 * 
 * Takes any angle and normalizes it to the standard [-π, π) half-open interval.
 * 
 * @tparam Type Floating point type (float or double)
 * @param x Angle in radians to be wrapped
 * @return Wrapped angle in radians, guaranteed to be in [-π, π)
 * 
 * @post result >= -M_PI && result < M_PI
 * 
 * @see wrap_2pi() for wrapping to [0, 2π) range
 * @see unwrap_pi() for unwrapping cumulative angles
 */
template<typename Type>
Type wrap_pi(Type x)
{
	return wrap(x, Type(-M_PI), Type(M_PI));
}

/**
 * @brief Wrap angle to range [0, 2π) radians
 * 
 * Takes any angle and normalizes it to the standard [0, 2π) half-open interval.
 * 
 * @tparam Type Floating point type (float or double)
 * @param x Angle in radians to be wrapped
 * @return Wrapped angle in radians, guaranteed to be in [0, 2π)
 * 
 * @post result >= 0 && result < 2*M_PI
 * 
 * @see wrap_pi() for wrapping to [-π, π) range
 */
template<typename Type>
Type wrap_2pi(Type x)
{
	return wrap(x, Type(0), Type((2 * M_PI)));
}

/**
 * @brief Unwrap a value that was previously wrapped to range [low, high)
 * 
 * Reconstructs the original continuous sequence from wrapped values by
 * detecting and correcting discontinuities.
 * 
 * @tparam Type Arithmetic type (integer or floating point)
 * @param[in] last_x Last unwrapped value in the sequence
 * @param[in] new_x New wrapped value in the range [low, high)
 * @param low Lower limit of the wrapping range (inclusive)
 * @param high Upper limit of the wrapping range (exclusive)
 * @return New unwrapped value continuous with last_x
 * 
 * @pre low < high
 * @pre new_x is within [low, high)
 * 
 * @see wrap() for the corresponding wrapping function
 * @see unwrap_pi() for angle-specific unwrapping
 */
template<typename Type>
Type unwrap(const Type last_x, const Type new_x, const Type low, const Type high)
{
	return last_x + wrap(new_x - last_x, low, high);
}

/**
 * @brief Unwrap an angle from the range [-π, π) to a continuous sequence
 * 
 * Reconstructs a continuous angle sequence from normalized angles by
 * detecting and correcting 2π discontinuities.
 * 
 * @tparam Type Floating point type (float or double)
 * @param[in] last_angle Last unwrapped angle in radians
 * @param[in] new_angle New angle in radians, expected to be in [-π, π]
 * @return New unwrapped angle continuous with last_angle
 * 
 * @pre new_angle is within [-π, π]
 * 
 * @note This is particularly useful for integrating angular velocities
 *       or tracking cumulative rotation.
 * 
 * @see wrap_pi() for the corresponding wrapping function
 * @see unwrap() for generic unwrapping
 */
template<typename Type>
Type unwrap_pi(const Type last_angle, const Type new_angle)
{
	return unwrap(last_angle, new_angle, Type(-M_PI), Type(M_PI));
}

/**
 * @brief Type-safe sign/signum function
 * 
 * Returns the mathematical sign of the input value.
 * 
 * @tparam T Arithmetic type (integer or floating point)
 * @param[in] val Number to take the sign from
 * @return -1 if val < 0, 0 if val == 0, 1 if val > 0
 * 
 * @note This implementation is branchless and type-safe.
 * @note For floating point values, -0.0 will return 0.
 * 
 * @warning Behavior for NaN is undefined (will return 0 due to comparisons)
 * 
 * @see std::copysign for alternative sign handling
 */
template<typename T>
int sign(T val)
{
	return (T(0) < val) - (val < T(0));
}

} // namespace matrix
