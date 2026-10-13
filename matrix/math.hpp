/**
 * @file math.hpp
 * @brief Convenience header for the public PX4 matrix types and algorithms.
 *
 * Include this header when a translation unit needs the complete matrix API.
 * Individual headers can be included instead to reduce compile-time
 * dependencies on constrained targets.
 */
#pragma once

#include "AxisAngle.hpp"
#include "Dcm.hpp"
#include "Dcm2.hpp"
#include "Dual.hpp"
#include "Euler.hpp"
#include "helper_functions.hpp"
#include "LeastSquaresSolver.hpp"
#include "Matrix.hpp"
#include "PseudoInverse.hpp"
#include "Quaternion.hpp"
#include "Scalar.hpp"
#include "Slice.hpp"
#include "SparseVector.hpp"
#include "SquareMatrix.hpp"
#include "Vector.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
