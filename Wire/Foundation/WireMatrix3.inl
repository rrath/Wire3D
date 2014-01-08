// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3(Bool zero)
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
Matrix3<Real>::Matrix3(Real m00, Real m01, Real m02, Real m10, Real m11,
	Real m12, Real m20, Real m21, Real m22)
{
	mEntry[0] = m00;
	mEntry[1] = m01;
	mEntry[2] = m02;
	mEntry[3] = m10;
	mEntry[4] = m11;
	mEntry[5] = m12;
	mEntry[6] = m20;
	mEntry[7] = m21;
	mEntry[8] = m22;
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3(const Vector3<Real>& rAxis, Real angle)
{
	FromAxisAngle(rAxis, angle);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3(const Vector3<Real>& rU, const Vector3<Real>& rV,
	const Vector3<Real>& rW, Bool isColumn)
{
	if (isColumn)
	{
		mEntry[0] = rU[0];
		mEntry[1] = rV[0];
		mEntry[2] = rW[0];
		mEntry[3] = rU[1];
		mEntry[4] = rV[1];
		mEntry[5] = rW[1];
		mEntry[6] = rU[2];
		mEntry[7] = rV[2];
		mEntry[8] = rW[2];
	}
	else
	{
		mEntry[0] = rU[0];
		mEntry[1] = rU[1];
		mEntry[2] = rU[2];
		mEntry[3] = rV[0];
		mEntry[4] = rV[1];
		mEntry[5] = rV[2];
		mEntry[6] = rW[0];
		mEntry[7] = rW[1];
		mEntry[8] = rW[2];
	}
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::MakeZero()
{
	mEntry[0] = static_cast<Real>(0.0);
	mEntry[1] = static_cast<Real>(0.0);
	mEntry[2] = static_cast<Real>(0.0);
	mEntry[3] = static_cast<Real>(0.0);
	mEntry[4] = static_cast<Real>(0.0);
	mEntry[5] = static_cast<Real>(0.0);
	mEntry[6] = static_cast<Real>(0.0);
	mEntry[7] = static_cast<Real>(0.0);
	mEntry[8] = static_cast<Real>(0.0);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::MakeIdentity()
{
	mEntry[0] = static_cast<Real>(1.0);
	mEntry[1] = static_cast<Real>(0.0);
	mEntry[2] = static_cast<Real>(0.0);
	mEntry[3] = static_cast<Real>(0.0);
	mEntry[4] = static_cast<Real>(1.0);
	mEntry[5] = static_cast<Real>(0.0);
	mEntry[6] = static_cast<Real>(0.0);
	mEntry[7] = static_cast<Real>(0.0);
	mEntry[8] = static_cast<Real>(1.0);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::FromAxisAngle(const Vector3<Real>& rAxis,
	Real angle)
{
	Vector3<Real> u = rAxis / rAxis.Length();
	Real c = Math<Real>::Cos(angle);
	Real s = Math<Real>::Sin(angle);
	Real t = (static_cast<Real>(1.0)) - c;

	Real tuxuy = t*u.X()*u.Y();
	Real tuxuz = t*u.X()*u.Z();
	Real tuyuz = t*u.Y()*u.Z();

	mEntry[0] = t*u.X()*u.X() + c;
	mEntry[1] = tuxuy - s*u.Z();
	mEntry[2] = tuxuz + s*u.Y();

	mEntry[3] = tuxuy + s*u.Z();
	mEntry[4] = t*u.Y()*u.Y() + c;
	mEntry[5] = tuyuz - s*u.X();

	mEntry[6] = tuxuz - s*u.Y();
	mEntry[7] = tuyuz + s*u.X();
	mEntry[8] = t*u.Z()*u.Z() + c;

	// alternative approach without Length/Sqrt()
// 	Real cos = Math<Real>::Cos(angle);
// 	Real sin = Math<Real>::Sin(angle);
// 	Real oneMinusCos = (static_cast<Real>(1.0))-cos;
// 	Real x2 = rAxis[0]*rAxis[0];
// 	Real y2 = rAxis[1]*rAxis[1];
// 	Real z2 = rAxis[2]*rAxis[2];
// 	Real xym = rAxis[0]*rAxis[1]*oneMinusCos;
// 	Real xzm = rAxis[0]*rAxis[2]*oneMinusCos;
// 	Real yzm = rAxis[1]*rAxis[2]*oneMinusCos;
// 	Real xSin = rAxis[0]*sin;
// 	Real ySin = rAxis[1]*sin;
// 	Real zSin = rAxis[2]*sin;
// 
// 	mEntry[0] = x2*oneMinusCos+cos;
// 	mEntry[1] = xym-zSin;
// 	mEntry[2] = xzm+ySin;
// 
// 	mEntry[3] = xym+zSin;
// 	mEntry[4] = y2*oneMinusCos+cos;
// 	mEntry[5] = yzm-xSin;
// 
// 	mEntry[6] = xzm-ySin;
// 	mEntry[7] = yzm+xSin;
// 	mEntry[8] = z2*oneMinusCos+cos;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix3<Real>::operator const Real* () const
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix3<Real>::operator Real* ()
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline const Real* Matrix3<Real>::operator[] (Int row) const
{
	return &mEntry[3 * row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real* Matrix3<Real>::operator[] (Int row)
{
	return &mEntry[3 * row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Matrix3<Real>::operator() (UInt row, UInt col) const
{
	return mEntry[col + 3 * row];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Matrix3<Real>::operator() (UInt row, UInt col)
{
	return mEntry[col + 3 * row];
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::SetRow(UInt row, const Vector3<Real>& rV)
{
	const UInt offset = 3*row;
	mEntry[offset] = rV[0];
	mEntry[offset+1] = rV[1];
	mEntry[offset+2] = rV[2];
}

//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Matrix3<Real>::GetRow(UInt row) const
{
	const UInt offset = 3*row;
	return Vector3<Real>(mEntry[offset], mEntry[offset+1], mEntry[offset+2]);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::SetColumn(UInt col, const Vector3<Real>& rVector)
{
	mEntry[col] = rVector[0];
	mEntry[col+3] = rVector[1];
	mEntry[col+6] = rVector[2];
}

//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Matrix3<Real>::GetColumn(UInt col) const
{
	return Vector3<Real>(mEntry[col], mEntry[col+3], mEntry[col+6]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix3<Real> Matrix3<Real>::operator* (const Matrix3& rMatrix) const
{
	return Matrix3<Real>(
		mEntry[0] * rMatrix.mEntry[0] +
		mEntry[1] * rMatrix.mEntry[3] +
		mEntry[2] * rMatrix.mEntry[6],

		mEntry[0] * rMatrix.mEntry[1] +
		mEntry[1] * rMatrix.mEntry[4] +
		mEntry[2] * rMatrix.mEntry[7],

		mEntry[0] * rMatrix.mEntry[2] +
		mEntry[1] * rMatrix.mEntry[5] +
		mEntry[2] * rMatrix.mEntry[8],

		mEntry[3] * rMatrix.mEntry[0] +
		mEntry[4] * rMatrix.mEntry[3] +
		mEntry[5] * rMatrix.mEntry[6],

		mEntry[3] * rMatrix.mEntry[1] +
		mEntry[4] * rMatrix.mEntry[4] +
		mEntry[5] * rMatrix.mEntry[7],

		mEntry[3] * rMatrix.mEntry[2] +
		mEntry[4] * rMatrix.mEntry[5] +
		mEntry[5] * rMatrix.mEntry[8],

		mEntry[6] * rMatrix.mEntry[0] +
		mEntry[7] * rMatrix.mEntry[3] +
		mEntry[8] * rMatrix.mEntry[6],

		mEntry[6] * rMatrix.mEntry[1] +
		mEntry[7] * rMatrix.mEntry[4] +
		mEntry[8] * rMatrix.mEntry[7],

		mEntry[6] * rMatrix.mEntry[2] +
		mEntry[7] * rMatrix.mEntry[5] +
		mEntry[8] * rMatrix.mEntry[8]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix3<Real> Matrix3<Real>::operator* (Real scalar) const
{
	return Matrix3<Real>(
		scalar * mEntry[0],
		scalar * mEntry[1],
		scalar * mEntry[2],
		scalar * mEntry[3],
		scalar * mEntry[4],
		scalar * mEntry[5],
		scalar * mEntry[6],
		scalar * mEntry[7],
		scalar * mEntry[8]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Matrix3<Real>::operator* (const Vector3<Real>& rV) const
{
	return Vector3<Real>(
		mEntry[0] * rV[0] + mEntry[1] * rV[1] + mEntry[2] * rV[2],
		mEntry[3] * rV[0] + mEntry[4] * rV[1] + mEntry[5] * rV[2],
		mEntry[6] * rV[0] + mEntry[7] * rV[1] + mEntry[8] * rV[2]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::Transpose() const
{
	return Matrix3<Real>(
		mEntry[0],
		mEntry[3],
		mEntry[6],
		mEntry[1],
		mEntry[4],
		mEntry[7],
		mEntry[2],
		mEntry[5],
		mEntry[8]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::TransposeTimes(const Matrix3& rM) const
{
	// P = A^T*B
	return Matrix3<Real>(
		mEntry[0] * rM.mEntry[0] +
		mEntry[3] * rM.mEntry[3] +
		mEntry[6] * rM.mEntry[6],

		mEntry[0] * rM.mEntry[1] +
		mEntry[3] * rM.mEntry[4] +
		mEntry[6] * rM.mEntry[7],

		mEntry[0] * rM.mEntry[2] +
		mEntry[3] * rM.mEntry[5] +
		mEntry[6] * rM.mEntry[8],

		mEntry[1] * rM.mEntry[0] +
		mEntry[4] * rM.mEntry[3] +
		mEntry[7] * rM.mEntry[6],

		mEntry[1] * rM.mEntry[1] +
		mEntry[4] * rM.mEntry[4] +
		mEntry[7] * rM.mEntry[7],

		mEntry[1] * rM.mEntry[2] +
		mEntry[4] * rM.mEntry[5] +
		mEntry[7] * rM.mEntry[8],

		mEntry[2] * rM.mEntry[0] +
		mEntry[5] * rM.mEntry[3] +
		mEntry[8] * rM.mEntry[6],

		mEntry[2] * rM.mEntry[1] +
		mEntry[5] * rM.mEntry[4] +
		mEntry[8] * rM.mEntry[7],

		mEntry[2] * rM.mEntry[2] +
		mEntry[5] * rM.mEntry[5] +
		mEntry[8] * rM.mEntry[8]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::TimesTranspose(const Matrix3& rM) const
{
	// P = A*B^T
	return Matrix3<Real>(
		mEntry[0] * rM.mEntry[0] +
		mEntry[1] * rM.mEntry[1] +
		mEntry[2] * rM.mEntry[2],

		mEntry[0] * rM.mEntry[3] +
		mEntry[1] * rM.mEntry[4] +
		mEntry[2] * rM.mEntry[5],

		mEntry[0] * rM.mEntry[6] +
		mEntry[1] * rM.mEntry[7] +
		mEntry[2] * rM.mEntry[8],

		mEntry[3] * rM.mEntry[0] +
		mEntry[4] * rM.mEntry[1] +
		mEntry[5] * rM.mEntry[2],

		mEntry[3] * rM.mEntry[3] +
		mEntry[4] * rM.mEntry[4] +
		mEntry[5] * rM.mEntry[5],

		mEntry[3] * rM.mEntry[6] +
		mEntry[4] * rM.mEntry[7] +
		mEntry[5] * rM.mEntry[8],

		mEntry[6] * rM.mEntry[0] +
		mEntry[7] * rM.mEntry[1] +
		mEntry[8] * rM.mEntry[2],

		mEntry[6] * rM.mEntry[3] +
		mEntry[7] * rM.mEntry[4] +
		mEntry[8] * rM.mEntry[5],

		mEntry[6] * rM.mEntry[6] +
		mEntry[7] * rM.mEntry[7] +
		mEntry[8] * rM.mEntry[8]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::Inverse() const
{
	// Invert a 3x3 using cofactors.  This is faster than using a generic
	// Gaussian elimination because of the loop overhead of such a method.

	Matrix3 inverse;

	inverse.mEntry[0] =	mEntry[4] * mEntry[8] - mEntry[5] * mEntry[7];
	inverse.mEntry[1] =	mEntry[2] * mEntry[7] - mEntry[1] * mEntry[8];
	inverse.mEntry[2] =	mEntry[1] * mEntry[5] - mEntry[2] * mEntry[4];
	inverse.mEntry[3] =	mEntry[5] * mEntry[6] - mEntry[3] * mEntry[8];
	inverse.mEntry[4] =	mEntry[0] * mEntry[8] - mEntry[2] * mEntry[6];
	inverse.mEntry[5] =	mEntry[2] * mEntry[3] - mEntry[0] * mEntry[5];
	inverse.mEntry[6] =	mEntry[3] * mEntry[7] - mEntry[4] * mEntry[6];
	inverse.mEntry[7] =	mEntry[1] * mEntry[6] - mEntry[0] * mEntry[7];
	inverse.mEntry[8] =	mEntry[0] * mEntry[4] - mEntry[1] * mEntry[3];

	Real det =
		mEntry[0] * inverse.mEntry[0] +
		mEntry[1] * inverse.mEntry[3] +
		mEntry[2] * inverse.mEntry[6];

	if (Math<Real>::FAbs(det) <= Math<Real>::ZERO_TOLERANCE)
	{
		return ZERO;
	}

	Real invDet = (static_cast<Real>(1.0)) / det;
	inverse.mEntry[0] *= invDet;
	inverse.mEntry[1] *= invDet;
	inverse.mEntry[2] *= invDet;
	inverse.mEntry[3] *= invDet;
	inverse.mEntry[4] *= invDet;
	inverse.mEntry[5] *= invDet;
	inverse.mEntry[6] *= invDet;
	inverse.mEntry[7] *= invDet;
	inverse.mEntry[8] *= invDet;
	return inverse;
}

//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::TimesDiagonal(const Vector3<Real>& rDiag) const
{
	return Matrix3<Real>(
		mEntry[0] * rDiag[0], mEntry[1] * rDiag[1], mEntry[2] * rDiag[2],
		mEntry[3] * rDiag[0], mEntry[4] * rDiag[1], mEntry[5] * rDiag[2],
		mEntry[6] * rDiag[0], mEntry[7] * rDiag[1], mEntry[8] * rDiag[2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix3<Real> operator* (Real scalar, const Matrix3<Real>& rM)
{
	return rM * scalar;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> operator* (const Vector3<Real>& rV,
	const Matrix3<Real>& rM)
{
	return Vector3<Real>(
		rV[0]*rM[0][0] + rV[1]*rM[1][0] + rV[2]*rM[2][0],
		rV[0]*rM[0][1] + rV[1]*rM[1][1] + rV[2]*rM[2][1],
		rV[0]*rM[0][2] + rV[1]*rM[1][2] + rV[2]*rM[2][2]);
}
