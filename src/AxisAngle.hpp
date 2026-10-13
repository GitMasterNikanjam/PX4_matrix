/**
 * @file AxisAngle.hpp
 * @brief Axis-angle rotation representation.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#include "Vector3.hpp"

namespace matrix
{

/** Forward declaration of the 3-2-1 Euler-angle representation. */
template <typename Type>
class Euler;

/** Forward declaration of the quaternion rotation representation. */
template<typename Type>
class Quaternion;

/**
 * @brief Rotation vector whose direction is the rotation axis and length is
 * the rotation angle in radians.
 *
 * A zero vector represents no rotation. The class inherits Vector3 so the
 * three components are available through the usual vector interface.
 * @tparam Type Floating-point scalar type.
 */
template<typename Type>
class AxisAngle : public Vector3<Type>
{
public:
	using Matrix31 = Matrix<Type, 3, 1>;

	/**
	 * @brief Construct from three row-major axis-angle components.
	 *
	 * @param data_ Pointer to three values [rx*angle, ry*angle, rz*angle].
	 */
	explicit AxisAngle(const Type data_[3]) :
		Vector3<Type>(data_)
	{
	}

	/**
	 * @brief Construct the zero rotation.
	 */
	AxisAngle() = default;

	/**
	 * @brief Construct from a three-element column matrix.
	 *
	 * @param other Source 3x1 matrix.
	 */
	AxisAngle(const Matrix31 &other) :
		Vector3<Type>(other)
	{
	}

	/**
	 * @brief Convert a quaternion to an axis-angle rotation vector.
	 *
	 * This sets the instance from a quaternion representing coordinate transformation from
	 * frame 2 to frame 1 where the rotation from frame 1 to frame 2 is described
	 * by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 * @param q Quaternion describing the rotation.
	 */
	AxisAngle(const Quaternion<Type> &q)
	{
		AxisAngle &v = *this;
		// The quaternion vector part is axis * sin(angle/2); its norm recovers
		// the half-angle sine and its direction is the rotation axis.
		Type mag = q.imag().norm();

		if (std::fabs(mag) >= Type(1e-10)) {
			// atan2 remains well-conditioned near zero rotation; scale the
			// quaternion vector so the resulting vector length is the full angle.
			v = q.imag() * Type(Type(2) * std::atan2(mag, q(0)) / mag);

		} else {
			// Near zero, avoid division by a tiny magnitude and retain the
			// quaternion's sign convention for the limiting rotation vector.
			v = q.imag() * Type(Type(2) * Type(sign(q(0))));
		}
	}

	/**
	 * @brief Construct from a direction cosine matrix.
	 *
	 * Instance is initialized from a dcm representing coordinate transformation
	 * from frame 2 to frame 1.
	 *
	 * @param dcm Direction cosine matrix describing the rotation.
	 */
	AxisAngle(const Dcm<Type> &dcm)
	{
		AxisAngle &v = *this;
		v = AxisAngle<Type>(Quaternion<Type>(dcm));
	}

	/**
	 * @brief Construct from 3-2-1 intrinsic Euler angles.
	 *
	 * This sets the instance to a quaternion representing coordinate transformation from
	 * frame 2 to frame 1 where the rotation from frame 1 to frame 2 is described
	 * by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 * @param euler Source Euler-angle rotation.
	 */
	AxisAngle(const Euler<Type> &euler)
	{
		AxisAngle &v = *this;
		v = AxisAngle<Type>(Quaternion<Type>(euler));
	}

	/**
	 * @brief Construct directly from rotation-vector components.
	 *
	 * @param x Rotation-vector component on X, in radians.
	 * @param y Rotation-vector component on Y, in radians.
	 * @param z Rotation-vector component on Z, in radians.
	 */
	AxisAngle(Type x, Type y, Type z)
	{
		AxisAngle &v = *this;
		v(0) = x;
		v(1) = y;
		v(2) = z;
	}

	/**
	 * @brief Construct from an axis and an angle.
	 *
	 * @param axis Rotation axis; normalized by this constructor.
	 * @param angle Rotation angle in radians.
	 */
	AxisAngle(const Matrix31 &axis_, Type angle_)
	{
		AxisAngle &v = *this;
		// Normalize before scaling so the vector length equals the angle.
		Vector3<Type> a = axis_;
		a = a.unit();
		v(0) = a(0) * angle_;
		v(1) = a(1) * angle_;
		v(2) = a(2) * angle_;
	}


	/** @brief Return the unit rotation axis (X axis for the zero rotation). */
	Vector3<Type> axis()
	{
		// A zero rotation has no unique axis; use the documented X-axis default.
		if (Vector3<Type>::norm() > 0) {
			return Vector3<Type>::unit();

		} else {
			return Vector3<Type>(1, 0, 0);
		}
	}

	/** @brief Return the rotation angle in radians, in the range [0, pi] for
	 * rotations produced by a normalized quaternion. */
	Type angle()
	{
		return Vector3<Type>::norm();
	}
};

/** Single-precision axis-angle rotation. */
using AxisAnglef = AxisAngle<float>;
/** Double-precision axis-angle rotation. */
using AxisAngled = AxisAngle<double>;

} // namespace matrix
