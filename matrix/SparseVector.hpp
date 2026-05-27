/**
 * @file SparseVector.hpp
 * @brief Sparse vector class storing only non‑zero elements specified at compile time
 * 
 * This file provides a template class for sparse vectors where the indices of
 * non‑zero entries are given as a template parameter pack. The class stores
 * only those elements, enabling memory‑efficient representation and fast
 * operations when most entries are zero.
 * 
 * Key features:
 * - Compile‑time fixed indices (parameter pack)
 * - No storage for zero entries
 * - Access via compile‑time index (`at<i>()`) or compressed index
 * - Dot product with dense vectors
 * - Addition to dense vectors
 * - Norm and norm squared
 * - Multiplication with dense matrices (via global `operator*`)
 * - Quadratic form `xᵀ A x` optimisation (global `quadraticForm`)
 * - Type alias `SparseVectorf` for float
 * 
 * @author Kamil Ritz <kritz@ethz.ch>
 * @author Julian Kent <julian@auterion.com>
 * @ingroup matrix
 */

// #####################################################################

#pragma once

#include "Vector.hpp"

// #####################################################################

namespace matrix
{
/**
 * @brief Helper to force compile‑time evaluation of an expression as a constant
 * 
 * This template is used to convert a `constexpr` function result into a
 * compile‑time constant, required for `static_assert` and array sizes.
 * 
 * @tparam N Value to be forced as compile‑time constant
 */
template<int N> struct force_constexpr_eval {
	static const int value = N;
};

/**
 * @brief Sparse vector with fixed non‑zero indices
 * 
 * SparseVector<Type, M, Idxs...> stores only the elements at positions
 * given by the index pack `Idxs`. All other indices are implicitly zero.
 * The total number of stored elements is `N = sizeof...(Idxs)`.
 * 
 * @tparam Type Element type (float, double, int, etc.)
 * @tparam M Total length of the vector (including zeros)
 * @tparam Idxs Indices (0‑based) of the non‑zero elements (must be unique, < M)
 * 
 * @note The indices are checked at compile time for:
 *       - Uniqueness
 *       - Non‑emptiness (at least one element)
 *       - Fit within `[0, M-1]`
 * 
 * @code
 * // Sparse vector of length 10 with non‑zeros at indices 0, 2, 5
 * SparseVector<float, 10, 0, 2, 5> sv;
 * sv.at<0>() = 1.0f;
 * sv.at<2>() = 3.0f;
 * sv.at<5>() = 4.0f;
 * 
 * // Dot product with dense vector
 * Vector<float, 10> dense;
 * float dot = sv.dot(dense);
 * @endcode
 */
template<typename Type, size_t M, size_t... Idxs>
class SparseVector
{
private:
	/** Number of stored non‑zero elements */
	static constexpr size_t N = sizeof...(Idxs);

	/** Array of indices (compile‑time) */
	static constexpr size_t _indices[N] {Idxs...};

	/**
	 * @brief Check for duplicate indices at compile time
	 * @return true if any index appears twice
	 */
	static constexpr bool duplicateIndices()
	{
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < i; j++) {
				if (_indices[i] == _indices[j]) {
					return true;
				}
			}
		}

		return false;
	}

	/**
	 * @brief Find the maximum index value
	 * @return Largest index among the pack
	 */
	static constexpr size_t findMaxIndex()
	{
		size_t maxIndex = 0;

		for (size_t i = 0; i < N; i++) {
			if (maxIndex < _indices[i]) {
				maxIndex = _indices[i];
			}
		}

		return maxIndex;
	}

	// Compile‑time sanity checks
	static_assert(!duplicateIndices(), "Duplicate indices");
	static_assert(N < M, "More entries than elements, use a dense vector");
	static_assert(N > 0, "A sparse vector needs at least one element");
	static_assert(findMaxIndex() < M, "Largest entry doesn't fit in sparse vector");

	Type _data[N] {};

	/**
	 * @brief Map a compile‑time index to its compressed position
	 * @param index The original index (must be one of Idxs)
	 * @return Compressed position (0 <= pos < N) or -1 if not found
	 */
	static constexpr int findCompressedIndex(size_t index)
	{
		int compressedIndex = -1;

		for (size_t i = 0; i < N; i++) {
			if (index == _indices[i]) {
				compressedIndex = static_cast<int>(i);
			}
		}

		/** Storage for the non‑zero values (size N) */
		return compressedIndex;
	}

public:
	/**
	 * @brief Return the number of stored non‑zero elements
	 * @return N (compile‑time constant)
	 */
	constexpr size_t non_zeros() const
	{
		return N;
	}

	/**
	 * @brief Return the i‑th stored index (in the order of the parameter pack)
	 * @param i Position in the compressed array (0 <= i < N)
	 * @return Original index of the i‑th stored element
	 */
	constexpr size_t index(size_t i) const
	{
		return SparseVector::_indices[i];
	}

	/**
	 * @brief Default constructor – zero‑initialises all stored elements
	 */
	SparseVector() = default;

	/**
	 * @brief Construct from a dense vector (copies only the specified indices)
	 * @param data Dense vector of length M
	 */
	SparseVector(const matrix::Vector<Type, M> &data)
	{
		for (size_t i = 0; i < N; i++) {
			_data[i] = data(_indices[i]);
		}
	}

	/**
	 * @brief Construct from a C array of N values (order must match the index pack)
	 * @param data Array of length N, stored in the same order as Idxs
	 */
	explicit SparseVector(const Type data[N])
	{
		memcpy(_data, data, sizeof(_data));
	}

	/**
	 * @brief Read‑only access to the element at compile‑time index i
	 * @tparam i Original index (must be one of Idxs)
	 * @return Value of the element at index i
	 * 
	 * @note Compile‑time checked: invalid indices cause a `static_assert`.
	 */
	template <size_t i>
	inline Type at() const
	{
		static constexpr int compressed_index = force_constexpr_eval<findCompressedIndex(i)>::value;
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		return _data[compressed_index];
	}

	/**
	 * @brief Mutable access to the element at compile‑time index i
	 * @tparam i Original index (must be one of Idxs)
	 * @return Reference to the element at index i
	 */
	template <size_t i>
	inline Type &at()
	{
		static constexpr int compressed_index = force_constexpr_eval<findCompressedIndex(i)>::value;
		static_assert(compressed_index >= 0, "cannot access unpopulated indices");
		return _data[compressed_index];
	}

	/**
	 * @brief Read‑only access by compressed index (runtime)
	 * @param i Compressed position (0 ≤ i < N)
	 * @return Value at compressed index i
	 * 
	 * @pre i < N (asserted in debug builds)
	 */
	inline Type atCompressedIndex(size_t i) const
	{
		assert(i < N);
		return _data[i];
	}

	/**
	 * @brief Mutable access by compressed index (runtime)
	 * @param i Compressed position (0 ≤ i < N)
	 * @return Reference to element at compressed index i
	 */
	inline Type &atCompressedIndex(size_t i)
	{
		assert(i < N);
		return _data[i];
	}

	/**
	 * @brief Set all stored elements to zero
	 */
	void setZero()
	{
		for (size_t i = 0; i < N; i++) {
			_data[i] = Type(0);
		}
	}

	/**
	 * @brief Dot product with a dense vector
	 * @param other Dense vector of length M
	 * @return Sum over non‑zero indices of `this[i] * other[i]`
	 */
	Type dot(const matrix::Vector<Type, M> &other) const
	{
		Type accum(0);

		for (size_t i = 0; i < N; i++) {
			accum += _data[i] * other(_indices[i]);
		}

		return accum;
	}

	/**
	 * @brief Add this sparse vector to a dense vector
	 * @param other Dense vector (will be copied)
	 * @return New dense vector = other + this (sparse added only at stored indices)
	 */
	matrix::Vector<Type, M> operator+(const matrix::Vector<Type, M> &other) const
	{
		matrix::Vector<Type, M> vec = other;

		for (size_t i = 0; i < N; i++) {
			vec(_indices[i]) +=  _data[i];
		}

		return vec;
	}

	/**
	 * @brief Add a scalar to every stored element (in‑place)
	 * @param t Scalar value to add
	 * @return Reference to this sparse vector
	 */
	SparseVector &operator+=(Type t)
	{
		for (size_t i = 0; i < N; i++) {
			_data[i] += t;
		}

		return *this;
	}

	/**
	 * @brief Squared Euclidean norm (sum of squares of stored elements)
	 * @return `∑(this[i]²)` over stored indices
	 */
	Type norm_squared() const
	{
		Type accum(0);

		for (size_t i = 0; i < N; i++) {
			accum += _data[i] * _data[i];
		}

		return accum;
	}

	/**
	 * @brief Euclidean norm
	 * @return `√norm_squared()`
	 */
	Type norm() const
	{
		return std::sqrt(norm_squared());
	}

	/**
	 * @brief Check if the norm exceeds a threshold (using squared norm for efficiency)
	 * @param testVal Threshold value
	 * @return true if `norm() > testVal`
	 */
	bool longerThan(Type testVal) const
	{
		return norm_squared() > testVal * testVal;
	}
};

// -----------------------------------------------------------------------------
// Global operators and functions
// -----------------------------------------------------------------------------

/**
 * @brief Multiply a dense matrix by a sparse vector
 * 
 * Computes `result = mat * vec`, where `vec` is sparse.
 * The multiplication is performed by computing the dot product of each row
 * of the matrix with the sparse vector.
 * 
 * @tparam Type Element type
 * @tparam Q Number of rows of the matrix (result vector dimension)
 * @tparam M Length of the vector (columns of the matrix)
 * @tparam Idxs Indices of non‑zero entries in the sparse vector
 * @param mat Dense matrix of size Q × M
 * @param vec Sparse vector of length M
 * @return Dense vector of length Q = mat * vec
 */
template<typename Type, size_t Q, size_t M, size_t ... Idxs>
matrix::Vector<Type, Q> operator*(const matrix::Matrix<Type, Q, M> &mat,
				  const matrix::SparseVector<Type, M, Idxs...> &vec)
{
	matrix::Vector<Type, Q> res;

	for (size_t i = 0; i < Q; i++) {
		const Vector<Type, M> row = mat.row(i);
		res(i) = vec.dot(row);
	}

	return res;
}

/**
 * @brief Compute quadratic form `xᵀ A x` efficiently for a sparse vector
 * 
 * Evaluates `xᵀ A x` by only iterating over the non‑zero indices of `x`.
 * Complexity O(N²) where N is the number of non‑zeros, rather than O(M²).
 * 
 * @tparam Type Element type
 * @tparam M Size of the square matrix and vector
 * @tparam Idxs Non‑zero indices of the sparse vector
 * @param A Dense square matrix of size M×M
 * @param x Sparse vector of length M
 * @return Scalar value `xᵀ A x`
 * 
 * @note The matrix `A` does not need to be symmetric; the quadratic form
 *       uses the full matrix.
 */
template<typename Type, size_t M, size_t ... Idxs>
Type quadraticForm(const matrix::SquareMatrix<Type, M> &A, const matrix::SparseVector<Type, M, Idxs...> &x)
{
	Type res = Type(0);

	for (size_t i = 0; i < x.non_zeros(); i++) {
		Type tmp = Type(0);

		for (size_t j = 0; j < x.non_zeros(); j++) {
			tmp += A(x.index(i), x.index(j)) * x.atCompressedIndex(j);
		}

		res += x.atCompressedIndex(i) * tmp;
	}

	return res;
}

// -----------------------------------------------------------------------------
// Static member definition (required for ODR)
// -----------------------------------------------------------------------------

/**
 * @brief Storage for the compile‑time index array (definition)
 */
template<typename Type, size_t M, size_t... Idxs>
constexpr size_t SparseVector<Type, M, Idxs...>::_indices[SparseVector<Type, M, Idxs...>::N];

// -----------------------------------------------------------------------------
// Type alias for convenience
// -----------------------------------------------------------------------------

/**
 * @brief Sparse vector with float elements
 * 
 * @tparam M Total length of the vector
 * @tparam Idxs Indices of non‑zero elements
 */
template<size_t M, size_t ... Idxs>
using SparseVectorf = SparseVector<float, M, Idxs...>;

}
