/**
 * @file Vector3.hpp
 * @brief 3D vector class derived from Vector<Type, 3>
 * 
 * This file provides a 3-dimensional vector specialization of the Vector class.
 * It adds 3D-specific operations such as cross product (returning a vector),
 * skew‑symmetric (hat) matrix for cross‑product linearization, and a slice
 * to extract the xy components.
 * 
 * Key features:
 * - Inherits all Vector<Type,3> operations (dot product, norm, etc.)
 * - Cross product returns a Vector3 (true 3D cross product)
 * - hat() creates a skew‑symmetric matrix (so that a×b = hat(a) * b)
 * - xy() slice to access the first two components as a Vector2 view
 * - Overloaded operators return Vector3 instead of base Matrix type
 * - Type aliases Vector3f and Vector3d for common usage
 * 
 * @author James Goppert <james.goppert@gmail.com>
 * @ingroup matrix
 */

// ########################################################################
#pragma once

#include "Vector.hpp"

// #########################################################################

namespace matrix
{

// Forward declaration for Dcm (Direction Cosine Matrix)
template <typename Type>
class Dcm;

/**
 * @brief 3D column vector class
 * 
 * Vector3 is a specialized 3-element column vector. It extends Vector<Type,3>
 * with 3D‑specific functionality:
 * - Cross product that returns a vector (unlike 2D where cross returns a scalar).
 * - Hat operator that returns a skew‑symmetric matrix, useful for linearising
 *   cross products and for Lie algebra operations.
 * - xy() slice that provides a view of the first two components as a Vector2.
 * 
 * @tparam Type The numeric type (float, double, int, etc.)
 * 
 * @see Vector
 * @see Vector2
 * @see Dcm
 */
template<typename Type>
class Vector3 : public Vector<Type, 3>
{
public:

	/** Base matrix type (3x1) */
	using Matrix31 = Matrix<Type, 3, 1>;

	/**
	 * @brief Default constructor – zero-initialises all components
	 */
	Vector3() = default;

	/**
	 * @brief Copy constructor from base 3x1 matrix
	 * @param other Source matrix (must be 3x1)
	 */
	Vector3(const Matrix31 &other) :
		Vector<Type, 3>(other)
	{
	}

	/**
	 * @brief Construct from a flat array of 3 elements
	 * @param data_ Array of length 3 [x, y, z]
	 */
	explicit Vector3(const Type data_[3]) :
		Vector<Type, 3>(data_)
	{
	}

	/**
	 * @brief Construct from x, y, z components
	 * @param x X component
	 * @param y Y component
	 * @param z Z component
	 */
	Vector3(Type x, Type y, Type z)
	{
		Vector3 &v(*this);
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	/** Inherit all constructors from base Vector<Type,3> */
	using base = Vector<Type, 3>;
	using base::base;

	/**
	 * @brief 3D cross product
	 * 
	 * Computes the vector cross product: a × b.
	 * For 3D vectors, the result is a vector perpendicular to both a and b,
	 * with magnitude ||a||·||b||·sin(θ).
	 * 
	 * @param b Right-hand side vector
	 * @return Vector3 result of the cross product
	 * 
	 * @code
	 * Vector3f a(1,0,0), b(0,1,0);
	 * Vector3f c = a.cross(b);  // (0,0,1)
	 * @endcode
	 */
	Vector3 cross(const Matrix31 &b) const
	{
		const Vector3 &a(*this);
		return {a(1) *b(2, 0) - a(2) *b(1, 0), -a(0) *b(2, 0) + a(2) *b(0, 0), a(0) *b(1, 0) - a(1) *b(0, 0)};
	}

	// ------------------------------------------------------------------------
	// Overloaded operators – return Vector3 instead of Matrix31
	// ------------------------------------------------------------------------

	/**
	 * Override matrix ops so Vector3 type is returned
	 */

	/**
	 * @brief Vector addition (element‑wise)
	 * @param other Right-hand side vector
	 * @return New Vector3 with element‑wise sum
	 */
	inline Vector3 operator+(Vector3 other) const
	{
		return Matrix31::operator+(other);
	}

	/**
	 * @brief Scalar addition (adds scalar to each component)
	 * @param scalar Value to add
	 * @return New Vector3 with each component increased by scalar
	 */
	inline Vector3 operator+(Type scalar) const
	{
		return Matrix31::operator+(scalar);
	}

	/**
	 * @brief Vector subtraction (element‑wise)
	 * @param other Right-hand side vector
	 * @return New Vector3 with element‑wise difference
	 */
	inline Vector3 operator-(Vector3 other) const
	{
		return Matrix31::operator-(other);
	}

	/**
	 * @brief Scalar subtraction (subtracts scalar from each component)
	 * @param scalar Value to subtract
	 * @return New Vector3 with each component reduced by scalar
	 */
	inline Vector3 operator-(Type scalar) const
	{
		return Matrix31::operator-(scalar);
	}

	/**
	 * @brief Unary negation
	 * @return Vector3 with all components negated
	 */
	inline Vector3 operator-() const
	{
		return Matrix31::operator-();
	}

	/**
	 * @brief Scalar multiplication
	 * @param scalar Multiplier
	 * @return New Vector3 with each component multiplied by scalar
	 */
	inline Vector3 operator*(Type scalar) const
	{
		return Matrix31::operator*(scalar);
	}

	/**
	 * @brief Dot product with another Vector3
	 * @param b Right-hand side vector
	 * @return Scalar dot product
	 */
	inline Type operator*(Vector3 b) const
	{
		return Vector<Type, 3>::operator*(b);
	}

	/**
	 * @brief Cross product operator (alternative syntax)
	 * @param b Right-hand side vector
	 * @return Vector3 cross product
	 * 
	 * @code
	 * Vector3f a(1,0,0), b(0,1,0);
	 * Vector3f c = a % b;  // (0,0,1)
	 * @endcode
	 */
	inline Vector3 operator%(const Matrix31 &b) const
	{
		return (*this).cross(b);
	}

	// ------------------------------------------------------------------------
	// Slicing and linear algebra utilities
	// ------------------------------------------------------------------------

	/**
	 * @brief Extract the xy components as a 2D vector view (const)
	 * @return Const slice of the first two rows as a 2×1 vector
	 * 
	 * @note This is a view, not a copy. Modifying the original vector
	 *       will affect the view and vice versa.
	 */
	ConstSlice<Type, 2, 1, 3, 1> xy() const
	{
		return {0, 0, this};
	}

	/**
	 * @brief Extract the xy components as a 2D vector view (mutable)
	 * @return Mutable slice of the first two rows as a 2×1 vector
	 */
	Slice<Type, 2, 1, 3, 1> xy()
	{
		return {0, 0, this};
	}

	/**
	 * @brief Skew‑symmetric “hat” matrix
	 * 
	 * Creates the 3×3 skew‑symmetric matrix corresponding to this vector,
	 * such that for any vector v, `hat(a) * v = a × v`.
	 * 
	 * This is useful for:
	 * - Linearising cross products
	 * - Representing the Lie algebra so(3)
	 * - Converting angular velocity to a matrix for time derivatives of rotation matrices
	 * 
	 * @return Dcm<Type> (3×3 matrix) – the hat matrix of this vector
	 * 
	 * @code
	 * Vector3f omega(0.1f, 0.2f, 0.3f);
	 * auto Omega = omega.hat();   // skew‑symmetric matrix
	 * // Then Omega * v == omega.cross(v)
	 * @endcode
	 * 
	 * @see Dcm::vee() for the inverse operation
	 */
	Dcm<Type> hat() const      // inverse to Dcm.vee() operation
	{
		const Vector3 &v(*this);
		Dcm<Type> A;
		A(0, 0) = 0;
		A(0, 1) = -v(2);
		A(0, 2) = v(1);
		A(1, 0) = v(2);
		A(1, 1) = 0;
		A(1, 2) = -v(0);
		A(2, 0) = -v(1);
		A(2, 1) = v(0);
		A(2, 2) = 0;
		return A;
	}

};

// -----------------------------------------------------------------------------
// Common type aliases
// -----------------------------------------------------------------------------

/** @brief 3D vector with float components */
using Vector3f = Vector3<float>;

/** @brief 3D vector with double components */
using Vector3d = Vector3<double>;

} // namespace matrix
