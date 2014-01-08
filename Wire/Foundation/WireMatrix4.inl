// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real>::Matrix4()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real>::Matrix4(Bool zero)
{
	if (zero)
	{
		MakeZero();
	}
	else
	{
		MakeIdentity();
	}
}

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real>::Matrix4(
	Real m00, Real m01, Real m02, Real m03,
	Real m10, Real m11, Real m12, Real m13,
	Real m20, Real m21, Real m22, Real m23,
	Real m30, Real m31, Real m32, Real m33)
{
	mEntry[ 0] = m00;
	mEntry[ 1] = m01;
	mEntry[ 2] = m02;
	mEntry[ 3] = m03;
	mEntry[ 4] = m10;
	mEntry[ 5] = m11;
	mEntry[ 6] = m12;
	mEntry[ 7] = m13;
	mEntry[ 8] = m20;
	mEntry[ 9] = m21;
	mEntry[10] = m22;
	mEntry[11] = m23;
	mEntry[12] = m30;
	mEntry[13] = m31;
	mEntry[14] = m32;
	mEntry[15] = m33;
}

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real>::Matrix4(const Vector4<Real>& rV0, const Vector4<Real>& rV1,
	const Vector4<Real>& rV2, const Vector4<Real>& rV3, Bool isColumn)
{
	if (isColumn)
	{
		mEntry[ 0] = rV0[0];
		mEntry[ 1] = rV1[0];
		mEntry[ 2] = rV2[0];
		mEntry[ 3] = rV3[0];
		mEntry[ 4] = rV0[1];
		mEntry[ 5] = rV1[1];
		mEntry[ 6] = rV2[1];
		mEntry[ 7] = rV3[1];
		mEntry[ 8] = rV0[2];
		mEntry[ 9] = rV1[2];
		mEntry[10] = rV2[2];
		mEntry[11] = rV3[2];
		mEntry[12] = rV0[3];
		mEntry[13] = rV1[3];
		mEntry[14] = rV2[3];
		mEntry[15] = rV3[3];
	}
	else
	{
		mEntry[ 0] = rV0[0];
		mEntry[ 1] = rV0[1];
		mEntry[ 2] = rV0[2];
		mEntry[ 3] = rV0[3];
		mEntry[ 4] = rV1[0];
		mEntry[ 5] = rV1[1];
		mEntry[ 6] = rV1[2];
		mEntry[ 7] = rV1[3];
		mEntry[ 8] = rV2[0];
		mEntry[ 9] = rV2[1];
		mEntry[10] = rV2[2];
		mEntry[11] = rV2[3];
		mEntry[12] = rV3[0];
		mEntry[13] = rV3[1];
		mEntry[14] = rV3[2];
		mEntry[15] = rV3[3];
	}
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix4<Real>::MakeZero()
{
	mEntry[ 0] = static_cast<Real>(0.0);
	mEntry[ 1] = static_cast<Real>(0.0);
	mEntry[ 2] = static_cast<Real>(0.0);
	mEntry[ 3] = static_cast<Real>(0.0);
	mEntry[ 4] = static_cast<Real>(0.0);
	mEntry[ 5] = static_cast<Real>(0.0);
	mEntry[ 6] = static_cast<Real>(0.0);
	mEntry[ 7] = static_cast<Real>(0.0);
	mEntry[ 8] = static_cast<Real>(0.0);
	mEntry[ 9] = static_cast<Real>(0.0);
	mEntry[10] = static_cast<Real>(0.0);
	mEntry[11] = static_cast<Real>(0.0);
	mEntry[12] = static_cast<Real>(0.0);
	mEntry[13] = static_cast<Real>(0.0);
	mEntry[14] = static_cast<Real>(0.0);
	mEntry[15] = static_cast<Real>(0.0);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix4<Real>::MakeIdentity()
{
	mEntry[ 0] = static_cast<Real>(1.0);
	mEntry[ 1] = static_cast<Real>(0.0);
	mEntry[ 2] = static_cast<Real>(0.0);
	mEntry[ 3] = static_cast<Real>(0.0);
	mEntry[ 4] = static_cast<Real>(0.0);
	mEntry[ 5] = static_cast<Real>(1.0);
	mEntry[ 6] = static_cast<Real>(0.0);
	mEntry[ 7] = static_cast<Real>(0.0);
	mEntry[ 8] = static_cast<Real>(0.0);
	mEntry[ 9] = static_cast<Real>(0.0);
	mEntry[10] = static_cast<Real>(1.0);
	mEntry[11] = static_cast<Real>(0.0);
	mEntry[12] = static_cast<Real>(0.0);
	mEntry[13] = static_cast<Real>(0.0);
	mEntry[14] = static_cast<Real>(0.0);
	mEntry[15] = static_cast<Real>(1.0);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real>::operator const Real* () const
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real>::operator Real* ()
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline const Real* Matrix4<Real>::operator[] (Int row) const
{
	return &mEntry[4 * row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real* Matrix4<Real>::operator[] (Int row)
{
	return &mEntry[4 * row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Matrix4<Real>::operator() (UInt row, UInt col) const
{
	return mEntry[col + 4*row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Matrix4<Real>::operator() (UInt row, UInt col)
{
	return mEntry[col + 4*row];
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix4<Real>::SetRow(UInt row, const Vector4<Real>& rkV)
{
	const UInt offset = 4*row;
	mEntry[offset] = rkV[0];
	mEntry[offset+1] = rkV[1];
	mEntry[offset+2] = rkV[2];
	mEntry[offset+3] = rkV[3];
}

//----------------------------------------------------------------------------
template <class Real>
Vector4<Real> Matrix4<Real>::GetRow(UInt row) const
{
	const UInt offset = 4*row;
	return Vector4<Real>(mEntry[offset], mEntry[offset+1], mEntry[offset+2],
		mEntry[offset+3]);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix4<Real>::SetColumn(UInt col, const Vector4<Real>& rV)
{
	mEntry[col] = rV[0];
	mEntry[col+4] = rV[1];
	mEntry[col+8] = rV[2];
	mEntry[col+12] = rV[3];
}

//----------------------------------------------------------------------------
template <class Real>
Vector4<Real> Matrix4<Real>::GetColumn(UInt col) const
{
	return Vector4<Real>(mEntry[col], mEntry[col+4] , mEntry[col+8],
		mEntry[col+12]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator* (const Matrix4& rM) const
{
	return Matrix4<Real>(
		mEntry[ 0] * rM.mEntry[ 0] +
		mEntry[ 1] * rM.mEntry[ 4] +
		mEntry[ 2] * rM.mEntry[ 8] +
		mEntry[ 3] * rM.mEntry[12],

		mEntry[ 0] * rM.mEntry[ 1] +
		mEntry[ 1] * rM.mEntry[ 5] +
		mEntry[ 2] * rM.mEntry[ 9] +
		mEntry[ 3] * rM.mEntry[13],

		mEntry[ 0] * rM.mEntry[ 2] +
		mEntry[ 1] * rM.mEntry[ 6] +
		mEntry[ 2] * rM.mEntry[10] +
		mEntry[ 3] * rM.mEntry[14],

		mEntry[ 0] * rM.mEntry[ 3] +
		mEntry[ 1] * rM.mEntry[ 7] +
		mEntry[ 2] * rM.mEntry[11] +
		mEntry[ 3] * rM.mEntry[15],

		mEntry[ 4] * rM.mEntry[ 0] +
		mEntry[ 5] * rM.mEntry[ 4] +
		mEntry[ 6] * rM.mEntry[ 8] +
		mEntry[ 7] * rM.mEntry[12],

		mEntry[ 4] * rM.mEntry[ 1] +
		mEntry[ 5] * rM.mEntry[ 5] +
		mEntry[ 6] * rM.mEntry[ 9] +
		mEntry[ 7] * rM.mEntry[13],

		mEntry[ 4] * rM.mEntry[ 2] +
		mEntry[ 5] * rM.mEntry[ 6] +
		mEntry[ 6] * rM.mEntry[10] +
		mEntry[ 7] * rM.mEntry[14],

		mEntry[ 4] * rM.mEntry[ 3] +
		mEntry[ 5] * rM.mEntry[ 7] +
		mEntry[ 6] * rM.mEntry[11] +
		mEntry[ 7] * rM.mEntry[15],

		mEntry[ 8] * rM.mEntry[ 0] +
		mEntry[ 9] * rM.mEntry[ 4] +
		mEntry[10] * rM.mEntry[ 8] +
		mEntry[11] * rM.mEntry[12],

		mEntry[ 8] * rM.mEntry[ 1] +
		mEntry[ 9] * rM.mEntry[ 5] +
		mEntry[10] * rM.mEntry[ 9] +
		mEntry[11] * rM.mEntry[13],

		mEntry[ 8] * rM.mEntry[ 2] +
		mEntry[ 9] * rM.mEntry[ 6] +
		mEntry[10] * rM.mEntry[10] +
		mEntry[11] * rM.mEntry[14],

		mEntry[ 8] * rM.mEntry[ 3] +
		mEntry[ 9] * rM.mEntry[ 7] +
		mEntry[10] * rM.mEntry[11] +
		mEntry[11] * rM.mEntry[15],

		mEntry[12] * rM.mEntry[ 0] +
		mEntry[13] * rM.mEntry[ 4] +
		mEntry[14] * rM.mEntry[ 8] +
		mEntry[15] * rM.mEntry[12],

		mEntry[12] * rM.mEntry[ 1] +
		mEntry[13] * rM.mEntry[ 5] +
		mEntry[14] * rM.mEntry[ 9] +
		mEntry[15] * rM.mEntry[13],

		mEntry[12] * rM.mEntry[ 2] +
		mEntry[13] * rM.mEntry[ 6] +
		mEntry[14] * rM.mEntry[10] +
		mEntry[15] * rM.mEntry[14],

		mEntry[12] * rM.mEntry[ 3] +
		mEntry[13] * rM.mEntry[ 7] +
		mEntry[14] * rM.mEntry[11] +
		mEntry[15] * rM.mEntry[15]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix4<Real>::operator* (Real scalar) const
{
	return Matrix4<Real>(
		scalar * mEntry[ 0],
		scalar * mEntry[ 1],
		scalar * mEntry[ 2],
		scalar * mEntry[ 3],
		scalar * mEntry[ 4],
		scalar * mEntry[ 5],
		scalar * mEntry[ 6],
		scalar * mEntry[ 7],
		scalar * mEntry[ 8],
		scalar * mEntry[ 9],
		scalar * mEntry[10],
		scalar * mEntry[11],
		scalar * mEntry[12],
		scalar * mEntry[13],
		scalar * mEntry[14],
		scalar * mEntry[15]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Matrix4<Real>::operator* (const Vector4<Real>& rV) const
{
	return Vector4<Real>(
		mEntry[ 0] * rV[0] +
		mEntry[ 1] * rV[1] +
		mEntry[ 2] * rV[2] +
		mEntry[ 3] * rV[3],

		mEntry[ 4] * rV[0] +
		mEntry[ 5] * rV[1] +
		mEntry[ 6] * rV[2] +
		mEntry[ 7] * rV[3],

		mEntry[ 8] * rV[0] +
		mEntry[ 9] * rV[1] +
		mEntry[10] * rV[2] +
		mEntry[11] * rV[3],

		mEntry[12] * rV[0] +
		mEntry[13] * rV[1] +
		mEntry[14] * rV[2] +
		mEntry[15] * rV[3]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::Transpose() const
{
	return Matrix4<Real>(
		mEntry[ 0],
		mEntry[ 4],
		mEntry[ 8],
		mEntry[12],
		mEntry[ 1],
		mEntry[ 5],
		mEntry[ 9],
		mEntry[13],
		mEntry[ 2],
		mEntry[ 6],
		mEntry[10],
		mEntry[14],
		mEntry[ 3],
		mEntry[ 7],
		mEntry[11],
		mEntry[15]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::TransposeTimes(const Matrix4& rM) const
{
	// P = A^T*B
	return Matrix4<Real>(
		mEntry[ 0] * rM.mEntry[ 0] +
		mEntry[ 4] * rM.mEntry[ 4] +
		mEntry[ 8] * rM.mEntry[ 8] +
		mEntry[12] * rM.mEntry[12],

		mEntry[ 0] * rM.mEntry[ 1] +
		mEntry[ 4] * rM.mEntry[ 5] +
		mEntry[ 8] * rM.mEntry[ 9] +
		mEntry[12] * rM.mEntry[13],

		mEntry[ 0] * rM.mEntry[ 2] +
		mEntry[ 4] * rM.mEntry[ 6] +
		mEntry[ 8] * rM.mEntry[10] +
		mEntry[12] * rM.mEntry[14],

		mEntry[ 0] * rM.mEntry[ 3] +
		mEntry[ 4] * rM.mEntry[ 7] +
		mEntry[ 8] * rM.mEntry[11] +
		mEntry[12] * rM.mEntry[15],

		mEntry[ 1] * rM.mEntry[ 0] +
		mEntry[ 5] * rM.mEntry[ 4] +
		mEntry[ 9] * rM.mEntry[ 8] +
		mEntry[13] * rM.mEntry[12],

		mEntry[ 1] * rM.mEntry[ 1] +
		mEntry[ 5] * rM.mEntry[ 5] +
		mEntry[ 9] * rM.mEntry[ 9] +
		mEntry[13] * rM.mEntry[13],

		mEntry[ 1] * rM.mEntry[ 2] +
		mEntry[ 5] * rM.mEntry[ 6] +
		mEntry[ 9] * rM.mEntry[10] +
		mEntry[13] * rM.mEntry[14],

		mEntry[ 1] * rM.mEntry[ 3] +
		mEntry[ 5] * rM.mEntry[ 7] +
		mEntry[ 9] * rM.mEntry[11] +
		mEntry[13] * rM.mEntry[15],

		mEntry[ 2] * rM.mEntry[ 0] +
		mEntry[ 6] * rM.mEntry[ 4] +
		mEntry[10] * rM.mEntry[ 8] +
		mEntry[14] * rM.mEntry[12],

		mEntry[ 2] * rM.mEntry[ 1] +
		mEntry[ 6] * rM.mEntry[ 5] +
		mEntry[10] * rM.mEntry[ 9] +
		mEntry[14] * rM.mEntry[13],

		mEntry[ 2] * rM.mEntry[ 2] +
		mEntry[ 6] * rM.mEntry[ 6] +
		mEntry[10] * rM.mEntry[10] +
		mEntry[14] * rM.mEntry[14],

		mEntry[ 2] * rM.mEntry[ 3] +
		mEntry[ 6] * rM.mEntry[ 7] +
		mEntry[10] * rM.mEntry[11] +
		mEntry[14] * rM.mEntry[15],

		mEntry[ 3] * rM.mEntry[ 0] +
		mEntry[ 7] * rM.mEntry[ 4] +
		mEntry[11] * rM.mEntry[ 8] +
		mEntry[15] * rM.mEntry[12],

		mEntry[ 3] * rM.mEntry[ 1] +
		mEntry[ 7] * rM.mEntry[ 5] +
		mEntry[11] * rM.mEntry[ 9] +
		mEntry[15] * rM.mEntry[13],

		mEntry[ 3] * rM.mEntry[ 2] +
		mEntry[ 7] * rM.mEntry[ 6] +
		mEntry[11] * rM.mEntry[10] +
		mEntry[15] * rM.mEntry[14],

		mEntry[ 3] * rM.mEntry[ 3] +
		mEntry[ 7] * rM.mEntry[ 7] +
		mEntry[11] * rM.mEntry[11] +
		mEntry[15] * rM.mEntry[15]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::TimesTranspose(const Matrix4& rM) const
{
	// P = A*B^T
	return Matrix4<Real>(
		mEntry[ 0] * rM.mEntry[ 0] +
		mEntry[ 1] * rM.mEntry[ 1] +
		mEntry[ 2] * rM.mEntry[ 2] +
		mEntry[ 3] * rM.mEntry[ 3],

		mEntry[ 0] * rM.mEntry[ 4] +
		mEntry[ 1] * rM.mEntry[ 5] +
		mEntry[ 2] * rM.mEntry[ 6] +
		mEntry[ 3] * rM.mEntry[ 7],

		mEntry[ 0] * rM.mEntry[ 8] +
		mEntry[ 1] * rM.mEntry[ 9] +
		mEntry[ 2] * rM.mEntry[10] +
		mEntry[ 3] * rM.mEntry[11],

		mEntry[ 0] * rM.mEntry[12] +
		mEntry[ 1] * rM.mEntry[13] +
		mEntry[ 2] * rM.mEntry[14] +
		mEntry[ 3] * rM.mEntry[15],

		mEntry[ 4] * rM.mEntry[ 0] +
		mEntry[ 5] * rM.mEntry[ 1] +
		mEntry[ 6] * rM.mEntry[ 2] +
		mEntry[ 7] * rM.mEntry[ 3],

		mEntry[ 4] * rM.mEntry[ 4] +
		mEntry[ 5] * rM.mEntry[ 5] +
		mEntry[ 6] * rM.mEntry[ 6] +
		mEntry[ 7] * rM.mEntry[ 7],

		mEntry[ 4] * rM.mEntry[ 8] +
		mEntry[ 5] * rM.mEntry[ 9] +
		mEntry[ 6] * rM.mEntry[10] +
		mEntry[ 7] * rM.mEntry[11],

		mEntry[ 4] * rM.mEntry[12] +
		mEntry[ 5] * rM.mEntry[13] +
		mEntry[ 6] * rM.mEntry[14] +
		mEntry[ 7] * rM.mEntry[15],

		mEntry[ 8] * rM.mEntry[ 0] +
		mEntry[ 9] * rM.mEntry[ 1] +
		mEntry[10] * rM.mEntry[ 2] +
		mEntry[11] * rM.mEntry[ 3],

		mEntry[ 8] * rM.mEntry[ 4] +
		mEntry[ 9] * rM.mEntry[ 5] +
		mEntry[10] * rM.mEntry[ 6] +
		mEntry[11] * rM.mEntry[ 7],

		mEntry[ 8] * rM.mEntry[ 8] +
		mEntry[ 9] * rM.mEntry[ 9] +
		mEntry[10] * rM.mEntry[10] +
		mEntry[11] * rM.mEntry[11],

		mEntry[ 8] * rM.mEntry[12] +
		mEntry[ 9] * rM.mEntry[13] +
		mEntry[10] * rM.mEntry[14] +
		mEntry[11] * rM.mEntry[15],

		mEntry[12] * rM.mEntry[ 0] +
		mEntry[13] * rM.mEntry[ 1] +
		mEntry[14] * rM.mEntry[ 2] +
		mEntry[15] * rM.mEntry[ 3],

		mEntry[12] * rM.mEntry[ 4] +
		mEntry[13] * rM.mEntry[ 5] +
		mEntry[14] * rM.mEntry[ 6] +
		mEntry[15] * rM.mEntry[ 7],

		mEntry[12] * rM.mEntry[ 8] +
		mEntry[13] * rM.mEntry[ 9] +
		mEntry[14] * rM.mEntry[10] +
		mEntry[15] * rM.mEntry[11],

		mEntry[12] * rM.mEntry[12] +
		mEntry[13] * rM.mEntry[13] +
		mEntry[14] * rM.mEntry[14] +
		mEntry[15] * rM.mEntry[15]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix4<Real> Matrix4<Real>::Inverse() const
{
	Real a0 = mEntry[ 0] * mEntry[ 5] - mEntry[ 1] * mEntry[ 4];
	Real a1 = mEntry[ 0] * mEntry[ 6] - mEntry[ 2] * mEntry[ 4];
	Real a2 = mEntry[ 0] * mEntry[ 7] - mEntry[ 3] * mEntry[ 4];
	Real a3 = mEntry[ 1] * mEntry[ 6] - mEntry[ 2] * mEntry[ 5];
	Real a4 = mEntry[ 1] * mEntry[ 7] - mEntry[ 3] * mEntry[ 5];
	Real a5 = mEntry[ 2] * mEntry[ 7] - mEntry[ 3] * mEntry[ 6];
	Real b0 = mEntry[ 8] * mEntry[13] - mEntry[ 9] * mEntry[12];
	Real b1 = mEntry[ 8] * mEntry[14] - mEntry[10] * mEntry[12];
	Real b2 = mEntry[ 8] * mEntry[15] - mEntry[11] * mEntry[12];
	Real b3 = mEntry[ 9] * mEntry[14] - mEntry[10] * mEntry[13];
	Real b4 = mEntry[ 9] * mEntry[15] - mEntry[11] * mEntry[13];
	Real b5 = mEntry[10] * mEntry[15] - mEntry[11] * mEntry[14];

	Real det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
	if (Math<Real>::FAbs(det) <= Math<Real>::ZERO_TOLERANCE)
	{
		return Matrix4<Real>::ZERO;
	}

	Matrix4 inv;
	inv.mEntry[ 0] = + mEntry[ 5] * b5 - mEntry[ 6] * b4 + mEntry[ 7] * b3;
	inv.mEntry[ 4] = - mEntry[ 4] * b5 + mEntry[ 6] * b2 - mEntry[ 7] * b1;
	inv.mEntry[ 8] = + mEntry[ 4] * b4 - mEntry[ 5] * b2 + mEntry[ 7] * b0;
	inv.mEntry[12] = - mEntry[ 4] * b3 + mEntry[ 5] * b1 - mEntry[ 6] * b0;
	inv.mEntry[ 1] = - mEntry[ 1] * b5 + mEntry[ 2] * b4 - mEntry[ 3] * b3;
	inv.mEntry[ 5] = + mEntry[ 0] * b5 - mEntry[ 2] * b2 + mEntry[ 3] * b1;
	inv.mEntry[ 9] = - mEntry[ 0] * b4 + mEntry[ 1] * b2 - mEntry[ 3] * b0;
	inv.mEntry[13] = + mEntry[ 0] * b3 - mEntry[ 1] * b1 + mEntry[ 2] * b0;
	inv.mEntry[ 2] = + mEntry[13] * a5 - mEntry[14] * a4 + mEntry[15] * a3;
	inv.mEntry[ 6] = - mEntry[12] * a5 + mEntry[14] * a2 - mEntry[15] * a1;
	inv.mEntry[10] = + mEntry[12] * a4 - mEntry[13] * a2 + mEntry[15] * a0;
	inv.mEntry[14] = - mEntry[12] * a3 + mEntry[13] * a1 - mEntry[14] * a0;
	inv.mEntry[ 3] = - mEntry[ 9] * a5 + mEntry[10] * a4 - mEntry[11] * a3;
	inv.mEntry[ 7] = + mEntry[ 8] * a5 - mEntry[10] * a2 + mEntry[11] * a1;
	inv.mEntry[11] = - mEntry[ 8] * a4 + mEntry[ 9] * a2 - mEntry[11] * a0;
	inv.mEntry[15] = + mEntry[ 8] * a3 - mEntry[ 9] * a1 + mEntry[10] * a0;

	Real invDet = static_cast<Real>(1.0) / det;
	inv.mEntry[ 0] *= invDet;
	inv.mEntry[ 1] *= invDet;
	inv.mEntry[ 2] *= invDet;
	inv.mEntry[ 3] *= invDet;
	inv.mEntry[ 4] *= invDet;
	inv.mEntry[ 5] *= invDet;
	inv.mEntry[ 6] *= invDet;
	inv.mEntry[ 7] *= invDet;
	inv.mEntry[ 8] *= invDet;
	inv.mEntry[ 9] *= invDet;
	inv.mEntry[10] *= invDet;
	inv.mEntry[11] *= invDet;
	inv.mEntry[12] *= invDet;
	inv.mEntry[13] *= invDet;
	inv.mEntry[14] *= invDet;
	inv.mEntry[15] *= invDet;

	return inv;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> operator* (Real scalar, const Matrix4<Real>& rM)
{
	return rM * scalar;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> operator* (const Vector4<Real>& rV,
	const Matrix4<Real>& rM)
{
	return Vector4<Real>(
		rV[0]*rM[0][0] + rV[1]*rM[1][0] + rV[2]*rM[2][0] + rV[3]*rM[3][0],
		rV[0]*rM[0][1] + rV[1]*rM[1][1] + rV[2]*rM[2][1] + rV[3]*rM[3][1],
		rV[0]*rM[0][2] + rV[1]*rM[1][2] + rV[2]*rM[2][2] + rV[3]*rM[3][2],
		rV[0]*rM[0][3] + rV[1]*rM[1][3] + rV[2]*rM[2][3] + rV[3]*rM[3][3]);
}
