// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2(Bool zero)
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
Matrix2<Real>::Matrix2(Real m00, Real m01, Real m10, Real m11)
{
	mEntry[0] = m00;
	mEntry[1] = m01;
	mEntry[2] = m10;
	mEntry[3] = m11;
}

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2(const Vector2<Real>& rU, const Vector2<Real>& rV,
	Bool isColumn)
{
	if (isColumn)
	{
		mEntry[0] = rU[0];
		mEntry[1] = rV[0];
		mEntry[2] = rU[1];
		mEntry[3] = rV[1];
	}
	else
	{
		mEntry[0] = rU[0];
		mEntry[1] = rU[1];
		mEntry[2] = rV[0];
		mEntry[3] = rV[1];
	}
}

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2(Real angle)
{
	FromAngle(angle);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::MakeZero()
{
	mEntry[0] = static_cast<Real>(0.0);
	mEntry[1] = static_cast<Real>(0.0);
	mEntry[2] = static_cast<Real>(0.0);
	mEntry[3] = static_cast<Real>(0.0);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::MakeIdentity()
{
	mEntry[0] = static_cast<Real>(1.0);
	mEntry[1] = static_cast<Real>(0.0);
	mEntry[2] = static_cast<Real>(0.0);
	mEntry[3] = static_cast<Real>(1.0);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::FromAngle(Real angle)
{
	mEntry[0] = Math<Real>::Cos(angle);
	mEntry[2] = Math<Real>::Sin(angle);
	mEntry[1] = -mEntry[2];
	mEntry[3] =  mEntry[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix2<Real>::operator const Real* () const
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix2<Real>::operator Real* ()
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline const Real* Matrix2<Real>::operator[] (Int row) const
{
	return &mEntry[2 * row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real* Matrix2<Real>::operator[] (Int row)
{
	return &mEntry[2 * row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Matrix2<Real>::operator() (UInt row, UInt col) const
{
	return mEntry[col + 2*row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Matrix2<Real>::operator() (UInt row, UInt col)
{
	return mEntry[col + 2*row];
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::SetRow(UInt row, const Vector2<Real>& rkV)
{
	mEntry[2*row] = rkV[0];
	mEntry[2*row+1] = rkV[1];
}

//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Matrix2<Real>::GetRow(UInt row) const
{
	return Vector2<Real>(mEntry[2*row], mEntry[2*row+1]);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::SetColumn(UInt iCol, const Vector2<Real>& rV)
{
	mEntry[iCol] = rV[0];
	mEntry[iCol+2] = rV[1];
}

//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Matrix2<Real>::GetColumn(UInt col) const
{
	return Vector2<Real>(mEntry[col], mEntry[col+2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix2<Real> Matrix2<Real>::operator* (const Matrix2& rM) const
{
	return Matrix2<Real>(
		mEntry[0] * rM.mEntry[0] + mEntry[1] * rM.mEntry[2],
		mEntry[0] * rM.mEntry[1] + mEntry[1] * rM.mEntry[3],
		mEntry[2] * rM.mEntry[0] + mEntry[3] * rM.mEntry[2],
		mEntry[2] * rM.mEntry[1] + mEntry[3] * rM.mEntry[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix2<Real> Matrix2<Real>::operator* (Real scalar) const
{
	return Matrix2<Real>(
		scalar * mEntry[0],
		scalar * mEntry[1],
		scalar * mEntry[2],
		scalar * mEntry[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> Matrix2<Real>::operator* (const Vector2<Real>& rV) const
{
	return Vector2<Real>(
		mEntry[0] * rV[0] + mEntry[1] * rV[1],
		mEntry[2] * rV[0] + mEntry[3] * rV[1]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::Transpose() const
{
	return Matrix2<Real>(
		mEntry[0],
		mEntry[2],
		mEntry[1],
		mEntry[3]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::TransposeTimes(const Matrix2& rM) const
{
	// P = A^T*B
	return Matrix2<Real>(
		mEntry[0] * rM.mEntry[0] + mEntry[2] * rM.mEntry[2],
		mEntry[0] * rM.mEntry[1] + mEntry[2] * rM.mEntry[3],
		mEntry[1] * rM.mEntry[0] + mEntry[3] * rM.mEntry[2],
		mEntry[1] * rM.mEntry[1] + mEntry[3] * rM.mEntry[3]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::TimesTranspose(const Matrix2& rM) const
{
	// P = A*B^T
	return Matrix2<Real>(
		mEntry[0] * rM.mEntry[0] + mEntry[1] * rM.mEntry[1],
		mEntry[0] * rM.mEntry[2] + mEntry[1] * rM.mEntry[3],
		mEntry[2] * rM.mEntry[0] + mEntry[3] * rM.mEntry[1],
		mEntry[2] * rM.mEntry[2] + mEntry[3] * rM.mEntry[3]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::Inverse() const
{
	Matrix2<Real> inverse;

	Real det = mEntry[0] * mEntry[3] - mEntry[1] * mEntry[2];
	if (Math<Real>::FAbs(det) > Math<Real>::ZERO_TOLERANCE)
	{
		Real invDet = static_cast<Real>(1.0) / det;
		inverse.mEntry[0] =  mEntry[3] * invDet;
		inverse.mEntry[1] = -mEntry[1] * invDet;
		inverse.mEntry[2] = -mEntry[2] * invDet;
		inverse.mEntry[3] =  mEntry[0] * invDet;
	}
	else
	{
		inverse.mEntry[0] = static_cast<Real>(0.0);
		inverse.mEntry[1] = static_cast<Real>(0.0);
		inverse.mEntry[2] = static_cast<Real>(0.0);
		inverse.mEntry[3] = static_cast<Real>(0.0);
	}

	return inverse;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix2<Real> operator* (Real scalar, const Matrix2<Real>& rM)
{
	return rM * scalar;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> operator* (const Vector2<Real>& rV,
	const Matrix2<Real>& rM)
{
	return Vector2<Real>(
		rV[0] * rM[0][0] + rV[1] * rM[1][0],
		rV[0] * rM[0][1] + rV[1] * rM[1][1]);
}
