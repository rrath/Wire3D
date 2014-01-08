// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREMATRIX34_H
#define WIREMATRIX34_H

// Matrix34 is a 4x4 matrix with an implicit 4th row of (0, 0, 0, 1).
// It is a row-major matrix, thus it is indexed as Matrix[row][column].
// Vector3 is treated as (x, y, z, 1).

// Matrix operations are applied on the left. For example, given a matrix M
// and a vector V, matrix-times-vector is M*V. That is, V is treated as a
// column vector. Some graphics APIs use V*M where V is treated as a row
// vector. In this context the "M" matrix is really a transpose of the M as
// represented in Wire. Similarly, to apply two matrix operations M0 and M1,
// in that order, you compute M1*M0 so that the transform of a vector is
// (M1*M0)*V = M1*(M0*V). Some graphics APIs use M0*M1, but again these
// matrices are the transpose of those as represented in Wire. You must
// therefore be careful about how you interface the transformation code with
// graphics APIs.

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
#include "WireMatrix3.h"
#include "WireMatrix4.h"
#include "WireVector3.h"
#include "WireVector4.h"

namespace Wire
{

template <class Real>
class Matrix34
{
public:
	typedef Real Real4[4];

	Matrix34();	// uninitialized
	
	// If zero is true, create the zero matrix. Otherwise, create the
	// identity matrix.
	Matrix34(Bool zero);

	// input mRC is in row R, column C
	Matrix34(
		Real m00, Real m01, Real m02, Real m03,
		Real m10, Real m11, Real m12, Real m13,
		Real m20, Real m21, Real m22, Real m23);

	// Create rotation matrix (positive angle - counterclockwise). The
	// angle must be in radians, not degrees.
	Matrix34(const Vector3<Real>& rAxis, Real angle,
		const Vector3<Real>& rTranslate = Vector3<Real>::ZERO);

	// Create matrix based on row or column vector input.
	// 4th column is initialized to (0,0,0).
	Matrix34(const Vector3<Real>& rU, const Vector3<Real>& rV,
		const Vector3<Real>& rW, Bool isColumn);

	// Create matrix based on row vector input.
	Matrix34(const Vector4<Real>& rU, const Vector4<Real>& rV,
		const Vector4<Real>& rW);

	// Create matrix based on column vector input.
	Matrix34(const Vector3<Real>& rU, const Vector3<Real>& rV,
		const Vector3<Real>& rW, const Vector3<Real>& rT);

	// Create the matrix using a Matrix3 and a Vector3 as the 4th column.
	Matrix34(const Matrix3<Real>& rM, const Vector3<Real>& rV);

	// Set the matrix leaving the 4th column untouched.
	void SetMatrix3(const Matrix3<Real>& rM);

	// create various matrices
	void MakeZero();
	void MakeIdentity();

	void FromAxisAngle(const Vector3<Real>& rAxis, Real angle,
		const Vector3<Real>& rTranslate = Vector3<Real>::ZERO);

	// member access
	inline operator Real4* ();
	inline operator const Real* () const;
 	inline const Real* operator[] (Int row) const;
 	inline Real* operator[] (Int row);
	inline Real operator() (UInt row, UInt col) const;
    inline Real& operator() (UInt row, UInt col);
	void SetRow(UInt row, const Vector3<Real>& rV);
	Vector3<Real> GetRow(UInt row) const;
	void SetColumn(UInt col, const Vector3<Real>& rV);
	Vector3<Real> GetColumn(UInt col) const;

	// arithmetic operations
	inline Matrix34<Real> operator* (const Matrix34<Real>& rMatrix) const;
	inline Matrix4<Real> operator* (const Matrix4<Real>& rMatrix) const; // M34 * M4;

	// matrix times vector
	inline Vector3<Real> operator* (const Vector3<Real>& rV) const;	// M * v
	inline Vector4<Real> operator* (const Vector4<Real>& rV) const;	// M * v

	// 3x3 submatrix M * v
	inline Vector3<Real> Times3(const Vector3<Real>& rV) const;

	// 3x3 submatrix v^T * M
	inline Vector3<Real> Times3Row(const Vector3<Real>& rV) const;

	// other operations
	Matrix34 TimesDiagonal(const Vector3<Real>& rDiag) const;
    Matrix34 Inverse() const;

    // special matrices
    static const Matrix34 ZERO;
    static const Matrix34 IDENTITY;

private:
	Real mEntry[3][4];
};

// M4 * M34
template <class Real>
inline Matrix4<Real> operator* (const Matrix4<Real>& rM4,
	const Matrix34<Real>& rM34);

#ifdef WIRE_WII
#include "Wii/WireMatrix34Wii.inl"
#else
#include "WireMatrix34.inl"
#endif

typedef Matrix34<Float> Matrix34F;

}

#endif
