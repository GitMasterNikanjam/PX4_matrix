/**
 * @file Matrix.hpp
 * @brief Static-size matrix template library with compile-time dimension checking
 * 
 * This file provides a template-based matrix class with fixed dimensions determined
 * at compile time. It offers a comprehensive set of matrix operations including
 * arithmetic, slicing, and various utility functions. The library is designed for
 * embedded and real-time systems where dynamic memory allocation is avoided.
 * 
 * Key features:
 * - Compile-time dimension checking
 * - No dynamic memory allocation
 * - Slice/view support for submatrix operations
 * - Element-wise operations
 * - Matrix multiplication with dimension validation
 * - Row-major storage layout
 * - Support for const and non-const views
 * 
 * @author James Goppert <james.goppert@gmail.com>
 * @ingroup matrix
 */

// #############################################################################

#pragma once

// #############################################################################
// Include libraries

#include <cmath>      				// Mathematical functions (sqrt, fabs, etc.)
#include <cstdio>     				// Printf family for matrix output formatting
#include <cstring>    				// Memcpy/memset for efficient initialization

#include "helper_functions.hpp"  	// isEqualF and other helper utilities
#include "Slice.hpp"             	// Matrix slice/view implementation

// #############################################################################

namespace matrix
{

/**
 * @brief Static-size matrix class with compile-time dimensions
 * 
 * Matrix provides a fixed-size matrix container with comprehensive mathematical
 * operations. All dimensions are known at compile time, enabling compiler
 * optimizations and preventing out-of-bounds access.
 * 
 * Storage is row-major in a 2D C-style array embedded directly in the class
 * (no heap allocation). The matrix is stored as Type[M][N].
 * 
 * @tparam Type The numerical type of matrix elements (float, double, int, etc.)
 * @tparam M Number of rows (compile-time constant)
 * @tparam N Number of columns (compile-time constant)
 * 
 * @note This class is designed for small-to-medium sized matrices typical in
 *       control systems and robotics applications (e.g., 3x3, 4x4, 6x6).
 * 
 * @see Slice for submatrix views
 * @see Vector for column vector specialization
 */
template<typename Type, size_t M, size_t N>
class Matrix
{
	/**
	 * @brief Internal storage: row-major 2D array
	 * 
	 * All elements are stored contiguously within each row.
	 * Default-initialized to zero (value-initialization of array).
	 */
	Type _data[M][N] {};

public:

	// ========================================================================
	// Constructors
	// ========================================================================

	/**
	 * @brief Default constructor - zero-initializes all elements
	 * 
	 * Uses value-initialization (brace initializer) to set all elements to 0.
	 */
	Matrix() = default;

	/**
	 * @brief Construct from flat array (row-major order)
	 * 
	 * Creates a matrix by copying data from a flat array where elements are
	 * stored row by row.
	 * 
	 * @param data_ Flat array of size M * N in row-major order
	 * 
	 * @pre data_ must contain at least M*N elements
	 */
	explicit Matrix(const Type data_[M * N])
	{
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[N * i + j];
			}
		}
	}

	/**
	 * @brief Construct from 2D array
	 * 
	 * @param data_ 2D array of size M x N
	 */
	explicit Matrix(const Type data_[M][N])
	{
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				_data[i][j] = data_[i][j];
			}
		}
	}

	/**
	 * @brief Copy constructor
	 * 
	 * @param other Source matrix to copy from
	 */
	Matrix(const Matrix &other)
	{
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				_data[i][j] = other(i, j);
			}
		}
	}

	/**
	 * @brief Type-conversion constructor
	 * 
	 * Creates a matrix from another matrix with potentially different element type.
	 * Performs static_cast on each element.
	 * 
	 * @tparam S Source element type
	 * @param aa Source matrix of same dimensions but possibly different type
	 */
	template<typename S>
	Matrix(const Matrix<S, M, N> &aa)
	{
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				_data[i][j] = static_cast<Type>(aa(i, j));
			}
		}
	}

	/**
	 * @brief Construct from mutable slice
	 * 
	 * Creates a matrix by copying data from a slice/view.
	 * 
	 * @tparam P Number of rows in source slice (must be M)
	 * @tparam Q Number of columns in source slice (must be N)
	 * @param in_slice Source slice to copy from
	 */
	template<size_t P, size_t Q>
	Matrix(const Slice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	/**
	 * @brief Construct from const slice
	 * 
	 * Creates a matrix by copying data from a read-only slice/view.
	 * 
	 * @tparam P Number of rows in source slice (must be M)
	 * @tparam Q Number of columns in source slice (must be N)
	 * @param in_slice Source const slice to copy from
	 */
	template<size_t P, size_t Q>
	Matrix(const ConstSlice<Type, M, N, P, Q> &in_slice)
	{
		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				self(i, j) = in_slice(i, j);
			}
		}
	}

	// ========================================================================
	// Element Access
	// ========================================================================

	/**
	 * Accessors/ Assignment etc.
	 */

	/**
	 * @brief Read-only element access
	 * 
	 * @param i Row index (0 <= i < M)
	 * @param j Column index (0 <= j < N)
	 * @return Const reference to the element
	 * 
	 * @pre i < M && j < N
	 * 
	 * @note Bounds checking is performed only in debug builds via assert()
	 */
	inline const Type &operator()(size_t i, size_t j) const
	{
		assert(i < M);
		assert(j < N);

		return _data[i][j];
	}

	/**
	 * @brief Mutable element access
	 * 
	 * @param i Row index (0 <= i < M)
	 * @param j Column index (0 <= j < N)
	 * @return Mutable reference to the element
	 * 
	 * @pre i < M && j < N
	 * 
	 * @note Bounds checking is performed only in debug builds via assert()
	 */
	inline Type &operator()(size_t i, size_t j)
	{
		assert(i < M);
		assert(j < N);

		return _data[i][j];
	}

	/**
	 * @brief Copy assignment operator
	 * 
	 * @param other Source matrix
	 * @return Reference to this matrix after assignment
	 * 
	 * @note Handles self-assignment correctly
	 */
	Matrix<Type, M, N> &operator=(const Matrix<Type, M, N> &other)
	{
		if (this != &other) {
			Matrix<Type, M, N> &self = *this;

			for (size_t i = 0; i < M; i++) {
				for (size_t j = 0; j < N; j++) {
					self(i, j) = other(i, j);
				}
			}
		}

		return (*this);
	}

	/**
	 * @brief Copy matrix to flat row-major array
	 * 
	 * @param dst Destination array of size at least M * N
	 * 
	 * @pre dst is pre-allocated with M*N elements
	 */
	void copyTo(Type dst[M * N]) const
	{
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				dst[N * i + j] = self(i, j);
			}
		}
	}

	/**
	 * @brief Copy matrix to flat column-major array
	 * 
	 * Useful for interfacing with software expecting column-major storage
	 * (e.g., some BLAS/LAPACK implementations).
	 * 
	 * @param dst Destination array of size at least M * N
	 * 
	 * @pre dst is pre-allocated with M*N elements
	 */
	void copyToColumnMajor(Type dst[M * N]) const
	{
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				dst[i + (j * M)] = self(i, j);
			}
		}
	}

	// ========================================================================
	// Matrix Arithmetic Operations
	// ========================================================================

	/**
	 * Matrix Operations
	 */

	/**
	 * @brief Matrix multiplication with compile-time dimension checking
	 * 
	 * Performs matrix multiplication: (*this) * other
	 * 
	 * @tparam P Number of columns in the result (columns of other)
	 * @param other Right-hand side matrix of size N x P
	 * @return Result matrix of size M x P
	 * 
	 * @note Uses naive triple-loop algorithm; optimized for small matrices
	 * @warning Instantiates a class for each multiplication pair; may increase
	 *          binary size with many different dimension combinations
	 */
	template<size_t P>
	Matrix<Type, M, P> operator*(const Matrix<Type, N, P> &other) const
	{
		const Matrix<Type, M, N> &self = *this;
		Matrix<Type, M, P> res{};

		for (size_t i = 0; i < M; i++) {
			for (size_t k = 0; k < P; k++) {
				for (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(j, k);
				}
			}
		}

		return res;
	}

	// Using this function reduces the number of temporary variables needed to compute A * B.T

	/**
	 * @brief Multiply by transpose of another matrix
	 * 
	 * Computes (*this) * other^T efficiently without forming transpose.
	 * This can be faster than explicit transpose and multiply.
	 * 
	 * @tparam P Number of rows in other matrix
	 * @param other Matrix of size P x N
	 * @return Result matrix of size M x M
	 * 
	 * @note Reduces temporary variables compared to A * B.T
	 */
	template<size_t P>
	Matrix<Type, M, M> multiplyByTranspose(const Matrix<Type, P, N> &other) const
	{
		Matrix<Type, M, P> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t k = 0; k < P; k++) {
				for (size_t j = 0; j < N; j++) {
					res(i, k) += self(i, j) * other(k, j);
				}
			}
		}

		return res;
	}

	/**
	 * @brief Element-wise multiplication (Hadamard product)
	 * 
	 * @param other Right-hand side matrix (same dimensions)
	 * @return Matrix with each element = self(i,j) * other(i,j)
	 */
	Matrix<Type, M, N> emult(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * other(i, j);
			}
		}

		return res;
	}

	/**
	 * @brief Element-wise division
	 * 
	 * @param other Right-hand side matrix (same dimensions)
	 * @return Matrix with each element = self(i,j) / other(i,j)
	 * 
	 * @pre No division by zero (check other elements before calling)
	 */
	Matrix<Type, M, N> edivide(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) / other(i, j);
			}
		}

		return res;
	}

	/**
	 * @brief Matrix addition
	 * 
	 * @param other Right-hand side matrix (same dimensions)
	 * @return New matrix containing element-wise sum
	 */
	Matrix<Type, M, N> operator+(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + other(i, j);
			}
		}

		return res;
	}

	/**
	 * @brief Matrix subtraction
	 * 
	 * @param other Right-hand side matrix (same dimensions)
	 * @return New matrix containing element-wise difference
	 */
	Matrix<Type, M, N> operator-(const Matrix<Type, M, N> &other) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) - other(i, j);
			}
		}

		return res;
	}

	/**
	 * @brief Unary minus (negation)
	 * 
	 * @return Matrix with all elements negated
	 */
	Matrix<Type, M, N> operator-() const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(i, j) = -self(i, j);
			}
		}

		return res;
	}

	/**
	 * @brief Compound addition assignment
	 * 
	 * @param other Right-hand side matrix
	 */
	void operator+=(const Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				self(i, j) += other(i, j);
			}
		}
	}

	/**
	 * @brief Compound subtraction assignment
	 * 
	 * @param other Right-hand side matrix
	 */
	void operator-=(const Matrix<Type, M, N> &other)
	{
		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				self(i, j) -= other(i, j);
			}
		}
	}

	/**
	 * @brief Compound multiplication assignment (matrix multiplication)
	 * 
	 * @tparam P Number of columns in other matrix
	 * @param other Right-hand side matrix
	 * 
	 * @note This operation changes the matrix dimensions if N != P
	 */
	template<size_t P>
	void operator*=(const Matrix<Type, N, P> &other)
	{
		Matrix<Type, M, N> &self = *this;
		self = self * other;
	}

	// ========================================================================
	// Scalar Operations
	// ========================================================================

	/**
	 * @brief Scalar multiplication
	 * 
	 * @param scalar Multiplier
	 * @return New matrix with each element multiplied by scalar
	 */
	Matrix<Type, M, N> operator*(Type scalar) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) * scalar;
			}
		}

		return res;
	}

	/**
	 * @brief Scalar division
	 * 
	 * @param scalar Divisor (must be non-zero)
	 * @return New matrix with each element divided by scalar
	 * 
	 * @pre scalar != 0
	 */
	inline Matrix<Type, M, N> operator/(Type scalar) const
	{
		return (*this) * (1 / scalar);
	}

	/**
	 * @brief Scalar addition
	 * 
	 * @param scalar Value to add to each element
	 * @return New matrix with scalar added to each element
	 */
	Matrix<Type, M, N> operator+(Type scalar) const
	{
		Matrix<Type, M, N> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(i, j) = self(i, j) + scalar;
			}
		}

		return res;
	}

	/**
	 * @brief Scalar subtraction
	 * 
	 * @param scalar Value to subtract from each element
	 * @return New matrix with scalar subtracted from each element
	 */
	inline Matrix<Type, M, N> operator-(Type scalar) const
	{
		return (*this) + (-1 * scalar);
	}

	/**
	 * @brief Scalar multiplication assignment
	 * 
	 * @param scalar Multiplier
	 */
	void operator*=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				self(i, j) *= scalar;
			}
		}
	}

	/**
	 * @brief Scalar division assignment
	 * 
	 * @param scalar Divisor (must be non-zero)
	 */
	void operator/=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;
		self *= (Type(1) / scalar);
	}

	/**
	 * @brief Scalar addition assignment
	 * 
	 * @param scalar Value to add to each element
	 */
	inline void operator+=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				self(i, j) += scalar;
			}
		}
	}

	/**
	 * @brief Scalar subtraction assignment
	 * 
	 * @param scalar Value to subtract from each element
	 */
	inline void operator-=(Type scalar)
	{
		Matrix<Type, M, N> &self = *this;
		self += (-scalar);
	}

	/**
	 * @brief Equality comparison
	 * 
	 * @param other Matrix to compare with
	 * @return true if all corresponding elements are equal (within tolerance)
	 */
	bool operator==(const Matrix<Type, M, N> &other) const
	{
		return isEqual(*this, other);
	}

	/**
	 * @brief Inequality comparison
	 * 
	 * @param other Matrix to compare with
	 * @return true if any elements differ
	 */
	bool operator!=(const Matrix<Type, M, N> &other) const
	{
		const Matrix<Type, M, N> &self = *this;
		return !(self == other);
	}

	// ========================================================================
	// Utility Functions
	// ========================================================================

	/**
	 * @brief Write matrix to string buffer
	 * 
	 * Formats the matrix as text with tab-separated values and newlines.
	 * 
	 * @param buf Output character buffer
	 * @param n Size of the buffer (to prevent overflow)
	 * 
	 * @pre buf has capacity for at least n characters
	 */
	void write_string(char *buf, size_t n) const
	{
		buf[0] = '\0'; // make an empty string to begin with (we need the '\0' for strlen to work)
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				snprintf(buf + strlen(buf), n - strlen(buf), "\t%8.8g", double(self(i, j))); // directly append to the string buffer
			}

			snprintf(buf + strlen(buf), n - strlen(buf), "\n");
		}
	}

	/**
	 * @brief Print matrix to stdout
	 * 
	 * Outputs a formatted matrix with row/column numbers and
	 * symmetric matrix optimization (skips upper triangle for symmetric matrices).
	 * 
	 * @param eps Tolerance for symmetry detection (default: 1e-9)
	 * 
	 * @note For symmetric matrices, only lower triangle is printed to save space
	 * @note Values very close to zero are displayed as "0"
	 */
	void print(float eps = 1e-9) const
	{
		// print column numbering
		if (N > 1) {
			printf("  ");

			for (unsigned i = 0; i < N; i++) {
				printf("|%2u      ", i);

			}

			printf("\n");
		}

		const Matrix<Type, M, N> &self = *this;
		bool is_prev_symmetric = true; // assume symmetric until one element is not

		for (unsigned i = 0; i < M; i++) {
			printf("%2u|", i); // print row numbering

			for (unsigned j = 0; j < N; j++) {
				double d = static_cast<double>(self(i, j));

				// if symmetric don't print upper triangular elements
				if (is_prev_symmetric && (M == N) && (j > i) && (i < N) && (j < M)
				    && (fabs(d - static_cast<double>(self(j, i))) < (double)eps)
				   ) {
					// print empty space
					printf("         ");

				} else {
					// avoid -0.0 for display
					if (fabs(d - 0.0) < (double)eps) {
						// print fixed width zero
						printf(" 0       ");

					} else if ((fabs(d) < 1e-4) || (fabs(d) >= 10.0)) {
						printf("% .1e ", d);

					} else {
						printf("% 6.5f ", d);
					}

					is_prev_symmetric = false; // not symmetric if once inside here
				}
			}

			printf("\n");
		}
	}

	/**
	 * @brief Matrix transpose
	 * 
	 * @return New matrix of size N x M with rows and columns swapped
	 */
	Matrix<Type, N, M> transpose() const
	{
		Matrix<Type, N, M> res;
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res(j, i) = self(i, j);
			}
		}

		return res;
	}

	/**
	 * @brief Transpose alias (short form)
	 * 
	 * @return Transposed matrix
	 */
	inline Matrix<Type, N, M> T() const
	{
		return transpose();
	}

	/**
	 * @brief Create const slice/view of this matrix
	 * 
	 * @tparam P Number of rows in the slice
	 * @tparam Q Number of columns in the slice
	 * @param x0 Starting row index
	 * @param y0 Starting column index
	 * @return Const slice view of the matrix
	 * 
	 * @pre x0 + P <= M && y0 + Q <= N
	 */
	template<size_t P, size_t Q>
	ConstSlice<Type, P, Q, M, N> slice(size_t x0, size_t y0) const
	{
		return {x0, y0, this};
	}

	/**
	 * @brief Create mutable slice/view of this matrix
	 * 
	 * @tparam P Number of rows in the slice
	 * @tparam Q Number of columns in the slice
	 * @param x0 Starting row index
	 * @param y0 Starting column index
	 * @return Mutable slice view of the matrix
	 * 
	 * @pre x0 + P <= M && y0 + Q <= N
	 */
	template<size_t P, size_t Q>
	Slice<Type, P, Q, M, N> slice(size_t x0, size_t y0)
	{
		return {x0, y0, this};
	}

	/**
	 * @brief Extract a row as const slice
	 * 
	 * @param i Row index (0 <= i < M)
	 * @return Const slice view of the row (size 1 x N)
	 */
	ConstSlice<Type, 1, N, M, N> row(size_t i) const
	{
		return slice<1, N>(i, 0);
	}

	/**
	 * @brief Extract a row as mutable slice
	 * 
	 * @param i Row index (0 <= i < M)
	 * @return Mutable slice view of the row (size 1 x N)
	 */
	Slice<Type, 1, N, M, N> row(size_t i)
	{
		return slice<1, N>(i, 0);
	}

	/**
	 * @brief Extract a column as const slice
	 * 
	 * @param j Column index (0 <= j < N)
	 * @return Const slice view of the column (size M x 1)
	 */
	ConstSlice<Type, M, 1, M, N> col(size_t j) const
	{
		return slice<M, 1>(0, j);
	}

	/**
	 * @brief Extract a column as mutable slice
	 * 
	 * @param j Column index (0 <= j < N)
	 * @return Mutable slice view of the column (size M x 1)
	 */
	Slice<Type, M, 1, M, N> col(size_t j)
	{
		return slice<M, 1>(0, j);
	}

	/**
	 * @brief Set a row from a column vector
	 * 
	 * @param i Row index to set
	 * @param row_in Column vector of length N (will be transposed)
	 */
	void setRow(size_t i, const Matrix<Type, N, 1> &row_in)
	{
		slice<1, N>(i, 0) = row_in.transpose();
	}

	/**
	 * @brief Set a row to a constant value
	 * 
	 * @param i Row index to set
	 * @param val Value to assign to all elements in the row
	 */
	void setRow(size_t i, Type val)
	{
		slice<1, N>(i, 0) = val;
	}

	/**
	 * @brief Set a column from a vector
	 * 
	 * @param j Column index to set
	 * @param column Column vector of length M
	 */
	void setCol(size_t j, const Matrix<Type, M, 1> &column)
	{
		slice<M, 1>(0, j) = column;
	}

	/**
	 * @brief Set a column to a constant value
	 * 
	 * @param j Column index to set
	 * @param val Value to assign to all elements in the column
	 */
	void setCol(size_t j, Type val)
	{
		slice<M, 1>(0, j) = val;
	}

	/**
	 * @brief Zero all elements
	 * 
	 * Uses memset for efficient zeroing of POD types.
	 */
	void setZero()
	{
		memset(_data, 0, sizeof(_data));
	}

	/**
	 * @brief Zero all elements (alias)
	 */
	inline void zero()
	{
		setZero();
	}

	/**
	 * @brief Set all elements to a constant value
	 * 
	 * @param val Value to assign to every element
	 */
	void setAll(Type val)
	{
		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				self(i, j) = val;
			}
		}
	}

	/**
	 * @brief Set all elements to 1
	 */
	inline void setOne()
	{
		setAll(1);
	}

	/**
	 * @brief Set all elements to NaN (Not a Number)
	 * 
	 * @note Only valid for floating point types
	 */
	inline void setNaN()
	{
		setAll(NAN);
	}

	/**
	 * @brief Set to identity matrix
	 * 
	 * Sets diagonal elements to 1, off-diagonal elements to 0.
	 * For non-square matrices, sets min(M,N) diagonal elements to 1.
	 */
	void setIdentity()
	{
		setZero();
		Matrix<Type, M, N> &self = *this;

		const size_t min_i = M > N ? N : M;

		for (size_t i = 0; i < min_i; i++) {
			self(i, i) = 1;
		}
	}

	/**
	 * @brief Set to identity matrix (alias)
	 */
	inline void identity()
	{
		setIdentity();
	}

	/**
	 * @brief Swap two rows
	 * 
	 * @param a First row index
	 * @param b Second row index
	 */
	inline void swapRows(size_t a, size_t b)
	{
		assert(a < M);
		assert(b < M);

		if (a == b) {
			return;
		}

		Matrix<Type, M, N> &self = *this;

		for (size_t j = 0; j < N; j++) {
			Type tmp = self(a, j);
			self(a, j) = self(b, j);
			self(b, j) = tmp;
		}
	}

	/**
	 * @brief Swap two columns
	 * 
	 * @param a First column index
	 * @param b Second column index
	 */
	inline void swapCols(size_t a, size_t b)
	{
		assert(a < N);
		assert(b < N);

		if (a == b) {
			return;
		}

		Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			Type tmp = self(i, a);
			self(i, a) = self(i, b);
			self(i, b) = tmp;
		}
	}

	/**
	 * @brief Element-wise absolute value
	 * 
	 * @return Matrix with absolute values of each element
	 */
	Matrix<Type, M, N> abs() const
	{
		Matrix<Type, M, N> r;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				r(i, j) = Type(std::fabs((*this)(i, j)));
			}
		}

		return r;
	}

	/**
	 * @brief Find maximum element value
	 * 
	 * @return Maximum value in the matrix
	 */
	Type max() const
	{
		Type max_val = (*this)(0, 0);

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
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
	 * @return Minimum value in the matrix
	 */
	Type min() const
	{
		Type min_val = (*this)(0, 0);

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				Type val = (*this)(i, j);

				if (val < min_val) {
					min_val = val;
				}
			}
		}

		return min_val;
	}

	/**
	 * @brief Check if all elements are NaN
	 * 
	 * @return true if every element is NaN
	 */
	bool isAllNan() const
	{
		const Matrix<Type, M, N> &self = *this;
		bool result = true;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				result = result && std::isnan(self(i, j));
			}
		}

		return result;
	}

	/**
	 * @brief Check if all elements are finite (not NaN or infinity)
	 * 
	 * @return true if all elements are finite numbers
	 */
	bool isAllFinite() const
	{
		const Matrix<Type, M, N> &self = *this;

		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				if (!std::isfinite(self(i, j))) {
					return false;
				}
			}
		}

		return true;
	}
};

// ============================================================================
// Global Factory Functions
// ============================================================================

/**
 * @brief Create a zero-initialized matrix
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @return Matrix with all elements set to 0
 */
template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> zeros()
{
	Matrix<Type, M, N> m;
	m.setZero();
	return m;
}

/**
 * @brief Create a matrix filled with ones * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @return Matrix with all elements set to 1
 */
template<typename Type, size_t M, size_t N>
Matrix<Type, M, N> ones()
{
	Matrix<Type, M, N> m;
	m.setOne();
	return m;
}

/**
 * @brief Create a matrix filled with NaN
 * 
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @return Matrix with all elements set to NaN (float type)
 * 
 * @note Specialization for float type
 */
template<size_t M, size_t N>
Matrix<float, M, N> nans()
{
	Matrix<float, M, N> m;
	m.setNaN();
	return m;
}

// ============================================================================
// Global Operator Overloads
// ============================================================================

/**
 * @brief Scalar multiplication (left side)
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param scalar Scalar multiplier
 * @param other Matrix to multiply
 * @return Matrix with each element multiplied by scalar
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> operator*(Type scalar, const Matrix<Type, M, N> &other)
{
	return other * scalar;
}

/**
 * @brief Element-wise equality check with tolerance
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param x First matrix
 * @param y Second matrix
 * @param eps Numerical tolerance (default: 1e-4f)
 * @return true if all corresponding elements are within tolerance
 * 
 * @see isEqualF for floating point comparison details
 */
template<typename Type, size_t  M, size_t N>
bool isEqual(const Matrix<Type, M, N> &x,
	     const Matrix<Type, M, N> &y, const Type eps = Type(1e-4f))
{
	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			if (!isEqualF(x(i, j), y(i, j), eps)) {
				return false;
			}
		}
	}

	return true;
}

// ============================================================================
// Type-Safe Utility Functions
// ============================================================================

namespace typeFunction
{

/**
 * @brief Type-safe minimum with NaN handling
 * 
 * Returns the minimum of two values. If one is NaN, returns the non-NaN value.
 * If both are NaN, returns NaN.
 * 
 * @tparam Type Numeric type
 * @param x First value
 * @param y Second value
 * @return Minimum value (with NaN precedence rules)
 */
template<typename Type>
Type min(const Type x, const Type y)
{
	bool x_is_nan = std::isnan(x);
	bool y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	if (x_is_nan || y_is_nan) {
		if (x_is_nan && !y_is_nan) {
			return y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		return x;
	}

	return (x < y) ? x : y;
}

/**
 * @brief Type-safe maximum with NaN handling
 * 
 * Returns the maximum of two values. If one is NaN, returns the non-NaN value.
 * If both are NaN, returns NaN.
 * 
 * @tparam Type Numeric type
 * @param x First value
 * @param y Second value
 * @return Maximum value (with NaN precedence rules)
 */
template<typename Type>
Type max(const Type x, const Type y)
{
	bool x_is_nan = std::isnan(x);
	bool y_is_nan = std::isnan(y);

	// take the non-nan value if there is one
	if (x_is_nan || y_is_nan) {
		if (x_is_nan && !y_is_nan) {
			return y;
		}

		// either !x_is_nan && y_is_nan or both are NAN anyways
		return x;
	}

	return (x > y) ? x : y;
}

/**
 * @brief Constrain a value to a range with NaN handling
 * 
 * @tparam Type Numeric type
 * @param x Value to constrain
 * @param lower_bound Lower bound (inclusive)
 * @param upper_bound Upper bound (inclusive)
 * @return Constrained value, or NaN if bounds are invalid or x is NaN
 * 
 * @pre lower_bound <= upper_bound
 */
template<typename Type>
Type constrain(const Type x, const Type lower_bound, const Type upper_bound)
{
	if (lower_bound > upper_bound) {
		return NAN;

	} else if (std::isnan(x)) {
		return NAN;

	} else {
		return typeFunction::max(lower_bound, typeFunction::min(upper_bound, x));
	}
}
}

// ============================================================================
// Matrix Element-Wise Operation Wrappers
// ============================================================================

/**
 * @brief Element-wise minimum with scalar upper bound
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param x Input matrix
 * @param scalar_upper_bound Upper bound scalar
 * @return Matrix where each element = min(x(i,j), scalar_upper_bound)
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(const Matrix<Type, M, N> &x, const Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x(i, j), scalar_upper_bound);
		}
	}

	return m;
}

/**
 * @brief Element-wise minimum with scalar upper bound (commutative version)
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(const Type scalar_upper_bound, const Matrix<Type, M, N> &x)
{
	return min(x, scalar_upper_bound);
}

/**
 * @brief Element-wise minimum of two matrices
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param x1 First matrix
 * @param x2 Second matrix
 * @return Matrix where each element = min(x1(i,j), x2(i,j))
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> min(const Matrix<Type, M, N> &x1, const Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::min(x1(i, j), x2(i, j));
		}
	}

	return m;
}

/**
 * @brief Element-wise maximum with scalar lower bound
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param x Input matrix
 * @param scalar_lower_bound Lower bound scalar
 * @return Matrix where each element = max(x(i,j), scalar_lower_bound)
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(const Matrix<Type, M, N> &x, const Type scalar_lower_bound)
{
	Matrix<Type, M, N> m;

	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x(i, j), scalar_lower_bound);
		}
	}

	return m;
}

/**
 * @brief Element-wise maximum with scalar lower bound (commutative version)
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(const Type scalar_lower_bound, const Matrix<Type, M, N> &x)
{
	return max(x, scalar_lower_bound);
}

/**
 * @brief Element-wise maximum of two matrices
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param x1 First matrix
 * @param x2 Second matrix
 * @return Matrix where each element = max(x1(i,j), x2(i,j))
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> max(const Matrix<Type, M, N> &x1, const Matrix<Type, M, N> &x2)
{
	Matrix<Type, M, N> m;

	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::max(x1(i, j), x2(i, j));
		}
	}

	return m;
}

/**
 * @brief Constrain matrix elements to a scalar range
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param x Input matrix
 * @param scalar_lower_bound Lower bound (inclusive)
 * @param scalar_upper_bound Upper bound (inclusive)
 * @return Matrix with each element constrained to [lower, upper]
 * 
 * @pre scalar_lower_bound <= scalar_upper_bound
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(const Matrix<Type, M, N> &x,
			     const Type scalar_lower_bound,
			     const Type scalar_upper_bound)
{
	Matrix<Type, M, N> m;

	if (scalar_lower_bound > scalar_upper_bound) {
		m.setNaN();

	} else {
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				m(i, j) = typeFunction::constrain(x(i, j), scalar_lower_bound, scalar_upper_bound);
			}
		}
	}

	return m;
}

/**
 * @brief Constrain matrix elements element-wise with matrix bounds
 * 
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param x Input matrix
 * @param x_lower_bound Lower bound matrix
 * @param x_upper_bound Upper bound matrix
 * @return Matrix with each element constrained to [lower(i,j), upper(i,j)]
 * 
 * @pre For all i,j: x_lower_bound(i,j) <= x_upper_bound(i,j)
 */
template<typename Type, size_t  M, size_t N>
Matrix<Type, M, N> constrain(const Matrix<Type, M, N> &x,
			     const Matrix<Type, M, N> &x_lower_bound,
			     const Matrix<Type, M, N> &x_upper_bound)
{
	Matrix<Type, M, N> m;

	for (size_t i = 0; i < M; i++) {
		for (size_t j = 0; j < N; j++) {
			m(i, j) = typeFunction::constrain(x(i, j), x_lower_bound(i, j), x_upper_bound(i, j));
		}
	}

	return m;
}

// ============================================================================
// Stream Output Operator
// ============================================================================

/**
 * @brief Stream output operator for matrix
 * 
 * Allows matrices to be written to output streams (e.g., std::cout, std::ostringstream).
 * 
 * @tparam OStream Output stream type
 * @tparam Type Element type
 * @tparam M Number of rows
 * @tparam N Number of columns
 * @param os Output stream
 * @param matrix Matrix to output
 * @return Reference to output stream for chaining
 * 
 * @example
 * @code
 * matrix::Matrix<float, 3, 3> I;
 * I.identity();
 * std::cout << I << std::endl;
 * @endcode
 */
template<typename OStream, typename Type, size_t M, size_t N>
OStream &operator<<(OStream &os, const matrix::Matrix<Type, M, N> &matrix)
{
	os << "\n";
	// element: tab, point, 8 digits, 4 scientific notation chars; row: newline; string: \0 end
	static const size_t n = 15 * N * M + M + 1;
	char string[n];
	matrix.write_string(string, n);
	os << string;
	return os;
}

} // namespace matrix
