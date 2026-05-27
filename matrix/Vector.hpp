/**
 * @file Vector.hpp
 * @brief Column vector class derived from Matrix<Type, M, 1>
 * 
 * This file provides a column vector specialization of the Matrix class.
 * Vectors are represented as M×1 matrices and inherit all matrix operations,
 * with additional convenience functions specific to vectors (dot product,
 * norm, normalization, etc.).
 * 
 * Key features:
 * - Column vector of compile-time size M
 * - Inherits all Matrix operations (addition, scalar multiplication, etc.)
 * - Vector-specific methods: dot(), norm(), normalize(), unit()
 * - Conversion from row slices (1×M) to column vectors
 * - Stream output as a row vector for compact display
 * 
 * @author James Goppert <james.goppert@gmail.com>
 * @ingroup matrix
 */

// #############################################################################
#pragma once

// #############################################################################

#include "Matrix.hpp"

// #############################################################################

namespace matrix
{

/**
 * @brief Column vector class of fixed size M
 * 
 * Vector is a column vector (M rows, 1 column) that publicly inherits from
 * Matrix<Type, M, 1>. It provides all matrix operations plus vector-specific
 * convenience functions.
 * 
 * Vectors can be used in matrix expressions, e.g.:
 * @code
 * Vector<float,3> v1, v2;
 * float dot = v1.dot(v2);
 * auto v3 = v1 + v2;
 * auto scaled = v1 * 2.0f;
 * @endcode
 * 
 * @tparam Type The numeric type (float, double, int, etc.)
 * @tparam M Number of rows (vector dimension)
 * 
 * @note The base class Matrix<Type, M, 1> provides element access via (i,0)
 *       but Vector adds a convenience operator()(size_t i) for the single column.
 * 
 * @see Matrix
 */
template<typename Type, size_t M>
class Vector : public Matrix<Type, M, 1>
{
public:
	/** Base type alias for convenience */
	using MatrixM1 = Matrix<Type, M, 1>;

	/**
	 * @brief Default constructor – zero-initializes all elements
	 */
	Vector() = default;

	/**
	 * @brief Copy constructor from base matrix
	 * @param other Source matrix (must be M×1)
	 */
	Vector(const MatrixM1 &other) :
		MatrixM1(other)
	{
	}

	/**
	 * @brief Construct from a flat array of M elements
	 * @param data_ Array of length M, stored in row order (only one column)
	 */
	explicit Vector(const Type data_[M]) :
		MatrixM1(data_)
	{
	}

	/**
	 * @brief Construct from a mutable column slice (M×1)
	 * @tparam P Number of rows in source slice (must be M)
	 * @tparam Q Number of columns in source slice (must be 1)
	 * @param slice_in Source slice view
	 */
	template<size_t P, size_t Q>
	Vector(const Slice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	/**
	 * @brief Construct from a mutable row slice (1×M) – convert to column vector
	 * @tparam P Number of rows in source slice (must be 1)
	 * @tparam Q Number of columns in source slice (must be M)
	 * @param slice_in Source row slice
	 * 
	 * @note The dummy template parameter is used to enable this constructor
	 *       only when the slice dimensions are 1×M.
	 */
	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(const Slice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*this);

		for (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	/**
	 * @brief Construct from a const column slice (M×1)
	 * @tparam P Number of rows in source slice (must be M)
	 * @tparam Q Number of columns in source slice (must be 1)
	 * @param slice_in Source const slice view
	 */
	template<size_t P, size_t Q>
	Vector(const ConstSlice<Type, M, 1, P, Q> &slice_in) :
		Matrix<Type, M, 1>(slice_in)
	{
	}

	/**
	 * @brief Construct from a const row slice (1×M) – convert to column vector
	 * @tparam P Number of rows in source slice (must be 1)
	 * @tparam Q Number of columns in source slice (must be M)
	 * @param slice_in Source const row slice
	 */
	template<size_t P, size_t Q, size_t DUMMY = 1>
	Vector(const ConstSlice<Type, 1, M, P, Q> &slice_in)
	{
		Vector &self(*this);

		for (size_t i = 0; i < M; i++) {
			self(i) = slice_in(0, i);
		}
	}

	/**
	 * @brief Read-only element access (single index)
	 * @param i Row index (0 <= i < M)
	 * @return Const reference to the i-th element
	 * 
	 * @pre i < M
	 */
	inline const Type &operator()(size_t i) const
	{
		assert(i < M);

		const MatrixM1 &v = *this;
		return v(i, 0);
	}

	/**
	 * @brief Mutable element access (single index)
	 * @param i Row index (0 <= i < M)
	 * @return Mutable reference to the i-th element
	 * 
	 * @pre i < M
	 */
	inline Type &operator()(size_t i)
	{
		assert(i < M);

		MatrixM1 &v = *this;
		return v(i, 0);
	}

	/**
	 * @brief Dot product with another column vector
	 * @param b Right-hand side vector (same size)
	 * @return Scalar dot product
	 */
	Type dot(const MatrixM1 &b) const
	{
		const Vector &a(*this);
		Type r(0);

		for (size_t i = 0; i < M; i++) {
			r += a(i) * b(i, 0);
		}

		return r;
	}

	/**
	 * @brief Dot product via operator* (convenience)
	 * @param b Right-hand side vector
	 * @return Scalar dot product
	 */
	inline Type operator*(const MatrixM1 &b) const
	{
		const Vector &a(*this);
		return a.dot(b);
	}

	/**
	 * @brief Scalar multiplication (returns a new vector)
	 * @param b Scalar multiplier
	 * @return New vector with each element multiplied by b
	 */
	inline Vector operator*(Type b) const
	{
		return Vector(MatrixM1::operator*(b));
	}

	/**
	 * @brief Euclidean norm (L2 norm)
	 * @return sqrt(sum of squares)
	 */
	Type norm() const
	{
		const Vector &a(*this);
		return Type(std::sqrt(a.dot(a)));
	}

	/**
	 * @brief Squared Euclidean norm
	 * @return sum of squares (avoids sqrt)
	 */
	Type norm_squared() const
	{
		const Vector &a(*this);
		return a.dot(a);
	}

	/**
	 * @brief Alias for norm()
	 * @return Euclidean norm
	 */
	inline Type length() const
	{
		return norm();
	}

	/**
	 * @brief Normalize the vector in-place
	 * 
	 * Divides each element by the current norm.
	 * @pre norm() != 0
	 */
	inline void normalize()
	{
		(*this) /= norm();
	}

	/**
	 * @brief Return a unit vector (normalized copy)
	 * @return New vector with norm = 1
	 * @pre norm() != 0
	 */
	Vector unit() const
	{
		return (*this) / norm();
	}

	/**
	 * @brief Return a unit vector, or zero vector if norm is below tolerance
	 * @param eps Tolerance threshold (default 1e-5)
	 * @return Normalized vector if norm > eps, otherwise zero vector
	 */
	Vector unit_or_zero(const Type eps = Type(1e-5)) const
	{
		const Type n = norm();

		if (n > eps) {
			return (*this) / n;
		}

		return Vector();
	}

	/**
	 * @brief Alias for unit()
	 * @return Normalized copy
	 */
	inline Vector normalized() const
	{
		return unit();
	}

	/**
	 * @brief Check if the norm exceeds a threshold
	 * @param testVal Threshold value
	 * @return true if norm > testVal
	 * 
	 * @note More efficient than computing the full norm because it compares
	 *       squared norm against squared threshold.
	 */
	bool longerThan(Type testVal) const
	{
		return norm_squared() > testVal * testVal;
	}

	/**
	 * @brief Element-wise square root
	 * @return New vector with sqrt of each element
	 */
	Vector sqrt() const
	{
		const Vector &a(*this);
		Vector r;

		for (size_t i = 0; i < M; i++) {
			r(i) = Type(std::sqrt(a(i)));
		}

		return r;
	}

	/**
	 * @brief Print the vector as a row (transposed)
	 * 
	 * Outputs the vector in a single line, e.g.: [1, 2, 3]
	 */
	void print() const
	{
		(*this).transpose().print();
	}

	/**
	 * @brief Compile-time size of the vector
	 * @return M (number of elements)
	 */
	static size_t size()
	{
		return M;
	}
};

/**
 * @brief Stream output operator for Vector
 * 
 * Prints the vector as a row (transposed) for compact display.
 * 
 * @tparam OStream Output stream type (e.g., std::ostream)
 * @tparam Type Element type
 * @tparam M Vector dimension
 * @param os Output stream
 * @param vector Vector to output
 * @return Reference to the output stream (for chaining)
 * 
 * @example
 * @code
 * Vector<float,3> v;
 * v(0)=1; v(1)=2; v(2)=3;
 * std::cout << v << std::endl; // prints: [1, 2, 3]
 * @endcode
 */
template<typename OStream, typename Type, size_t M>
OStream &operator<<(OStream &os, const matrix::Vector<Type, M> &vector)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	static const size_t n = 15 * M * 1 + 1 + 1;
	char string[n];
	vector.transpose().write_string(string, n);
	os << string;
	return os;
}

} // namespace matrix
