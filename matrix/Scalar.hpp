/**
 * @file Scalar.hpp
 * @brief Conversion and wrapper class for scalar values used in matrix algebra
 * 
 * This file provides a Scalar class that wraps a single numeric value and enables
 * seamless conversion between primitive scalars, 1×1 matrices, and 1‑dimensional
 * vectors. It is useful for writing generic matrix code that must handle both
 * scalar and matrix operands uniformly.
 * 
 * Key features:
 * - Wraps a constant scalar value (immutable after construction)
 * - Implicit conversion from 1×1 matrix or primitive type
 * - Conversion operators to primitive type, 1×1 matrix, and Vector<Type,1>
 * - Equality comparison with float (using tolerance)
 * - Type aliases Scalarf and Scalard for common floating‑point types
 * 
 * @author James Goppert <james.goppert@gmail.com>
 * @ingroup matrix
 */

#pragma once

#include "Matrix.hpp"

namespace matrix
{

/**
 * @brief Immutable wrapper for a single scalar value, convertible to/from matrix/vector types
 * 
 * The Scalar class holds a constant value and provides implicit conversions to/from
 * Matrix<Type,1,1> and Vector<Type,1>. This allows scalar values to be used in
 * matrix expressions where a 1×1 matrix is expected, and vice versa.
 * 
 * @tparam Type The numeric type (float, double, int, etc.)
 * 
 * @note Objects of this class are immutable – the wrapped value cannot be changed
 *       after construction. This is by design to avoid ambiguity in expression templates.
 * 
 * @see Scalarf, Scalard
 */
template<typename Type>
class Scalar
{
public:
	/**
	 * @brief Default constructor is deleted – a Scalar must be initialised with a value
	 */	
	Scalar() = delete;

	/**
	 * @brief Construct a Scalar from a 1×1 matrix
	 * @param other 1×1 matrix whose single element becomes the scalar value
	 */
	Scalar(const Matrix<Type, 1, 1> &other) :
		_value{other(0, 0)}
	{
	}

	/**
	 * @brief Construct a Scalar directly from a primitive value
	 * @param other Primitive value (e.g., float, double, int)
	 */
	Scalar(Type other) : _value(other)
	{
	}

	/**
	 * @brief Implicit conversion to the underlying primitive type
	 * @return Reference to the stored constant value
	 */
	operator const Type &()
	{
		return _value;
	}

	/**
	 * @brief Convert to a 1×1 matrix
	 * @return Matrix<Type,1,1> containing the scalar value
	 */
	operator Matrix<Type, 1, 1>() const
	{
		Matrix<Type, 1, 1> m;
		m(0, 0) = _value;
		return m;
	}

	/**
	 * @brief Convert to a 1‑dimensional vector (column vector of size 1)
	 * @return Vector<Type,1> containing the scalar value
	 */
	operator Vector<Type, 1>() const
	{
		Vector<Type, 1> m;
		m(0) = _value;
		return m;
	}

	/**
	 * @brief Equality comparison with a float (or other primitive)
	 * @param other Right‑hand side value
	 * @return true if the wrapped value is equal to `other` within tolerance
	 * 
	 * @note Uses `isEqualF()` for tolerance‑based comparison, which handles NaN/Inf.
	 */
	bool operator==(const float other) const
	{
		return isEqualF(_value, other);
	}

private:
	const Type _value;		///< The wrapped constant scalar value

};

// -----------------------------------------------------------------------------
// Type aliases for common floating‑point types
// -----------------------------------------------------------------------------

/** @brief Scalar with float precision */
using Scalarf = Scalar<float>;

/** @brief Scalar with double precision */
using Scalard = Scalar<double>;

} // namespace matrix
