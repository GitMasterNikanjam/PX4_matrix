/**
 * @file PseudoInverse.hpp
 * @brief Fast computation of Moore‑Penrose pseudo‑inverse using full‑rank Cholesky factorisation
 * 
 * This file implements the algorithm described in:
 *   Courrieu, P. (2008). Fast Computation of Moore-Penrose Inverse Matrices, 8(2), 25–29.
 *   http://arxiv.org/abs/0804.4809
 * 
 * The pseudo‑inverse is computed via full‑rank Cholesky factorisation, which is
 * numerically efficient and avoids singular value decomposition for many practical
 * cases (e.g., control systems, least‑squares problems).
 * 
 * Key features:
 * - Works for both over‑determined (M > N) and under‑determined (M < N) matrices.
 * - Uses a tolerance based on the largest diagonal element and machine epsilon.
 * - Returns false if the inversion fails (singular or numerically unstable).
 * 
 * @author Julien Lecoeur <julien.lecoeur@gmail.com>
 * @author Julian Kent <julian@auterion.com>
 * @ingroup matrix
 */

#pragma once

#include "SquareMatrix.hpp"
#include "Vector.hpp"

namespace matrix
{

/**
 * @brief Compute the Moore‑Penrose pseudo‑inverse of a matrix G
 * 
 * This function computes the pseudo‑inverse G⁺ such that:
 *   G⁺ * G * G⁺ = G⁺
 *   G * G⁺ * G = G
 *   (G * G⁺) is Hermitian
 *   (G⁺ * G) is Hermitian
 * 
 * The algorithm chooses between two Cholesky‑based paths depending on whether
 * the matrix is tall (M ≤ N) or wide (M > N):
 *   - If M ≤ N:  uses A = G * Gᵀ  (size M×M)
 *   - If M > N:  uses A = Gᵀ * G   (size N×N)
 * 
 * @tparam Type Floating‑point type (float, double)
 * @tparam M Number of rows of input matrix G
 * @tparam N Number of columns of input matrix G
 * @param G Input matrix (size M×N)
 * @param[out] res Output pseudo‑inverse (size N×M)
 * @return true if computation succeeded, false if matrix is singular or numerically unstable
 * 
 * @note The pseudo‑inverse can be used to solve linear least‑squares problems:
 *       x = G⁺ * y  minimises ‖G x – y‖₂.
 * 
 * @example
 * @code
 * Matrix<float, 3, 2> G;
 * // ... fill G ...
 * Matrix<float, 2, 3> Gpinv;
 * bool ok = geninv(G, Gpinv);
 * @endcode
 */
template<typename Type, size_t M, size_t N>
bool geninv(const Matrix<Type, M, N> &G, Matrix<Type, N, M> &res)
{
	size_t rank;

	if (M <= N) {
		// Over‑determined or square: use G * Gᵀ	
		SquareMatrix<Type, M> A = G * G.transpose();
		SquareMatrix<Type, M> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, M> X;

		if (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			return false; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		// Compute G⁺ = Gᵀ * L * (X² * Lᵀ)
		A = X * X * L.transpose();
		res = G.transpose() * (L * A);

	} else {
		// Under‑determined: use Gᵀ * G
		SquareMatrix<Type, N> A = G.transpose() * G;
		SquareMatrix<Type, N> L = fullRankCholesky(A, rank);

		A = L.transpose() * L;
		SquareMatrix<Type, N> X;

		if (!inv(A, X, rank)) {
			res = Matrix<Type, N, M>();
			return false; // LCOV_EXCL_LINE -- this can only be hit from numerical issues
		}

		// doing an intermediate assignment reduces stack usage
		// Compute G⁺ = (L * X² * Lᵀ) * Gᵀ
		A = X * X * L.transpose();
		res = (L * A) * G.transpose();
	}

	return true;
}

/**
 * @brief Return the machine epsilon for the given floating‑point type
 * @tparam Type Numeric type (specialised for float, double, etc.)
 * @return Machine epsilon (FLT_EPSILON for float, DBL_EPSILON for double, etc.)
 */
template<typename Type>
Type typeEpsilon();

/** @brief Float epsilon */
template<> inline
float typeEpsilon<float>()
{
	return FLT_EPSILON;
}

// Double epsilon can be added if needed, but the file currently only provides float.
// For completeness we could add a double version, but it's not in the original.

/**
 * @brief Full‑rank Cholesky factorisation of a symmetric positive‑definite matrix A
 * 
 * This algorithm computes a lower‑triangular matrix L (with possibly zero columns)
 * such that A = L * Lᵀ, up to the numerical rank.
 * The rank is determined by a tolerance that depends on the machine epsilon and
 * the largest diagonal element of A.
 * 
 * The algorithm follows the description in Courrieu (2008).
 * 
 * @tparam Type Floating‑point type (float, double)
 * @tparam N Matrix dimension
 * @param A Input symmetric positive‑definite matrix (size N×N)
 * @param[out] rank Output numerical rank of A
 * @return Lower‑triangular factor L (size N×N, only first `rank` columns are filled)
 * 
 * @note The function modifies only the lower triangular part of L; the upper triangle
 *       is left untouched (not used). The result is ready for further use in
 *       pseudo‑inverse computation.
 */
template<typename Type, size_t N>
SquareMatrix<Type, N> fullRankCholesky(const SquareMatrix<Type, N> &A,
				       size_t &rank)
{
	// Loses one ulp accuracy per row of diag, relative to largest magnitude
	// Tolerance: N * epsilon * max(diag(A))
	const Type tol = N * typeEpsilon<Type>() * A.diag().max();

	Matrix<Type, N, N> L;		// L is lower triangular, but we store full square for simplicity

	size_t r = 0;				// current rank (number of active columns)

	for (size_t k = 0; k < N; k++) {

		if (r == 0) {
			// First column: copy A(i,k) directly
			for (size_t i = k; i < N; i++) {
				L(i, r) = A(i, k);
			}

		} else {
			// Subtract contributions from already computed columns
			for (size_t i = k; i < N; i++) {
				// Compute LL = L[k:n, :r] * L[k, :r].T
				Type LL = Type();

				for (size_t j = 0; j < r; j++) {
					LL += L(i, j) * L(k, j);
				}

				L(i, r) = A(i, k) - LL;
			}
		}

		// If the diagonal element is large enough, accept this column
		if (L(k, r) > tol) {
			L(k, r) = std::sqrt(L(k, r));		// diagonal element becomes sqrt

			if (k < N - 1) {
				// Normalise the rest of the column
				for (size_t i = k + 1; i < N; i++) {
					L(i, r) = L(i, r) / L(k, r);
				}
			}

			r = r + 1;	// increase rank
		}
	}

	// Return rank
	rank = r;

	return L;
}

} // namespace matrix
