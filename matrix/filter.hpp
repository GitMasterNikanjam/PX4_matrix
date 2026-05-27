/**
 * @file filter.hpp
 * @brief Kalman filter update (correction) step
 * 
 * This file provides a function that computes the Kalman filter correction
 * step for a linear system with Gaussian noise. It calculates the state
 * correction vector `dx`, the state covariance update `dP`, and the
 * normalised innovation squared `beta` (Mahalanobis distance).
 * 
 * The function implements:
 *   S = C * P * C^T + R
 *   K = P * C^T * S^{-1}
 *   dx = K * r
 *   beta = r^T * S^{-1} * r
 *   dP = -K * C * P
 * 
 * @author PX4 Development Team
 * @ingroup matrix
 */

#pragma once

#include "Scalar.hpp"
#include "SquareMatrix.hpp"

namespace matrix
{
/**
 * @brief Kalman filter correction step
 * 
 * Given prior state covariance P, measurement matrix C, measurement noise
 * covariance R, and innovation residual r, this function computes:
 * - State correction dx (to be added to the prior state estimate)
 * - Covariance update dP (to be added to the prior covariance)
 * - Normalised innovation squared beta (Mahalanobis distance, chi‑square distributed)
 * 
 * The formulas are:
 *   S = C * P * C^T + R
 *   K = P * C^T * S^{-1}
 *   dx = K * r
 *   beta = r^T * S^{-1} * r
 *   dP = -K * C * P
 * 
 * @tparam Type Floating‑point type (float, double)
 * @tparam M Dimension of state vector
 * @tparam N Dimension of measurement vector
 * 
 * @param P Prior state covariance matrix (M×M)
 * @param C Measurement matrix (N×M)
 * @param R Measurement noise covariance matrix (N×N)
 * @param r Innovation residual (N×1), i.e., measurement minus expected measurement
 * @param[out] dx Computed state correction vector (M×1)
 * @param[out] dP Computed covariance update (M×M) – after correction: P_new = P + dP
 * @param[out] beta Normalised innovation squared (scalar) – a chi‑square test statistic
 * 
 * @return 0 on success (always succeeds, but may produce NaN if inversion fails)
 * 
 * @note This function does not check for positive definiteness or invertibility;
 *       if S is singular, the result will be invalid (NaNs). The caller must ensure
 *       that R is positive definite and C*P*C^T + R is invertible.
 * 
 * @warning The covariance update uses the Joseph form equivalent: 
 *          P_new = (I - K*C) * P. Here we return dP = -K*C*P so that
 *          P_new = P + dP. This is valid for the standard Kalman gain.
 * 
 * @see Scalar, SquareMatrix
 */
template<typename Type, size_t M, size_t N>
int kalman_correct(
	const Matrix<Type, M, M> &P,
	const Matrix<Type, N, M> &C,
	const Matrix<Type, N, N> &R,
	const Matrix<Type, N, 1> &r,
	Matrix<Type, M, 1> &dx,
	Matrix<Type, M, M> &dP,
	Type &beta
)
{
	SquareMatrix<Type, N> S_I = SquareMatrix<Type, N>(C * P * C.T() + R).I();
	Matrix<Type, M, N> K = P * C.T() * S_I;
	dx = K * r;
	beta = Scalar<Type>(r.T() * S_I * r);
	dP = K * C * P * (-1);
	return 0;
}

} // namespace matrix
