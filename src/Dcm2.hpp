/****************************************************************************
 *
 *   Copyright (C) 2023 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file Dcm2.hpp
 * @brief Two-dimensional direction cosine matrix rotation type.
 *
 * A givens rotation matrix 2x2.
 * All rotations and axis systems follow the right-hand rule.
 *
 * This library uses the convention that premultiplying a two dimensional
 * vector represented in coordinate system 1 will apply a rotation from coordinate system
 * 1 to coordinate system 2 to the vector.
 * Likewise, a matrix instance of this class also represents a coordinate transformation
 * from frame 2 to frame 1.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include "SquareMatrix.hpp"
#include "Vector2.hpp"

namespace matrix
{

/** @brief Two-dimensional rotation represented by a 2x2 direction cosine matrix.
 * @tparam Type Floating-point scalar type.
 */
template<typename Type>
class Dcm2 : public SquareMatrix<Type, 2>
{
public:
	/**
	 * @brief Construct the identity rotation.
	 *
	 * Initializes to identity
	 */
	Dcm2() : SquareMatrix<Type, 2>(eye<Type, 2>()) {}

	/**
	 * @brief Construct from a 2x2 row-major array.
	 *
	 * @param data Pointer to four values arranged by row.
	 */
	explicit Dcm2(const Type data_[2][2]) : SquareMatrix<Type, 2>(data_)
	{
	}

	/**
	 * @brief Construct from a flat row-major array.
	 *
	 * @param data Pointer to four values arranged by row.
	 */
	explicit Dcm2(const Type data_[4]) : SquareMatrix<Type, 2>(data_)
	{
	}

	/**
	 * @brief Construct from a general 2x2 matrix.
	 *
	 * @param other Source matrix; it is assumed to represent a valid rotation.
	 */
	Dcm2(const Matrix<Type, 2, 2> &other) : SquareMatrix<Type, 2>(other)
	{
	}

	/**
	 * @brief Construct from a rotation angle.
	 *
	 * This sets the transformation matrix from frame 2 to frame 1 where the rotation
	 * from frame 1 to frame 2 is described by an angle in radians.
	 *
	 * @param angle Rotation angle in radians.
	 */
	Dcm2(const Type angle)
	{
		Dcm2 &dcm = *this;
		// Cache sine and cosine because each is used twice.
		Type sin_angle = std::sin(angle);
		Type cos_angle = std::cos(angle);

		dcm(0, 0) = cos_angle;
		dcm(0, 1) = -sin_angle;
		dcm(1, 0) = sin_angle;
		dcm(1, 1) = cos_angle;
	}

	/** @brief Normalize both rows to limit accumulated roundoff.
	 * @note This does not enforce orthogonality between rows. */
	void renormalize()
	{
		// Normalize each row as a 2D vector and store it back in place.
		for (size_t r = 0; r < 2; r++) {
			Vector2<Type> rvec(Matrix<Type, 1, 2>(this->row(r)).transpose());
			this->row(r) = rvec.normalized();
		}
	}
};

/** Single-precision 2D direction cosine matrix. */
using Dcm2f = Dcm2<float>;
/** Double-precision 2D direction cosine matrix. */
using Dcm2d = Dcm2<double>;

} // namespace matrix
