/**
 * @file Euler.hpp
 * @brief 3-2-1 intrinsic Tait-Bryan angle representation.
 *
 * All rotations and axis systems follow the right-hand rule
 *
 * An instance of this class defines a rotation from coordinate frame 1 to coordinate frame 2.
 * It follows the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.
 * In order to go from frame 1 to frame 2 we apply the following rotations consecutively.
 * 1) We rotate about our initial Z axis by an angle of _psi.
 * 2) We rotate about the newly created Y' axis by an angle of _theta.
 * 3) We rotate about the newly created X'' axis by an angle of _phi.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#pragma once

namespace matrix
{

/**
 * @brief Roll, pitch, and yaw angles for a 3-2-1 intrinsic rotation.
 *
 * This class describes the rotation from frame 1
 * to frame 2 via 3-2-1 intrinsic Tait-Bryan rotation sequence. Angles are
 * stored in radians and exposed in the order roll (phi), pitch (theta), yaw
 * (psi).
 * @tparam Type Floating-point scalar type.
 */
template<typename Type>
class Euler : public Vector<Type, 3>
{
public:
	/**
	 * @brief Construct with all three angles initialized to zero.
	 */
	Euler() = default;

	/**
	 * @brief Construct from a three-element vector.
	 *
	 * @param other Source vector ordered as phi, theta, psi.
	 */
	Euler(const Vector<Type, 3> &other) :
		Vector<Type, 3>(other)
	{
	}

	/**
	 * @brief Construct from a three-element column matrix.
	 *
	 * @param other Source 3x1 matrix ordered as phi, theta, psi.
	 */
	Euler(const Matrix<Type, 3, 1> &other) :
		Vector<Type, 3>(other)
	{
	}

	/**
	 * @brief Construct from roll, pitch, and yaw angles.
	 *
	 * Instance is initialized from an 3-2-1 intrinsic Tait-Bryan
	 * rotation sequence representing transformation from frame 1
	 * to frame 2.
	 *
	 * @param phi_ Roll about the X axis, in radians.
	 * @param theta_ Pitch about the Y axis, in radians.
	 * @param psi_ Yaw about the Z axis, in radians.
	 */
	Euler(Type phi_, Type theta_, Type psi_) : Vector<Type, 3>()
	{
		phi() = phi_;
		theta() = theta_;
		psi() = psi_;
	}

	/**
	 * @brief Extract 3-2-1 intrinsic angles from a direction cosine matrix.
	 *
	 * Instance is set from Dcm representing transformation from
	 * frame 2 to frame 1.
	 * This instance will hold the angles defining the 3-2-1 intrinsic
	 * Tait-Bryan rotation sequence from frame 1 to frame 2.
	 *
	 * @param dcm Direction cosine matrix describing the rotation.
	*/
	Euler(const Dcm<Type> &dcm)
	{
		// The (2,0) entry gives pitch for the chosen 3-2-1 convention.
		theta() = std::asin(-dcm(2, 0));

		// At pitch = +/-90 degrees, roll and yaw are not independently
		// observable. Fix roll to zero and recover the combined heading.
		if ((std::fabs(theta() - Type(M_PI / 2))) < Type(1.0e-3)) {
			phi() = 0;
			psi() = std::atan2(dcm(1, 2), dcm(0, 2));

		} else if ((std::fabs(theta() + Type(M_PI / 2))) < Type(1.0e-3)) {
			phi() = 0;
			psi() = std::atan2(-dcm(1, 2), -dcm(0, 2));

		} else {
			phi() = std::atan2(dcm(2, 1), dcm(2, 2));
			psi() = std::atan2(dcm(1, 0), dcm(0, 0));
		}
	}

	/**
	 * @brief Extract 3-2-1 intrinsic angles from a quaternion.
	 *
	 * Instance is set from a quaternion representing transformation
	 * from frame 2 to frame 1.
	 * This instance will hold the angles defining the 3-2-1 intrinsic
	 * Tait-Bryan rotation sequence from frame 1 to frame 2.
	 *
	 * @param q Quaternion describing the rotation.
	*/
	Euler(const Quaternion<Type> &q) : Vector<Type, 3>(Euler(Dcm<Type>(q)))
	{
	}

	/** @brief Return roll about X in radians. */
	inline Type phi() const
	{
		return (*this)(0);
	}
	/** @brief Return pitch about Y in radians. */
	inline Type theta() const
	{
		return (*this)(1);
	}
	/** @brief Return yaw about Z in radians. */
	inline Type psi() const
	{
		return (*this)(2);
	}

	/** @brief Return a mutable reference to roll about X, in radians. */
	inline Type &phi()
	{
		return (*this)(0);
	}
	/** @brief Return a mutable reference to pitch about Y, in radians. */
	inline Type &theta()
	{
		return (*this)(1);
	}
	/** @brief Return a mutable reference to yaw about Z, in radians. */
	inline Type &psi()
	{
		return (*this)(2);
	}

};

/** Single-precision Euler-angle rotation. */
using Eulerf = Euler<float>;
/** Double-precision Euler-angle rotation. */
using Eulerd = Euler<double>;

} // namespace matrix
