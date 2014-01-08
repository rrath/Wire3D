// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREMATRIX3_H
#define WIREMATRIX3_H

// Matrix operations are applied on the left. For example, given a matrix M
// and a vector V, matrix-times-vector is M*V. That is, V is treated as a
// column vector. Some graphics APIs use V*M where V is treated as a row
// vector. In this context the "M" matrix is really a transpose of the M as
// represented in Wire. Similarly, to apply two matrix operations M0 and M1,
// in that order, you compute M1*M0 so that the transform of a vector is
// (M1*M0)*V = M1*(M0*V). Some graphics APIs use M0*M1, but again these
// matrices are the transpose of those as represented in Wire. You must
// therefore be careful about how you interface the transformation code with
// graphics APIS.

// The (x,y,z) coordinate system is assumed to be right-handed. Coordinate
// axis rotation matrices are of the form
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// where t > 0 indicates a counterclockwise rotation in the yz-plane
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// where t > 0 indicates a counterclockwise rotation in the zx-plane
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// where t > 0 indicates a counterclockwise rotation in the xy-plane.

#include "WireTypes.h"
#include "WireVector3.h"

namespace Wire
{

template <class Real>
class Matrix3
{
public:
	Matrix3();	// uninitialized

	// If zero is true, create the zero matrix. Otherwise, create the
	// identity matrix.
	Matrix3(Bool zero);

	// input mRC is in row R, column C.
	Matrix3(Real m00, Real m01, Real m02,
		Real m10, Real m11, Real m12,
		Real m20, Real m21, Real m22);

	// Create matrices based on row or column vector input
	Matrix3(const Vector3<Real>& rU, const Vector3<Real>& rV,
		const Vector3<Real>& rW, Bool isColumn);

	// Create rotation matrix (positive angle - counterclockwise).  The
	// angle must be in radians, not degrees.
	Matrix3(const Vector3<Real>& rAxis, Real angle);

	// create various matrices
	void MakeZero();
	void MakeIdentity();
    void FromAxisAngle(const Vector3<Real>& rAxis, Real angle);

	// member access
	inline operator const Real* () const;
	inline operator Real* ();
	inline const Real* operator[] (Int row) const;
	inline Real* operator[] (Int row);
	inline Real operator() (UInt row, UInt col) const;
	inline Real& operator() (UInt row, UInt col);
	void SetRow(UInt row, const Vector3<Real>& rV);
	Vector3<Real> GetRow(UInt row) const;
	void SetColumn(UInt col, const Vector3<Real>& rV);
	Vector3<Real> GetColumn(UInt col) const;

	// arithmetic operations
	inline Matrix3 operator* (const Matrix3& rMatrix) const;
	inline Matrix3 operator* (Real scalar) const;

	// matrix times vector
	inline Vector3<Real> operator* (const Vector3<Real>& rV) const;	// M * v

	// other operations
	Matrix3 Transpose() const;  // M^T
	Matrix3 TransposeTimes(const Matrix3& rM) const;  // this^T * M
	Matrix3 TimesTranspose(const Matrix3& rM) const;  // this * M^T
	Matrix3 Inverse() const;
	Matrix3 TimesDiagonal(const Vector3<Real>& rDiag) const;

	// special matrices
	static const Matrix3 ZERO;
	static const Matrix3 IDENTITY;

private:
    Real mEntry[9];
};

// c * M
template <class Real>
inline Matrix3<Real> operator* (Real scalar, const Matrix3<Real>& rM);

// v^T * M
template <class Real>
inline Vector3<Real> operator* (const Vector3<Real>& rV,
	const Matrix3<Real>& rM);

#include "WireMatrix3.inl"

typedef Matrix3<Float> Matrix3F;
typedef Matrix3<Double> Matrix3D;

}

#endif
