/**
 * @file Dcm.hpp
 * @brief Three-dimensional direction cosine matrix rotation type.
 *
 * A direction cosine matrix class.
 * All rotations and axis systems follow the right-hand rule.
 *
 * This library uses the convention that premultiplying a three dimensional
 * vector represented in coordinate system 1 will apply a rotation from coordinate system
 * 1 to coordinate system 2 to the vector.
 * Likewise, a matrix instance of this class also represents a coordinate transformation
 * from frame 2 to frame 1.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

#include "SquareMatrix.hpp"
#include "Vector3.hpp"

namespace matrix
{

/** Forward declaration of the axis-angle rotation type. */
template<typename Type>
class AxisAngle;

/** Forward declaration of the Euler-angle rotation type. */
template<typename Type>
class Euler;

/** Forward declaration of the quaternion rotation type. */
template<typename Type>
class Quaternion;

/**
 * @brief Three-dimensional rotation represented by a 3x3 direction cosine matrix.
 *
 * The rotation between two coordinate frames is
 * described by this class. The default value is identity.
 * @tparam Type Floating-point scalar type.
 */
template<typename Type>
class Dcm : public SquareMatrix<Type, 3>
{
public:
	/**
	 * @brief Construct the identity rotation.
	 *
	 * Initializes to identity
	 */
	Dcm() : SquareMatrix<Type, 3>(eye<Type, 3>()) {}

	/**
	 * @brief Construct from a 3x3 row-major array.
	 *
	 * @param data Pointer to nine values arranged by row.
	 */
	explicit Dcm(const Type data_[3][3]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * @brief Construct from a flat row-major array.
	 *
	 * @param data Pointer to nine values arranged by row.
	 */
	explicit Dcm(const Type data_[9]) : SquareMatrix<Type, 3>(data_)
	{
	}

	/**
	 * @brief Construct from a general 3x3 matrix.
	 *
	 * @param other Source matrix; it is assumed to represent a valid rotation.
	 */
	Dcm(const Matrix<Type, 3, 3> &other) : SquareMatrix<Type, 3>(other)
	{
	}

	/**
	 * @brief Construct from a quaternion.
	 *
	 * Instance is initialized from quaternion representing
	 * coordinate transformation from frame 2 to frame 1.
	 *
	 * @param q Quaternion describing the coordinate transformation.
	 */
	Dcm(const Quaternion<Type> &q)
	{
		Dcm &dcm = *this;
		const Type a = q(0);
		const Type b = q(1);
		const Type c = q(2);
		const Type d = q(3);
		// Cache pairwise products because each appears in two matrix entries.
		const Type ab = a * b;
		const Type ac = a * c;
		const Type ad = a * d;
		const Type bb = b * b;
		const Type bc = b * c;
		const Type bd = b * d;
		const Type cc = c * c;
		const Type cd = c * d;
		const Type dd = d * d;
		dcm(0, 0) = Type(1) - Type(2) * (cc + dd);
		dcm(0, 1) = Type(2) * (bc - ad);
		dcm(0, 2) = Type(2) * (ac + bd);
		dcm(1, 0) = Type(2) * (bc + ad);
		dcm(1, 1) = Type(1) - Type(2) * (bb + dd);
		dcm(1, 2) = Type(2) * (cd - ab);
		dcm(2, 0) = Type(2) * (bd - ac);
		dcm(2, 1) = Type(2) * (ab + cd);
		dcm(2, 2) = Type(1) - Type(2) * (bb + cc);
	}

	/**
	 * @brief Construct from 3-2-1 intrinsic Tait-Bryan angles.
	 *
	 * This sets the transformation matrix from frame 2 to frame 1 where the rotation
	 * from frame 1 to frame 2 is described by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 *
	 * @param euler Roll, pitch, and yaw rotation.
	 */
	Dcm(const Euler<Type> &euler)
	{
		Dcm &dcm = *this;
		// Evaluate each trigonometric term once and reuse it in the matrix.
		Type cosPhi = Type(std::cos(euler.phi()));
		Type sinPhi = Type(std::sin(euler.phi()));
		Type cosThe = Type(std::cos(euler.theta()));
		Type sinThe = Type(std::sin(euler.theta()));
		Type cosPsi = Type(std::cos(euler.psi()));
		Type sinPsi = Type(std::sin(euler.psi()));

		dcm(0, 0) = cosThe * cosPsi;
		dcm(0, 1) = -cosPhi * sinPsi + sinPhi * sinThe * cosPsi;
		dcm(0, 2) = sinPhi * sinPsi + cosPhi * sinThe * cosPsi;

		dcm(1, 0) = cosThe * sinPsi;
		dcm(1, 1) = cosPhi * cosPsi + sinPhi * sinThe * sinPsi;
		dcm(1, 2) = -sinPhi * cosPsi + cosPhi * sinThe * sinPsi;

		dcm(2, 0) = -sinThe;
		dcm(2, 1) = sinPhi * cosThe;
		dcm(2, 2) = cosPhi * cosThe;
	}


	/**
	 * @brief Construct from an axis-angle rotation vector.
	 *
	 * This sets the transformation matrix from frame 2 to frame 1 where the rotation
	 * from frame 1 to frame 2 is described by a 3-2-1 intrinsic Tait-Bryan rotation sequence.
	 *
	 *
	 * @param aa Axis-angle rotation vector.
	 */
	Dcm(const AxisAngle<Type> &aa)
	{
		Dcm &dcm = *this;
		dcm = Quaternion<Type>(aa);
	}

	/** @brief Apply the inverse of the skew-symmetric hat mapping to the
	 * off-diagonal elements of this matrix.
	 * @return Three-vector containing [-R(1,2), R(0,2), -R(0,1)]. */
	Vector3<Type> vee() const
	{
		// These entries are the independent components of the skew part.
		const Dcm &A(*this);
		return {-A(1, 2), A(0, 2), -A(0, 1)};
	}

	/** @brief Normalize each row independently to limit accumulated roundoff.
	 * @note This does not enforce mutual orthogonality between rows. */
	void renormalize()
	{
		// Convert each row to a vector, normalize it, then write it back.
		for (size_t r = 0; r < 3; r++) {
			matrix::Vector3<Type> rvec(Matrix<Type, 1, 3>(this->Matrix<Type, 3, 3>::row(r)).transpose());
			this->Matrix<Type, 3, 3>::row(r) = rvec.normalized();
		}
	}
};

/** Single-precision 3D direction cosine matrix. */
using Dcmf = Dcm<float>;
/** Double-precision 3D direction cosine matrix. */
using Dcmd = Dcm<double>;

} // namespace matrix
