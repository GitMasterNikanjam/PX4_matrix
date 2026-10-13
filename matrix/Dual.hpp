/**
 * @file Dual.hpp
 * @brief Forward-mode automatic differentiation using dual numbers.
 *
 * This is a dual number type for calculating automatic derivatives.
 *
 * Based roughly on the methods described in:
 * Automatic Differentiation, C++ Templates and Photogrammetry, by Dan Piponi
 * and
 * Ceres Solver's excellent Jet.h
 *
 * @author Julian Kent <julian@auterion.com>
 */

#pragma once

#include <cmath>

#include "Scalar.hpp"
#include "Vector.hpp"

namespace matrix
{

/**
 * @brief Scalar value paired with its gradient with respect to N inputs.
 *
 * Arithmetic and supported elementary functions propagate derivatives using
 * forward-mode automatic differentiation.
 * @tparam Scalar Floating-point scalar type.
 * @tparam N Number of independent input variables.
 */
template <typename Scalar, size_t N>
struct Dual {
	/** Number of derivative components. */
	static constexpr size_t WIDTH = N;

	/** The ordinary, non-derivative value. */
	Scalar value {};
	/** Partial derivatives ordered by independent input variable. */
	Vector<Scalar, N> derivative;

	/** @brief Construct a zero value with zero derivatives. */
	Dual() = default;

	/**
	 * @brief Construct a value and optionally mark one component as an input.
	 * @param v Initial scalar value.
	 * @param inputDimension Derivative index to set to one; values >= N mean
	 *                       this value is a constant.
	 */
	explicit Dual(Scalar v, size_t inputDimension = 65535)
	{
		value = v;

		// Seed the selected independent variable with derivative one; the
		// default sentinel leaves every derivative at zero.
		if (inputDimension < N) {
			derivative(inputDimension) = Scalar(1);
		}
	}

	/** @brief Construct from a value and an explicit derivative vector. */
	explicit Dual(Scalar v, const Vector<Scalar, N> &d) :
		value(v), derivative(d)
	{}

	/** @brief Assign a constant and clear all derivatives. */
	Dual<Scalar, N> &operator=(const Scalar &a)
	{
		derivative.setZero();
		value = a;
		return *this;
	}

	/** @brief Add another dual number in place. */
	Dual<Scalar, N> &operator +=(const Dual<Scalar, N> &a)
	{
		return (*this = *this + a);
	}

	/** @brief Multiply by another dual number in place. */
	Dual<Scalar, N> &operator *=(const Dual<Scalar, N> &a)
	{
		return (*this = *this * a);
	}

	/** @brief Subtract another dual number in place. */
	Dual<Scalar, N> &operator -=(const Dual<Scalar, N> &a)
	{
		return (*this = *this - a);
	}

	/** @brief Divide by another dual number in place. */
	Dual<Scalar, N> &operator /=(const Dual<Scalar, N> &a)
	{
		return (*this = *this / a);
	}

	/** @brief Add a constant in place; derivatives are unchanged. */
	Dual<Scalar, N> &operator +=(Scalar a)
	{
		return (*this = *this + a);
	}

	/** @brief Subtract a constant in place; derivatives are unchanged. */
	Dual<Scalar, N> &operator -=(Scalar a)
	{
		return (*this = *this - a);
	}

	/** @brief Scale the value and gradient in place. */
	Dual<Scalar, N> &operator *=(Scalar a)
	{
		return (*this = *this * a);
	}

	/** @brief Divide the value and gradient by a constant in place. */
	Dual<Scalar, N> &operator /=(Scalar a)
	{
		return (*this = *this / a);
	}

	/** @brief Compare value and gradient, using tolerant comparison for value. */
	bool operator==(const Dual<Scalar, N> &other) const
	{
		return isEqualF(value, other.value) && (derivative == other.derivative);
	}

	/** @brief Return true when value or gradient differs. */
	bool operator!=(const Dual<Scalar, N> &other) const
	{
		const Dual<Scalar, N> &self = *this;
		return !(self == other);
	}
};

/** @name Dual arithmetic operators
 * These overloads apply the usual arithmetic rules and propagate gradients.
 * @{ */

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(const Dual<Scalar, N> &a)
{
	return a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(const Dual<Scalar, N> &a)
{
	return Dual<Scalar, N>(-a.value, -a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(const Dual<Scalar, N> &a, const Dual<Scalar, N> &b)
{
	return Dual<Scalar, N>(a.value + b.value, a.derivative + b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(const Dual<Scalar, N> &a, const Dual<Scalar, N> &b)
{
	return a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(const Dual<Scalar, N> &a, Scalar b)
{
	return Dual<Scalar, N>(a.value + b, a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(const Dual<Scalar, N> &a, Scalar b)
{
	return a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator+(Scalar a, const Dual<Scalar, N> &b)
{
	return Dual<Scalar, N>(a + b.value, b.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator-(Scalar a, const Dual<Scalar, N> &b)
{
	return a + (-b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(const Dual<Scalar, N> &a, const Dual<Scalar, N> &b)
{
	// Product rule: d(a*b) = a*db + b*da.
	return Dual<Scalar, N>(a.value * b.value, a.value * b.derivative + b.value * a.derivative);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(const Dual<Scalar, N> &a, Scalar b)
{
	return Dual<Scalar, N>(a.value * b, a.derivative * b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator*(Scalar a, const Dual<Scalar, N> &b)
{
	return b * a;
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(const Dual<Scalar, N> &a, const Dual<Scalar, N> &b)
{
	// Quotient rule written using 1/b to reuse the reciprocal in value and gradient.
	const Scalar inv_b_real = Scalar(1) / b.value;
	return Dual<Scalar, N>(a.value * inv_b_real, a.derivative * inv_b_real -
			       a.value * b.derivative * inv_b_real * inv_b_real);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(const Dual<Scalar, N> &a, Scalar b)
{
	return a * (Scalar(1) / b);
}

template <typename Scalar, size_t N>
Dual<Scalar, N> operator/(Scalar a, const Dual<Scalar, N> &b)
{
	// d(a/b) = -a * db / b^2 because the numerator is constant.
	const Scalar inv_b_real = Scalar(1) / b.value;
	return Dual<Scalar, N>(a * inv_b_real, (-inv_b_real * a * inv_b_real) * b.derivative);
}

/** @} */

/** @name Elementary functions for dual numbers
 * Each derivative follows the corresponding scalar chain rule.
 * @{ */

/** @brief Square root with derivative da/(2*sqrt(a)). */
template <typename Scalar, size_t N>
Dual<Scalar, N> sqrt(const Dual<Scalar, N> &a)
{
	// The derivative is undefined at zero; callers should avoid that point.
	Scalar real = std::sqrt(a.value);
	return Dual<Scalar, N>(real, a.derivative * (Scalar(1) / (Scalar(2) * real)));
}

/** @brief Absolute value; the derivative changes sign for negative inputs. */
template <typename Scalar, size_t N>
Dual<Scalar, N> abs(const Dual<Scalar, N> &a)
{
	return a.value >= Scalar(0) ? a : -a;
}

/** @brief Ceiling; returns a constant dual because the function is locally flat. */
template <typename Scalar, size_t N>
Dual<Scalar, N> ceil(const Dual<Scalar, N> &a)
{
	return Dual<Scalar, N>(std::ceil(a.value));
}

/** @brief Floor; returns a constant dual because the function is locally flat. */
template <typename Scalar, size_t N>
Dual<Scalar, N> floor(const Dual<Scalar, N> &a)
{
	return Dual<Scalar, N>(std::floor(a.value));
}

/** @brief Remainder by a constant; carries the input derivative through. */
template <typename Scalar, size_t N>
Dual<Scalar, N> fmod(const Dual<Scalar, N> &a, Scalar mod)
{
	return Dual<Scalar, N>(a.value - Scalar(size_t(a.value / mod)) * mod, a.derivative);
}

/** @brief Select the dual operand with the greater value. */
template <typename Scalar, size_t N>
Dual<Scalar, N> max(const Dual<Scalar, N> &a, const Dual<Scalar, N> &b)
{
	return a.value >= b.value ? a : b;
}

/** @brief Select the dual operand with the lesser value. */
template <typename Scalar, size_t N>
Dual<Scalar, N> min(const Dual<Scalar, N> &a, const Dual<Scalar, N> &b)
{
	return a.value < b.value ? a : b;
}

/** @brief Test whether a scalar value is NaN. */
template <typename Scalar>
bool IsNan(Scalar a)
{
	return std::isnan(a);
}

/** @brief Test whether the value component of a dual number is NaN. */
template <typename Scalar, size_t N>
bool IsNan(const Dual<Scalar, N> &a)
{
	return IsNan(a.value);
}

/** @brief Test whether a scalar value is finite. */
template <typename Scalar>
bool IsFinite(Scalar a)
{
	return std::isfinite(a);
}

/** @brief Test whether the value component of a dual number is finite. */
template <typename Scalar, size_t N>
bool IsFinite(const Dual<Scalar, N> &a)
{
	return IsFinite(a.value);
}

/** @brief Test whether a scalar value is infinite. */
template <typename Scalar>
bool IsInf(Scalar a)
{
	return std::isinf(a);
}

/** @brief Test whether the value component of a dual number is infinite. */
template <typename Scalar, size_t N>
bool IsInf(const Dual<Scalar, N> &a)
{
	return IsInf(a.value);
}

/** @brief Sine with derivative cos(a)*da. */
template <typename Scalar, size_t N>
Dual<Scalar, N> sin(const Dual<Scalar, N> &a)
{
	return Dual<Scalar, N>(std::sin(a.value), std::cos(a.value) * a.derivative);
}

/** @brief Cosine with derivative -sin(a)*da. */
template <typename Scalar, size_t N>
Dual<Scalar, N> cos(const Dual<Scalar, N> &a)
{
	return Dual<Scalar, N>(std::cos(a.value), -std::sin(a.value) * a.derivative);
}

/** @brief Tangent with derivative (1+tan(a)^2)*da. */
template <typename Scalar, size_t N>
Dual<Scalar, N> tan(const Dual<Scalar, N> &a)
{
	Scalar real = std::tan(a.value);
	return Dual<Scalar, N>(real, (Scalar(1) + real * real) * a.derivative);
}

/** @brief Inverse sine with derivative da/sqrt(1-a^2). */
template <typename Scalar, size_t N>
Dual<Scalar, N> asin(const Dual<Scalar, N> &a)
{
	Scalar asin_d = Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	return Dual<Scalar, N>(std::asin(a.value), asin_d * a.derivative);
}

/** @brief Inverse cosine with derivative -da/sqrt(1-a^2). */
template <typename Scalar, size_t N>
Dual<Scalar, N> acos(const Dual<Scalar, N> &a)
{
	Scalar acos_d = -Scalar(1) / std::sqrt(Scalar(1) - a.value * a.value);
	return Dual<Scalar, N>(std::acos(a.value), acos_d * a.derivative);
}

/** @brief Inverse tangent with derivative da/(1+a^2). */
template <typename Scalar, size_t N>
Dual<Scalar, N> atan(const Dual<Scalar, N> &a)
{
	Scalar atan_d = Scalar(1) / (Scalar(1) + a.value * a.value);
	return Dual<Scalar, N>(std::atan(a.value), atan_d * a.derivative);
}

/** @brief Two-argument inverse tangent with derivatives for both operands. */
template <typename Scalar, size_t N>
Dual<Scalar, N> atan2(const Dual<Scalar, N> &a, const Dual<Scalar, N> &b)
{
	// Differentiate atan2(a,b) directly; this form avoids explicitly forming a/b.
	Scalar atan_d = Scalar(1) / (a.value * a.value + b.value * b.value);
	return Dual<Scalar, N>(std::atan2(a.value, b.value), (a.derivative * b.value - a.value * b.derivative) * atan_d);
}

/** @} */

/** @brief Collect each dual element's gradient as one row of a Jacobian.
 * @tparam Scalar Underlying scalar type.
 * @tparam M Number of output values.
 * @tparam N Number of independent inputs.
 * @param input Mx1 vector of dual values.
 * @return MxN Jacobian matrix.
 */
template <typename Scalar, size_t M, size_t N>
Matrix<Scalar, M, N> collectDerivatives(const Matrix<Dual<Scalar, N>, M, 1> &input)
{
	Matrix<Scalar, M, N> jac;

	for (size_t i = 0; i < M; i++) {
		jac.row(i) = input(i, 0).derivative;
	}

	return jac;
}

/** @brief Extract value components from a matrix of dual numbers.
 * @tparam Scalar Underlying scalar type.
 * @tparam M Number of rows.
 * @tparam N Number of columns.
 * @tparam D Derivative dimension carried by each dual value.
 * @param input Matrix whose elements are dual numbers.
 * @return MxN matrix containing only the values.
 */
template <typename Scalar, size_t M, size_t N, size_t D>
Matrix<Scalar, M, N> collectReals(const Matrix<Dual<Scalar, D>, M, N> &input)
{
	Matrix<Scalar, M, N> r;

	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			r(i, j) = input(i, j).value;
		}
	}

	return r;
}

/** @brief Write a dual number's value and gradient to an output stream. */
template<typename OStream, typename Type, size_t N>
OStream &operator<<(OStream &os, const matrix::Dual<Type, N> &dual)
{
	os << "\nValue: " << dual.value << "\nDerivative:" << dual.derivative;
	return os;
}

}
