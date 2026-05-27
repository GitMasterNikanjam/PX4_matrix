/**
 * @file Vector2.hpp
 * @brief 2D vector class derived from Vector<Type, 2>
 * 
 * This file provides a 2-dimensional vector specialization of the Vector class.
 * It adds 2D-specific operations such as cross product (scalar result) and
 * convenient constructors from x,y coordinates.
 * 
 * Key features:
 * - Inherits all Vector<Type,2> operations (dot product, norm, etc.)
 * - Cross product for 2D vectors (returns scalar)
 * - Type aliases Vector2f and Vector2d for common usage
 * - Overloaded operators return Vector2 instead of base Matrix type
 * 
 * @author James Goppert <james.goppert@gmail.com>
 * @ingroup matrix
 */

// ############################################################################

#pragma once

#include "Vector.hpp"

namespace matrix
{

/**
 * @brief 2D column vector class
 * 
 * Vector2 is a specialized 2-element column vector. It extends Vector<Type,2>
 * with 2D-specific functionality, most notably the scalar cross product.
 * 
 * @tparam Type The numeric type (float, double, int, etc.)
 * 
 * @see Vector
 * @see Vector3
 */
template<typename Type>
class Vector2 : public Vector<Type, 2>
{
public:
	/** Base matrix type (2x1) */
	using Matrix21 = Matrix<Type, 2, 1>;

	/** 3D vector type (for conversion constructor) */
	using Vector3 = Vector<Type, 3>;

	/**
	 * @brief Default constructor – zero-initializes both components
	 */
	Vector2() = default;

	/**
	 * @brief Copy constructor from base 2x1 matrix
	 * @param other Source matrix (must be 2x1)
	 */
	Vector2(const Matrix21 &other) :
		Vector<Type, 2>(other)
	{
	}

	/**
	 * @brief Construct from a flat array of 2 elements
	 * @param data_ Array of length 2 [x, y]
	 */
	explicit Vector2(const Type data_[2]) :
		Vector<Type, 2>(data_)
	{
	}

	/**
	 * @brief Construct from x and y components
	 * @param x X component
	 * @param y Y component
	 */
	Vector2(Type x, Type y)
	{
		Vector2 &v(*this);
		v(0) = x;
		v(1) = y;
	}

	/** Inherit all constructors from base Vector<Type,2> */
	using base = Vector<Type, 2>;
	using base::base;

	/**
	 * @brief Construct a 2D vector from the first two components of a 3D vector
	 * @param other 3D vector (z component is ignored)
	 */
	explicit Vector2(const Vector3 &other)
	{
		Vector2 &v(*this);
		v(0) = other(0);
		v(1) = other(1);
	}

	/**
	 * @brief 2D cross product (scalar)
	 * 
	 * Computes the signed area of the parallelogram formed by this vector and b.
	 * For 2D vectors, cross(a,b) = a.x * b.y - a.y * b.x.
	 * 
	 * @param b Right-hand side vector
	 * @return Scalar cross product (positive if b is counter-clockwise from a)
	 */
	Type cross(const Matrix21 &b) const
	{
		const Vector2 &a(*this);
		return a(0) * b(1, 0) - a(1) * b(0, 0);
	}

	/**
	 * Override matrix ops so Vector2 type is returned
	 */

	 /**
	 * @brief Override matrix addition to return Vector2
	 * @param other Right-hand side vector
	 * @return New Vector2 with element-wise sum
	 */
	Vector2 operator+(Vector2 other) const
	{
		return Matrix21::operator+(other);
	}

	/**
	 * @brief Override scalar addition to return Vector2
	 * @param scalar Value to add to each component
	 * @return New Vector2 with each component increased by scalar
	 */	
	Vector2 operator+(Type scalar) const
	{
		return Matrix21::operator+(scalar);
	}

	/**
	 * @brief Override matrix subtraction to return Vector2
	 * @param other Right-hand side vector
	 * @return New Vector2 with element-wise difference
	 */
	Vector2 operator-(Vector2 other) const
	{
		return Matrix21::operator-(other);
	}

	/**
	 * @brief Override scalar subtraction to return Vector2
	 * @param scalar Value to subtract from each component
	 * @return New Vector2 with each component reduced by scalar
	 */
	Vector2 operator-(Type scalar) const
	{
		return Matrix21::operator-(scalar);
	}

	/**
	 * @brief Unary negation
	 * @return Vector2 with both components negated
	 */
	Vector2 operator-() const
	{
		return Matrix21::operator-();
	}

	/**
	 * @brief Scalar multiplication
	 * @param scalar Multiplier
	 * @return New Vector2 with each component multiplied by scalar
	 */
	Vector2 operator*(Type scalar) const
	{
		return Matrix21::operator*(scalar);
	}

	/**
	 * @brief Dot product with another Vector2
	 * @param b Right-hand side vector
	 * @return Scalar dot product
	 */
	Type operator*(Vector2 b) const
	{
		return Vector<Type, 2>::operator*(b);
	}

	/**
	 * @brief Cross product operator (alternative syntax)
	 * @param b Right-hand side vector
	 * @return Scalar cross product
	 * 
	 * @code
	 * Vector2f a(1,0), b(0,1);
	 * float cross = a % b;  // result = 1
	 * @endcode
	 */
	Type operator%(const Matrix21 &b) const
	{
		return (*this).cross(b);
	}

};

// -----------------------------------------------------------------------------
// Common type aliases
// -----------------------------------------------------------------------------

/** @brief 2D vector with float components */
using Vector2f = Vector2<float>;

/** @brief 2D vector with double components */
using Vector2d = Vector2<double>;

} // namespace matrix
