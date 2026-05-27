/**
 * @file SquareMatrix.hpp
 * @brief Square matrix class with specialized operations for symmetric and covariance matrices
 * 
 * This file provides a template class for square matrices (M×M) derived from Matrix<Type, M, M>.
 * It adds square‑matrix‑specific functionality including:
 * - Matrix inversion (LU decomposition with partial pivoting, plus optimized 2x2 and 3x3 versions)
 * - Cholesky decomposition and Cholesky inversion for positive‑definite matrices
 * - Covariance matrix utilities (uncorrelate blocks, make symmetric, copy triangles)
 * - Diagonal extraction, upper‑right triangle packing, partial trace
 * - Matrix exponential (expm) approximation
 * 
 * The file also provides global functions for inversion, Cholesky, identity, and diagonal matrix creation.
 * 
 * @author James Goppert <james.goppert@gmail.com>
 * @ingroup matrix
 */

/****************************************************************************
 * Copyright (C) PX4 Development Team. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted under the terms of the BSD 3-Clause License.
 * See the file LICENSE for details.
 ****************************************************************************/

#pragma once

#include <float.h>          // FLT_EPSILON for floating‑point tolerance
#include "Slice.hpp"        // Matrix slice views

namespace matrix
{

/**
 * @brief Square matrix class with fixed size M × M
 * 
 * SquareMatrix<Type, M> inherits from Matrix<Type, M, M> and adds methods
 * that are specific to square matrices, such as inversion, trace,
 * Cholesky decomposition, covariance handling, and matrix exponential.
 * 
 * @tparam Type The element type (float, double, etc.)
 * @tparam M Number of rows and columns (compile‑time constant)
 * 
 * @see Matrix
 */
template <typename Type, size_t  M>
class SquareMatrix : public Matrix<Type, M, M>
{
public:
	/**
	 * @brief Default constructor – zero‑initialises the matrix
	 */
	SquareMatrix() = default;

	/**
	 * @brief Construct from a 2D C array of size M×M
	 * @param data_ 2D array (row‑major)
	 */
	explicit SquareMatrix(const Type data_[M][M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	/**
	 * @brief Construct from a flat row‑major array of size M×M
	 * @param data_ Flat array of length M*M
	 */
	explicit SquareMatrix(const Type data_[M * M]) :
		Matrix<Type, M, M>(data_)
	{
	}

	/**
	 * @brief Copy constructor from a generic M×M matrix
	 * @param other Source matrix
	 */
	SquareMatrix(const Matrix<Type, M, M> &other) :
		Matrix<Type, M, M>(other)
	{
	}

	/** Inherit all constructors from the base Matrix class */
	using base = Matrix<Type, M, M>;
	using base::base;

	/**
	 * @brief Assignment from a generic M×M matrix
	 * @param other Source matrix
	 * @return Reference to this square matrix
	 */
	SquareMatrix<Type, M> &operator=(const Matrix<Type, M, M> &other)
	{
		Matrix<Type, M, M>::operator=(other);
		return *this;
	}

	/**
	 * @brief Assignment from a mutable slice
	 * @tparam P Rows in slice (must be M)
	 * @tparam Q Columns in slice (must be M)
	 * @param in_slice Source slice view
	 * @return Reference to this square matrix
	 */
	template <size_t P, size_t Q>
	SquareMatrix<Type, M> &operator=(const Slice<Type, M, M, P, Q> &in_slice)
	{
		Matrix<Type, M, M>::operator=(in_slice);
		return *this;
	}

	/**
	 * @brief Create a const slice view of the matrix
	 * @tparam P Number of rows in the slice
	 * @tparam Q Number of columns in the slice
	 * @param x0 Starting row index
	 * @param y0 Starting column index
	 * @return Const slice view
	 */
	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, M> slice(size_t x0, size_t y0) const
	{
		return {x0, y0, this};
	}

	/**
	 * @brief Create a mutable slice view of the matrix
	 * @tparam P Number of rows in the slice
	 * @tparam Q Number of columns in the slice
	 * @param x0 Starting row index
	 * @param y0 Starting column index
	 * @return Mutable slice view
	 */
	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, M> slice(size_t x0, size_t y0)
	{
		return {x0, y0, this};
	}

	// inverse alias

	/**
	 * @brief Compute the matrix inverse (returns a new matrix)
	 * @return Inverse matrix, or zero matrix if singular
	 * 
	 * @note Uses the global `inv()` function which performs LU decomposition
	 *       with partial pivoting (optimised for 1×1, 2×2, 3×3).
	 */
	inline SquareMatrix<Type, M> I() const
	{
		SquareMatrix<Type, M> i;

		if (inv(*this, i)) {
			return i;

		} else {
			i.setZero();
			return i;
		}
	}

	// inverse alias

	/**
	 * @brief Compute the matrix inverse (output by reference)
	 * @param[out] i Output inverse matrix
	 * @return true if inversion succeeded, false if singular
	 */
	inline bool I(SquareMatrix<Type, M> &i) const
	{
		return inv(*this, i);
	}

	/**
	 * @brief Extract the diagonal elements as a column vector
	 * @return Vector of size M containing the diagonal entries
	 */
	Vector<Type, M> diag() const
	{
		Vector<Type, M> res;
		const SquareMatrix<Type, M> &self = *this;

		for (size_t i = 0; i < M; i++) {
			res(i) = self(i, i);
		}

		return res;
	}

	/**
	 * @brief Pack the upper‑right triangle into a row‑major vector
	 * 
	 * Stores elements where row index ≤ column index, in row‑major order.
	 * Length = M*(M+1)/2.
	 * 
	 * @return Vector containing the upper triangular part (including diagonal)
	 */
	Vector < Type, M *(M + 1) / 2 > upper_right_triangle() const
	{
		Vector < Type, M * (M + 1) / 2 > res;
		const SquareMatrix<Type, M> &self = *this;

		unsigned idx = 0;

		for (size_t x = 0; x < M; x++) {
			for (size_t y = x; y < M; y++) {
				res(idx) = self(x, y);
				++idx;
			}
		}

		return res;
	}

	/**
	 * @brief Compute the trace of a contiguous sub‑block (template size)
	 * @tparam Width Number of diagonal elements to sum (≤ M)
	 * @param first Starting diagonal index
	 * @return Sum of diagonal elements from `first` to `first+Width-1`
	 */
	template <size_t Width>
	Type trace(size_t first) const
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		Type res = 0;
		const SquareMatrix<Type, M> &self = *this;

		for (size_t i = first; i < (first + Width); i++) {
			res += self(i, i);
		}

		return res;
	}

	/**
	 * @brief Compute the full trace of the matrix
	 * @return Sum of all diagonal elements
	 */
	Type trace() const
	{
		const SquareMatrix<Type, M> &self = *this;
		return self.trace<M>(0);
	}

	/**
	 * @brief Zero out all covariance elements outside a diagonal block
	 * 
	 * Preserves the sub‑block at `[first, first]` of size Width×Width,
	 * and sets all other elements in the corresponding rows and columns to zero.
	 * 
	 * @tparam Width Size of the block to keep
	 * @param first Starting row/column index of the block
	 */
	template <size_t Width>
	void uncorrelateCovarianceBlock(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;
		SquareMatrix<Type, Width> cov = self.slice<Width, Width>(first, first);
		self.slice<M, Width>(0, first) = 0.f;
		self.slice<Width, M>(first, 0) = 0.f;
		self.slice<Width, Width>(first, first) = cov;
	}

	/**
	 * @brief Zero off‑diagonal elements in a block, keep only diagonal variances
	 * 
	 * Keeps the diagonal elements of the block at `[first, first]` (size Width)
	 * and sets all other elements (both inside the block and in the corresponding
	 * rows/columns) to zero.
	 * 
	 * @tparam Width Size of the block
	 * @param first Starting index
	 */
	template <size_t Width>
	void uncorrelateCovariance(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;
		Vector<Type, Width> diag_elements = self.slice<Width, Width>(first, first).diag();
		self.uncorrelateCovarianceSetVariance(first, diag_elements);
	}

	/**
	 * @brief Set a diagonal block to a diagonal matrix with given variances
	 * 
	 * Zeros the rows/columns corresponding to the block, then sets the diagonal
	 * entries from the provided vector.
	 * 
	 * @tparam Width Size of the block
	 * @param first Starting index
	 * @param vec Vector of length Width containing the diagonal values
	 */
	template <size_t Width>
	void uncorrelateCovarianceSetVariance(size_t first, const Vector<Type, Width> &vec)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;
		// zero rows and columns
		self.slice<Width, M>(first, 0) = Type(0);
		self.slice<M, Width>(0, first) = Type(0);

		// set diagonals
		unsigned vec_idx = 0;

		for (size_t idx = first; idx < first + Width; idx++) {
			self(idx, idx) = vec(vec_idx);
			vec_idx ++;
		}
	}

	/**
	 * @brief Set a diagonal block to a scalar times identity
	 * 
	 * Zeros the rows/columns corresponding to the block, then sets the diagonal
	 * entries to the same scalar value.
	 * 
	 * @tparam Width Size of the block
	 * @param first Starting index
	 * @param val Value to assign to each diagonal entry
	 */
	template <size_t Width>
	void uncorrelateCovarianceSetVariance(size_t first, Type val)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;
		// zero rows and columns
		self.slice<Width, M>(first, 0) = Type(0);
		self.slice<M, Width>(0, first) = Type(0);

		// set diagonals
		for (size_t idx = first; idx < first + Width; idx++) {
			self(idx, idx) = val;
		}
	}

	/**
	 * @brief Enforce symmetry on a diagonal block by averaging off‑diagonals
	 * 
	 * For the block starting at `first` of size Width, replaces each pair
	 * (i,j) and (j,i) with their average.
	 * 
	 * @tparam Width Size of the block
	 * @param first Starting index
	 */
	template <size_t Width>
	void makeBlockSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;

		if (Width > 1) {
			for (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				for (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
					self(row_idx, col_idx) = tmp;
					self(col_idx, row_idx) = tmp;
				}
			}
		}
	}

	/**
	 * @brief Enforce symmetry on rows/columns intersecting a diagonal block
	 * 
	 * Averages off‑diagonal elements both inside the block (using `makeBlockSymmetric`)
	 * and between the block and the rest of the matrix.
	 * 
	 * @tparam Width Size of the block
	 * @param first Starting index
	 */
	template <size_t Width>
	void makeRowColSymmetric(size_t first)
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;
		self.makeBlockSymmetric<Width>(first);

		for (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			for (size_t col_idx = 0; col_idx < first; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}

			for (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				Type tmp = (self(row_idx, col_idx) + self(col_idx, row_idx)) / Type(2);
				self(row_idx, col_idx) = tmp;
				self(col_idx, row_idx) = tmp;
			}
		}
	}

	/**
	 * @brief Check if a diagonal block is symmetric within tolerance
	 * @tparam Width Size of the block
	 * @param first Starting index
	 * @param eps Tolerance (default 1e-8)
	 * @return true if block is symmetric
	 */
	template <size_t Width>
	bool isBlockSymmetric(size_t first, const Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;

		if (Width > 1) {
			for (size_t row_idx = first + 1; row_idx < first + Width; row_idx++) {
				for (size_t col_idx = first; col_idx < row_idx; col_idx++) {
					if (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
						return false;
					}
				}
			}
		}

		return true;
	}

	/**
	 * @brief Check if rows/columns intersecting a diagonal block are symmetric
	 * @tparam Width Size of the block
	 * @param first Starting index
	 * @param eps Tolerance (default 1e-8)
	 * @return true if the entire intersection is symmetric
	 */
	template <size_t Width>
	bool isRowColSymmetric(size_t first, const Type eps = Type(1e-8f))
	{
		static_assert(Width <= M, "Width bigger than matrix");
		assert(first + Width <= M);

		SquareMatrix<Type, M> &self = *this;

		for (size_t row_idx = first; row_idx < first + Width; row_idx++) {
			for (size_t col_idx = 0; col_idx < first; col_idx++) {
				if (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					return false;
				}
			}

			for (size_t col_idx = first + Width; col_idx < M; col_idx++) {
				if (!isEqualF(self(row_idx, col_idx), self(col_idx, row_idx), eps)) {
					return false;
				}
			}
		}

		return self.isBlockSymmetric<Width>(first, eps);
	}

	/**
	 * @brief Copy lower triangle to upper triangle (make symmetric by copying)
	 * 
	 * Overwrites the upper triangle with the corresponding lower triangle values.
	 */
	void copyLowerToUpperTriangle()
	{
		SquareMatrix<Type, M> &self = *this;

		for (size_t row_idx = 1; row_idx < M; row_idx++) {
			for (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(col_idx, row_idx) = self(row_idx, col_idx);
			}
		}
	}

	/**
	 * @brief Copy upper triangle to lower triangle (make symmetric by copying)
	 * 
	 * Overwrites the lower triangle with the corresponding upper triangle values.
	 */
	void copyUpperToLowerTriangle()
	{
		SquareMatrix<Type, M> &self = *this;

		for (size_t row_idx = 1; row_idx < M; row_idx++) {
			for (size_t col_idx = 0 ; col_idx < row_idx; col_idx++) {
				self(row_idx, col_idx) = self(col_idx, row_idx);
			}
		}
	}
};

// -----------------------------------------------------------------------------
// Common type aliases
// -----------------------------------------------------------------------------

/** @brief 2×2 square matrix with float elements */
using SquareMatrix2f = SquareMatrix<float, 2>;

/** @brief 3×3 square matrix with float elements */
using SquareMatrix3f = SquareMatrix<float, 3>;

/** @brief 3×3 square matrix with double elements */
using SquareMatrix3d = SquareMatrix<double, 3>;

// -----------------------------------------------------------------------------
// Global utility functions
// -----------------------------------------------------------------------------

/**
 * @brief Create an identity matrix of size M×M
 * @tparam Type Element type
 * @tparam M Matrix dimension
 * @return Identity matrix
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> eye()
{
	SquareMatrix<Type, M> m;
	m.setIdentity();
	return m;
}

/**
 * @brief Create a diagonal matrix from a vector
 * @tparam Type Element type
 * @tparam M Matrix dimension
 * @param d Vector of length M containing diagonal entries
 * @return Diagonal matrix with the given diagonal
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> diag(Vector<Type, M> d)
{
	SquareMatrix<Type, M> m;

	for (size_t i = 0; i < M; i++) {
		m(i, i) = d(i);
	}

	return m;
}

/**
 * @brief Compute matrix exponential using truncated Taylor series
 * 
 * Approximates exp(A) = I + A + A^2/2! + A^3/3! + ... up to the given order.
 * 
 * @tparam Type Element type
 * @tparam M Matrix dimension
 * @param A Input matrix
 * @param order Number of terms (excluding the identity term). Default 5.
 * @return Approximated matrix exponential
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> expm(const Matrix<Type, M, M> &A, size_t order = 5)
{
	SquareMatrix<Type, M> res;
	SquareMatrix<Type, M> A_pow = A;
	res.setIdentity();
	size_t i_factorial = 1;

	for (size_t i = 1; i <= order; i++) {
		i_factorial *= i;
		res += A_pow / Type(i_factorial);
		A_pow *= A_pow;
	}

	return res;
}

// -----------------------------------------------------------------------------
// Matrix inversion (specialised for 1×1, 2×2, 3×3, and general M)
// -----------------------------------------------------------------------------

/**
 * @brief Invert a 1×1 matrix
 * @param A Input matrix
 * @param inv Output inverse (1×1)
 * @param rank Unused (kept for compatibility)
 * @return true if invertible (non‑zero element), false otherwise
 */
template<typename Type>
bool inv(const SquareMatrix<Type, 1> &A, SquareMatrix<Type, 1> &inv, size_t rank = 1)
{
	if (std::fabs(A(0, 0)) < Type(FLT_EPSILON)) {
		return false;
	}

	inv(0, 0) = Type(1) / A(0, 0);
	return true;
}

/**
 * @brief General matrix inversion using LU decomposition with partial pivoting
 * @tparam Type Element type
 * @tparam M Matrix dimension
 * @param A Input matrix
 * @param inv Output inverse
 * @param rank Use only the top‑left rank×rank submatrix (default M)
 * @return true if inversion succeeded, false if singular
 */
template<typename Type, size_t M>
bool inv(const SquareMatrix<Type, M> &A, SquareMatrix<Type, M> &inv, size_t rank = M)
{
	SquareMatrix<Type, M> L;
	L.setIdentity();
	SquareMatrix<Type, M> U = A;
	SquareMatrix<Type, M> P;
	P.setIdentity();

	//printf("A:\n"); A.print();

	// for all diagonal elements
	for (size_t n = 0; n < rank; n++) {

		// if diagonal is zero, swap with row below
		if (std::fabs(U(n, n)) < Type(FLT_EPSILON)) {
			//printf("trying pivot for row %d\n",n);
			for (size_t i = n + 1; i < rank; i++) {

				//printf("\ttrying row %d\n",i);
				if (std::fabs(U(i, n)) > Type(FLT_EPSILON)) {
					//printf("swapped %d\n",i);
					U.swapRows(i, n);
					P.swapRows(i, n);
					L.swapRows(i, n);
					L.swapCols(i, n);
					break;
				}
			}
		}

#ifdef MATRIX_ASSERT
		//printf("A:\n"); A.print();
		//printf("U:\n"); U.print();
		//printf("P:\n"); P.print();
		//fflush(stdout);
		//ASSERT(fabs(U(n, n)) > 1e-8f);
#endif

		// failsafe, return zero matrix
		if (std::fabs(static_cast<float>(U(n, n))) < FLT_EPSILON) {
			return false;
		}

		// for all rows below diagonal
		for (size_t i = (n + 1); i < rank; i++) {
			L(i, n) = U(i, n) / U(n, n);

			// add i-th row and n-th row
			// multiplied by: -a(i,n)/a(n,n)
			for (size_t k = n; k < rank; k++) {
				U(i, k) -= L(i, n) * U(n, k);
			}
		}
	}

	//printf("L:\n"); L.print();
	//printf("U:\n"); U.print();

	// solve LY=P*I for Y by forward subst
	//SquareMatrix<Type, M> Y = P;

	// for all columns of Y
	for (size_t c = 0; c < rank; c++) {
		// for all rows of L
		for (size_t i = 0; i < rank; i++) {
			// for all columns of L
			for (size_t j = 0; j < i; j++) {
				// for all existing y
				// subtract the component they
				// contribute to the solution
				P(i, c) -= L(i, j) * P(j, c);
			}

			// divide by the factor
			// on current
			// term to be solved
			// Y(i,c) /= L(i,i);
			// but L(i,i) = 1.0
		}
	}

	//printf("Y:\n"); Y.print();

	// solve Ux=y for x by back subst
	//SquareMatrix<Type, M> X = Y;

	// for all columns of X
	for (size_t c = 0; c < rank; c++) {
		// for all rows of U
		for (size_t k = 0; k < rank; k++) {
			// have to go in reverse order
			size_t i = rank - 1 - k;

			// for all columns of U
			for (size_t j = i + 1; j < rank; j++) {
				// for all existing x
				// subtract the component they
				// contribute to the solution
				P(i, c) -= U(i, j) * P(j, c);
			}

			// divide by the factor
			// on current
			// term to be solved
			//
			// we know that U(i, i) != 0 from above
			P(i, c) /= U(i, i);
		}
	}

	//check sanity of results
	for (size_t i = 0; i < rank; i++) {
		for (size_t j = 0; j < rank; j++) {
			if (!std::isfinite(P(i, j))) {
				return false;
			}
		}
	}

	//printf("X:\n"); X.print();
	inv = P;
	return true;
}

/**
 * @brief Optimised inversion for 2×2 matrices
 * @param A Input 2×2 matrix
 * @param inv Output inverse
 * @return true if invertible (determinant not zero), false otherwise
 */
template<typename Type>
bool inv(const SquareMatrix<Type, 2> &A, SquareMatrix<Type, 2> &inv)
{
	Type det = A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1);

	if (std::fabs(static_cast<float>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		return false;
	}

	inv(0, 0) = A(1, 1);
	inv(1, 0) = -A(1, 0);
	inv(0, 1) = -A(0, 1);
	inv(1, 1) = A(0, 0);
	inv /= det;
	return true;
}

/**
 * @brief Optimised inversion for 3×3 matrices using adjugate formula
 * @param A Input 3×3 matrix
 * @param inv Output inverse
 * @return true if invertible, false otherwise
 */
template<typename Type>
bool inv(const SquareMatrix<Type, 3> &A, SquareMatrix<Type, 3> &inv)
{
	Type det = A(0, 0) * (A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2)) -
		   A(0, 1) * (A(1, 0) * A(2, 2) - A(1, 2) * A(2, 0)) +
		   A(0, 2) * (A(1, 0) * A(2, 1) - A(1, 1) * A(2, 0));

	if (std::fabs(static_cast<float>(det)) < FLT_EPSILON || !std::isfinite(det)) {
		return false;
	}

	inv(0, 0) = A(1, 1) * A(2, 2) - A(2, 1) * A(1, 2);
	inv(0, 1) = A(0, 2) * A(2, 1) - A(0, 1) * A(2, 2);
	inv(0, 2) = A(0, 1) * A(1, 2) - A(0, 2) * A(1, 1);
	inv(1, 0) = A(1, 2) * A(2, 0) - A(1, 0) * A(2, 2);
	inv(1, 1) = A(0, 0) * A(2, 2) - A(0, 2) * A(2, 0);
	inv(1, 2) = A(1, 0) * A(0, 2) - A(0, 0) * A(1, 2);
	inv(2, 0) = A(1, 0) * A(2, 1) - A(2, 0) * A(1, 1);
	inv(2, 1) = A(2, 0) * A(0, 1) - A(0, 0) * A(2, 1);
	inv(2, 2) = A(0, 0) * A(1, 1) - A(1, 0) * A(0, 1);
	inv /= det;
	return true;
}

/**
 * @brief Inverse that returns a matrix (throws away singularity information)
 * @param A Input matrix
 * @return Inverse if invertible, otherwise zero matrix
 */
template<typename Type, size_t M>
SquareMatrix<Type, M> inv(const SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> i;

	if (inv(A, i)) {
		return i;

	} else {
		i.setZero();
		return i;
	}
}

// -----------------------------------------------------------------------------
// Cholesky decomposition and Cholesky inversion
// -----------------------------------------------------------------------------

/**
 * @brief Compute the lower‑triangular Cholesky factor L such that A = L * L^T
 * 
 * @tparam Type Element type
 * @tparam M Matrix dimension
 * @param A Positive‑definite input matrix
 * @return Lower triangular matrix L (if A is not positive definite, L may contain zeros)
 * 
 * @note For indefinite matrices, the decomposition may fail silently (returns L with zeros).
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> cholesky(const SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L;

	for (size_t j = 0; j < M; j++) {
		for (size_t i = j; i < M; i++) {
			if (i == j) {
				float sum = 0;

				for (size_t k = 0; k < j; k++) {
					sum += L(j, k) * L(j, k);
				}

				Type res = A(j, j) - sum;

				if (res <= 0) {
					L(j, j) = 0;

				} else {
					L(j, j) = std::sqrt(res);
				}

			} else {
				float sum = 0;

				for (size_t k = 0; k < j; k++) {
					sum += L(i, k) * L(j, k);
				}

				if (L(j, j) <= 0) {
					L(i, j) = 0;

				} else {
					L(i, j) = (A(i, j) - sum) / L(j, j);
				}
			}
		}
	}

	return L;
}

/**
 * @brief Compute the inverse of a positive‑definite matrix using Cholesky decomposition
 * 
 * Computes A⁻¹ = (L⁻¹)^T * L⁻¹, where A = L * L^T.
 * 
 * @tparam Type Element type
 * @tparam M Matrix dimension
 * @param A Positive‑definite input matrix
 * @return Inverse of A (or zero matrix if decomposition fails)
 */
template<typename Type, size_t M>
SquareMatrix <Type, M> choleskyInv(const SquareMatrix<Type, M> &A)
{
	SquareMatrix<Type, M> L_inv = inv(cholesky(A));
	return L_inv.T() * L_inv;
}

// -----------------------------------------------------------------------------
// Additional type aliases for backward compatibility
// -----------------------------------------------------------------------------

/** @brief 2×2 square matrix (float) – alias for SquareMatrix2f */
using Matrix2f = SquareMatrix<float, 2>;

/** @brief 3×3 square matrix (float) – alias for SquareMatrix3f */
using Matrix3f = SquareMatrix<float, 3>;

/** @brief 3×3 square matrix (double) – alias for SquareMatrix3d */
using Matrix3d = SquareMatrix<double, 3>;

} // namespace matrix
