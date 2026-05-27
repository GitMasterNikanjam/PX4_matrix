/**
 * @file Slice.hpp
 * @brief Matrix slice/view template class for non-owning submatrix access
 * 
 * This file provides a slice/view mechanism for the matrix library, allowing
 * efficient submatrix operations without copying data. Slices provide a
 * window into a larger matrix and support most matrix operations.
 * 
 * @author Julian Kent <julian@auterion.com>
 * @ingroup matrix
 */

// ###########################################################################

#pragma once

// ###########################################################################
// Iclude Libraries

#include <cassert>   // Runtime bounds checking (debug builds only)
#include <cstdio>    // Standard I/O (reserved for debugging/output)
#include <cmath>     // Mathematical functions (sqrt, etc.)

// ###########################################################################

namespace matrix
{

// Forward declarations
template<typename Type, size_t M, size_t N>
class Matrix;

template<typename Type, size_t M>
class Vector;

/**
 * @brief Template class for matrix slice/view operations
 * 
 * SliceT provides a non-owning view into a matrix, allowing operations on
 * submatrices without copying data. This is useful for:
 * - Extracting submatrices
 * - Modifying portions of matrices in-place
 * - Creating efficient aliases for matrix regions
 * 
 * @tparam MatrixT The type of the underlying matrix (const or non-const)
 * @tparam Type The element type (float, double, int, etc.)
 * @tparam P Number of rows in the slice (compile-time constant)
 * @tparam Q Number of columns in the slice (compile-time constant)
 * @tparam M Number of rows in the backing matrix
 * @tparam N Number of columns in the backing matrix
 * 
 * @note The slice does NOT own the underlying data; it holds a pointer
 *       to the parent matrix. Ensure the parent matrix outlives any slices.
 * 
 * @see Matrix
 * @see Slice for mutable slice type alias
 * @see ConstSlice for const slice type alias
 */
template <typename MatrixT, typename Type, size_t P, size_t Q, size_t M, size_t N>
class SliceT
{
public:
	/** Self type alias for convenience */
	using Self = SliceT<MatrixT, Type, P, Q, M, N>;

	/**
	 * @brief Construct a slice from a parent matrix
	 * 
	 * Creates a view into the parent matrix starting at position (x0, y0)
	 * with dimensions P x Q.
	 * 
	 * @param x0 Starting row index in the parent matrix (inclusive)
	 * @param y0 Starting column index in the parent matrix (inclusive)
	 * @param data Pointer to the parent matrix
	 * 
	 * @pre x0 + P <= M (slice fits within rows)
	 * @pre y0 + Q <= N (slice fits within columns)
	 * 
	 * @warning No bounds checking beyond debug assertions
	 */
	SliceT(size_t x0, size_t y0, MatrixT *data) :
		_x0(x0),
		_y0(y0),
		_data(data)
	{
		static_assert(P <= M, "Slice rows bigger than backing matrix");
		static_assert(Q <= N, "Slice cols bigger than backing matrix");
		assert(x0 + P <= M);
		assert(y0 + Q <= N);
	}

	/** @brief Default copy constructor */
	SliceT(const Self &other) = default;

	/**
	 * @brief Access element at (i, j) in the slice (const version)
	 * 
	 * @param i Row index within the slice (0 <= i < P)
	 * @param j Column index within the slice (0 <= j < Q)
	 * @return Const reference to the element
	 * 
	 * @pre i < P && j < Q
	 */
	const Type &operator()(size_t i, size_t j) const
	{
		assert(i < P);
		assert(j < Q);

		return (*_data)(_x0 + i, _y0 + j);
	}

	/**
	 * @brief Access element at (i, j) in the slice (mutable version)
	 * 
	 * @param i Row index within the slice (0 <= i < P)
	 * @param j Column index within the slice (0 <= j < Q)
	 * @return Mutable reference to the element
	 * 
	 * @pre i < P && j < Q
	 */
	Type &operator()(size_t i, size_t j)
	{
		assert(i < P);
		assert(j < Q);

		return (*_data)(_x0 + i, _y0 + j);
	}

	/**
	 * @brief Copy assignment from another slice
	 * 
	 * Performs element-wise deep copy from another slice.
	 * 
	 * @param other Source slice to copy from
	 * @return Reference to this slice after assignment
	 * 
	 * @note Handles self-assignment correctly
	 */
	// Separate function needed otherwise the default copy constructor matches before the deep copy implementation
	Self &operator=(const Self &other)
	{
		// Protect against self-assignment
		if (this == &other) {
			return *this;
		}

		return this->operator=<M, N>(other);
	}

	/**
	 * @brief Copy assignment from a mutable matrix slice
	 * 
	 * @tparam MM Number of rows in source matrix
	 * @tparam NN Number of columns in source matrix
	 * @param other Source slice to copy from
	 * @return Reference to this slice after assignment
	 */
	template<size_t MM, size_t NN>
	Self &operator=(const SliceT<Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		Self &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		return self;
	}

	/**
	 * @brief Copy assignment from a const matrix slice
	 * 
	 * @tparam MM Number of rows in source matrix
	 * @tparam NN Number of columns in source matrix
	 * @param other Source slice to copy from
	 * @return Reference to this slice after assignment
	 */
	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(const SliceT<const Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		return self;
	}

	/**
	 * @brief Assign a full matrix to this slice
	 * 
	 * Copies all elements from a matrix of matching dimensions into the slice.
	 * 
	 * @param other Source matrix
	 * @return Reference to this slice after assignment
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(const Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) = other(i, j);
			}
		}

		return self;
	}

	/**
	 * @brief Assign a scalar value to all elements in the slice
	 * 
	 * @param other Scalar value to assign to every element
	 * @return Reference to this slice after assignment
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) = other;
			}
		}

		return self;
	}

	/**
	 * @brief Matrix subtraction with another slice
	 * 
	 * @tparam MM Number of rows in other matrix
	 * @tparam NN Number of columns in other matrix
	 * @param other Right-hand side slice
	 * @return New matrix containing element-wise difference
	 */
	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator-(const SliceT<const Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		return Matrix<Type, P, Q> {*this} - other;
	}

	/**
	 * @brief Matrix subtraction with another matrix
	 * 
	 * @param other Right-hand side matrix
	 * @return New matrix containing element-wise difference
	 */
	Matrix<Type, P, Q> operator-(const Matrix<Type, P, Q> &other)
	{
		return Matrix<Type, P, Q> {*this} - other;
	}

	/**
	 * @brief Scalar subtraction
	 * 
	 * @param other Scalar value to subtract from each element
	 * @return New matrix with each element reduced by scalar
	 */
	Matrix<Type, P, Q> operator-(const Type &other)
	{
		return Matrix<Type, P, Q> {*this} - other;
	}

	/**
	 * @brief Matrix addition with another slice
	 * 
	 * @tparam MM Number of rows in other matrix
	 * @tparam NN Number of columns in other matrix
	 * @param other Right-hand side slice
	 * @return New matrix containing element-wise sum
	 */
	template<size_t MM, size_t NN>
	Matrix<Type, P, Q> operator+(const SliceT<const Matrix<Type, MM, NN>, Type, P, Q, MM, NN> &other)
	{
		return Matrix<Type, P, Q> {*this} + other;
	}

	/**
	 * @brief Matrix addition with another matrix
	 * 
	 * @param other Right-hand side matrix
	 * @return New matrix containing element-wise sum
	 */
	Matrix<Type, P, Q> operator+(const Matrix<Type, P, Q> &other)
	{
		return Matrix<Type, P, Q> {*this} + other;
	}

	/**
	 * @brief Scalar addition
	 * 
	 * @param other Scalar value to add to each element
	 * @return New matrix with each element increased by scalar
	 */
	Matrix<Type, P, Q> operator+(const Type &other)
	{
		return Matrix<Type, P, Q> {*this} + other;
	}

	/**
	 * @brief Assign a vector to a row slice
	 * 
	 * This operator is only available when the slice has exactly one row (P == 1).
	 * 
	 * @tparam DUMMY Template parameter to enable conditional instantiation
	 * @param other Source vector
	 * @return Reference to this slice after assignment
	 */
	// allow assigning vectors to a slice that are in the axis
	template <size_t DUMMY = 1> // make this a template function since it only exists for some instantiations
	SliceT<MatrixT, Type, 1, Q, M, N> &operator=(const Vector<Type, Q> &other)
	{
		SliceT<MatrixT, Type, 1, Q, M, N> &self = *this;

		for (size_t j = 0; j < Q; j++) {
			self(0, j) = other(j);
		}

		return self;
	}

	/**
	 * @brief Compound addition assignment from another slice
	 * 
	 * @tparam MM Number of rows in other matrix
	 * @tparam NN Number of columns in other matrix
	 * @param other Right-hand side slice
	 * @return Reference to this slice after addition
	 */
	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(const SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		return self;
	}

	/**
	 * @brief Compound addition assignment from a matrix
	 * 
	 * @param other Right-hand side matrix
	 * @return Reference to this slice after addition
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(const Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) += other(i, j);
			}
		}

		return self;
	}

	/**
	 * @brief Compound addition assignment of a scalar
	 * 
	 * Adds the same scalar value to every element in the slice.
	 * 
	 * @param other Scalar value to add
	 * @return Reference to this slice after addition
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator+=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) += other;
			}
		}

		return self;
	}

	/**
	 * @brief Compound subtraction assignment from another slice
	 * 
	 * @tparam MM Number of rows in other matrix
	 * @tparam NN Number of columns in other matrix
	 * @param other Right-hand side slice
	 * @return Reference to this slice after subtraction
	 */
	template<size_t MM, size_t NN>
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(const SliceT<MatrixT, Type, P, Q, MM, NN> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		return self;
	}

	/**
	 * @brief Compound subtraction assignment from a matrix
	 * 
	 * @param other Right-hand side matrix
	 * @return Reference to this slice after subtraction
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(const Matrix<Type, P, Q> &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) -= other(i, j);
			}
		}

		return self;
	}

	/**
	 * @brief Compound subtraction assignment of a scalar
	 * 
	 * Subtracts the same scalar value from every element in the slice.
	 * 
	 * @param other Scalar value to subtract
	 * @return Reference to this slice after subtraction
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator-=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) -= other;
			}
		}

		return self;
	}

	/**
	 * @brief Scalar multiplication assignment
	 * 
	 * Multiplies every element in the slice by a scalar value.
	 * 
	 * @param other Scalar multiplier
	 * @return Reference to this slice after multiplication
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator*=(const Type &other)
	{
		SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				self(i, j) *= other;
			}
		}

		return self;
	}

	/**
	 * @brief Scalar division assignment
	 * 
	 * Divides every element in the slice by a scalar value.
	 * 
	 * @param scalar Scalar divisor (must be non-zero)
	 * @return Reference to this slice after division
	 * 
	 * @pre scalar != 0
	 */
	SliceT<MatrixT, Type, P, Q, M, N> &operator/=(const Type &scalar)
	{
		return operator*=(Type(1) / scalar);
	}

	/**
	 * @brief Scalar multiplication
	 * 
	 * @param scalar Scalar multiplier
	 * @return New matrix with each element multiplied by scalar
	 */
	Matrix<Type, P, Q> operator*(const Type &scalar) const
	{
		return Matrix<Type, P, Q> {*this} * scalar;
	}

	/**
	 * @brief Scalar division
	 * 
	 * @param scalar Scalar divisor (must be non-zero)
	 * @return New matrix with each element divided by scalar
	 * 
	 * @pre scalar != 0
	 */
	Matrix<Type, P, Q> operator/(const Type &scalar) const
	{
		return (*this) * (1 / scalar);
	}

	/**
	 * @brief Create a sub-slice of this slice (const version)
	 * 
	 * @tparam R Number of rows in the sub-slice
	 * @tparam S Number of columns in the sub-slice
	 * @param x0 Starting row offset within this slice
	 * @param y0 Starting column offset within this slice
	 * @return New const slice representing the submatrix
	 */
	template<size_t R, size_t S>
	const SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0) const
	{
		return SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	/**
	 * @brief Create a sub-slice of this slice (mutable version)
	 * 
	 * @tparam R Number of rows in the sub-slice
	 * @tparam S Number of columns in the sub-slice
	 * @param x0 Starting row offset within this slice
	 * @param y0 Starting column offset within this slice
	 * @return New mutable slice representing the submatrix
	 */
	template<size_t R, size_t S>
	SliceT<MatrixT, Type, R, S, M, N> slice(size_t x0, size_t y0)
	{
		return SliceT<MatrixT, Type, R, S, M, N>(x0 + _x0, y0 + _y0, _data);
	}

	/**
	 * @brief Copy slice to a row-major C array
	 * 
	 * @param dst Destination array of size P * Q
	 * 
	 * @pre dst is pre-allocated with at least P*Q elements
	 */
	void copyTo(Type dst[P * Q]) const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				dst[i * N + j] = self(i, j);
			}
		}
	}

	/**
	 * @brief Copy slice to a column-major C array
	 * 
	 * @param dst Destination array of size P * Q
	 * 
	 * @pre dst is pre-allocated with at least P*Q elements
	 */
	void copyToColumnMajor(Type dst[P * Q]) const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	/**
	 * @brief Extract diagonal elements
	 * 
	 * @return Vector containing diagonal elements up to min(P, Q)
	 */
	Vector < Type, P < Q ? P : Q > diag() const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;
		Vector < Type, P < Q ? P : Q > res;

		for (size_t j = 0; j < (P < Q ? P : Q); j++) {
			res(j) = self(j, j);
		}

		return res;
	}

	/**
	 * @brief Compute squared Frobenius norm
	 * 
	 * @return Sum of squares of all elements
	 */
	Type norm_squared() const
	{
		const SliceT<MatrixT, Type, P, Q, M, N> &self = *this;
		Type accum(0);

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				accum += self(i, j) * self(i, j);
			}
		}

		return accum;
	}

	/**
	 * @brief Compute Frobenius norm
	 * 
	 * @return Square root of sum of squares of all elements
	 */
	Type norm() const
	{
		return std::sqrt(norm_squared());
	}

	/**
	 * @brief Check if norm exceeds a threshold
	 * 
	 * More efficient than computing full norm for comparison.
	 * 
	 * @param testVal Threshold value
	 * @return true if norm > testVal, false otherwise
	 */
	bool longerThan(Type testVal) const
	{
		return norm_squared() > testVal * testVal;
	}

	/**
	 * @brief Find maximum element value
	 * 
	 * @return Maximum value in the slice
	 */
	Type max() const
	{
		Type max_val = (*this)(0, 0);

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				Type val = (*this)(i, j);

				if (val > max_val) {
					max_val = val;
				}
			}
		}

		return max_val;
	}

	/**
	 * @brief Find minimum element value
	 * 
	 * @return Minimum value in the slice
	 */
	Type min() const
	{
		Type min_val = (*this)(0, 0);

		for (size_t i = 0; i < P; i++) {
			for (size_t j = 0; j < Q; j++) {
				Type val = (*this)(i, j);

				if (val < min_val) {
					min_val = val;
				}
			}
		}

		return min_val;
	}

private:
	size_t _x0, _y0;
	MatrixT *_data;
};

/**
 * @brief Mutable slice type alias
 * 
 * Creates a mutable view into a non-const matrix.
 * 
 * @tparam Type The element type
 * @tparam P Number of rows in the slice
 * @tparam Q Number of columns in the slice
 * @tparam M Number of rows in the backing matrix
 * @tparam N Number of columns in the backing matrix
 */
template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using Slice = SliceT<Matrix<Type, M, N>, Type, P, Q, M, N>;

/**
 * @brief Const slice type alias
 * 
 * Creates a read-only view into a const matrix.
 * 
 * @tparam Type The element type
 * @tparam P Number of rows in the slice
 * @tparam Q Number of columns in the slice
 * @tparam M Number of rows in the backing matrix
 * @tparam N Number of columns in the backing matrix
 */
template <typename Type, size_t P, size_t Q, size_t M, size_t N>
using ConstSlice = SliceT<const Matrix<Type, M, N>, Type, P, Q, M, N>;

}
