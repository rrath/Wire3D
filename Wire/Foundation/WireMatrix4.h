// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREMATRIX4_H
#define WIREMATRIX4_H

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

#include "WireTypes.h"
#include "WireVector4.h"

namespace Wire
{

template <class Real>
class Matrix4
{
public:
	Matrix4();	// uninitialized

	// If zero is true, create the zero matrix.  Otherwise, create the
	// identity matrix.
	Matrix4(Bool zero);

	// input mRC is in row R, column C
	Matrix4(Real m00, Real m01, Real m02, Real m03,
		Real m10, Real m11, Real m12, Real m13,
		Real m20, Real m21, Real m22, Real m23,
		Real m30, Real m31, Real m32, Real m33);

	// Create matrices based on row or column vector input
	Matrix4(const Vector4<Real>& rV0, const Vector4<Real>& rV1,
		const Vector4<Real>& rV2, const Vector4<Real>& rV3, Bool isColumn);

	void MakeZero();
	void MakeIdentity();

	// access operators
	inline operator const Real* () const;
	inline operator Real* ();
	inline const Real* operator[] (Int row) const;
	inline Real* operator[] (Int row);
	inline Real operator() (UInt row, UInt col) const;
	inline Real& operator() (UInt row, UInt col);
	void SetRow(UInt row, const Vector4<Real>& rV);
	Vector4<Real> GetRow(UInt row) const;
	void SetColumn(UInt col, const Vector4<Real>& rV);
	Vector4<Real> GetColumn(UInt col) const;

	// arithmetic operations
	inline Matrix4 operator* (const Matrix4& rMatrix) const;
	inline Matrix4 operator* (Real scalar) const;

	// matrix times vector
	inline Vector4<Real> operator* (const Vector4<Real>& rV) const;	// M * v

	// other operations
	Matrix4 Transpose() const;  // M^T
	Matrix4 TransposeTimes(const Matrix4& rM) const;  // this^T * M
	Matrix4 TimesTranspose(const Matrix4& rM) const;  // this * M^T
	Matrix4 Inverse() const;

	// special matrices
	static const Matrix4 ZERO;
	static const Matrix4 IDENTITY;

private:
	Real mEntry[16];
};

// c * M
template <class Real>
inline Matrix4<Real> operator* (Real scalar, const Matrix4<Real>& rM);

// v^T * M
template <class Real>
inline Vector4<Real> operator* (const Vector4<Real>& rV,
	const Matrix4<Real>& rM);

#include "WireMatrix4.inl"

typedef Matrix4<Float> Matrix4F;
typedef Matrix4<Double> Matrix4D;

}

#endif
