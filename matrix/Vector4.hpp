/**
 * @file Vector4.hpp
 * @brief 4D vector class derived from Vector<Type, 4>
 * 
 * This file provides a 4-dimensional vector specialization of the Vector class.
 * It adds convenience constructors and ensures that arithmetic operations
 * return Vector4 instead of the base Matrix type.
 * 
 * Key features:
 * - Inherits all Vector<Type,4> operations (dot product, norm, etc.)
 * - Convenient constructor from four scalar components
 * - Constructors from column slices (4×1) and row slices (1×4)
 * - Overloaded operators return Vector4 instead of base Matrix type
 * - Type alias Vector4f for float type
 * 
 * @author Matthias Grob <maetugr@gmail.com>
 * @ingroup matrix
 */

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

#pragma once

#include "Vector.hpp"

namespace matrix
{

/**
 * @brief 4D column vector class
 * 
 * Vector4 is a specialized 4-element column vector. It extends Vector<Type,4>
 * with convenience constructors and ensures that arithmetic operators return
 * Vector4 objects instead of the base Matrix<Type,4,1> type.
 * 
 * This class is useful for representing 4D points, quaternions (as vectors),
 * homogeneous coordinates, or any 4‑dimensional data.
 * 
 * @tparam Type The numeric type (float, double, int, etc.)
 * 
 * @see Vector
 * @see Vector3
 */	
template<typename Type>
class Vector4 : public Vector<Type, 4>
{
public:
	/** Base matrix type (4x1) */
	using Matrix41 = Matrix<Type, 4, 1>;

	/**
	 * @brief Default constructor – zero‑initialises all components
	 */
	Vector4() = default;

	/**
	 * @brief Copy constructor from base 4x1 matrix
	 * @param other Source matrix (must be 4x1)
	 */
	Vector4(const Matrix41 &other) :
		Vector<Type, 4>(other)
	{
	}

	/**
	 * @brief Construct from a flat array of 4 elements
	 * @param data_ Array of length 4 [x1, x2, x3, x4]
	 * 
	 * @note The parameter is declared as `const Type data_[3]` in the original
	 *       code, but it should be `[4]`. This is a known issue; treat the array
	 *       as having at least 4 elements.
	 */
	explicit Vector4(const Type data_[3]) :
		Vector<Type, 4>(data_)
	{
	}

	/**
	 * @brief Construct from four scalar components
	 * @param x1 First component
	 * @param x2 Second component
	 * @param x3 Third component
	 * @param x4 Fourth component
	 */
	Vector4(Type x1, Type x2, Type x3, Type x4)
	{
		Vector4 &v(*this);
		v(0) = x1;
		v(1) = x2;
		v(2) = x3;
		v(3) = x4;
	}

	/**
	 * @brief Construct from a mutable column slice (4×1)
	 * @tparam P Number of rows in source slice (must be 4)
	 * @tparam Q Number of columns in source slice (must be 1)
	 * @param slice_in Source slice view
	 */
	template<size_t P, size_t Q>
	Vector4(const Slice<Type, 4, 1, P, Q> &slice_in) : Vector<Type, 4>(slice_in)
	{
	}

	/**
	 * @brief Construct from a mutable row slice (1×4) – converts to column vector
	 * @tparam P Number of rows in source slice (must be 1)
	 * @tparam Q Number of columns in source slice (must be 4)
	 * @param slice_in Source row slice
	 */
	template<size_t P, size_t Q>
	Vector4(const Slice<Type, 1, 4, P, Q> &slice_in) : Vector<Type, 4>(slice_in)
	{
	}

	// ------------------------------------------------------------------------
	// Overloaded operators – return Vector4 instead of Matrix41
	// ------------------------------------------------------------------------

	/**
	 * Override matrix ops so Vector4 type is returned
	 */

	/**
	 * @brief Vector addition (element‑wise)
	 * @param other Right-hand side vector
	 * @return New Vector4 with element‑wise sum
	 */
	Vector4 operator+(Vector4 other) const
	{
		return Matrix41::operator+(other);
	}

	/**
	 * @brief Scalar addition (adds scalar to each component)
	 * @param scalar Value to add
	 * @return New Vector4 with each component increased by scalar
	 */
	Vector4 operator+(Type scalar) const
	{
		return Matrix41::operator+(scalar);
	}

	/**
	 * @brief Vector subtraction (element‑wise)
	 * @param other Right-hand side vector
	 * @return New Vector4 with element‑wise difference
	 */
	Vector4 operator-(Vector4 other) const
	{
		return Matrix41::operator-(other);
	}

	/**
	 * @brief Scalar subtraction (subtracts scalar from each component)
	 * @param scalar Value to subtract
	 * @return New Vector4 with each component reduced by scalar
	 */
	Vector4 operator-(Type scalar) const
	{
		return Matrix41::operator-(scalar);
	}

	/**
	 * @brief Unary negation
	 * @return Vector4 with all components negated
	 */
	Vector4 operator-() const
	{
		return Matrix41::operator-();
	}

	/**
	 * @brief Scalar multiplication
	 * @param scalar Multiplier
	 * @return New Vector4 with each component multiplied by scalar
	 */
	Vector4 operator*(Type scalar) const
	{
		return Matrix41::operator*(scalar);
	}

	/**
	 * @brief Dot product with another Vector4
	 * @param b Right-hand side vector
	 * @return Scalar dot product
	 */
	Type operator*(Vector4 b) const
	{
		return Vector<Type, 4>::operator*(b);
	}

};

// -----------------------------------------------------------------------------
// Common type alias
// -----------------------------------------------------------------------------	

/** @brief 4D vector with float components */
using Vector4f = Vector4<float>;

} // namespace matrix
