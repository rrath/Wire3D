// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real>::Matrix34()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real>::Matrix34(Bool zero)
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
Matrix34<Real>::Matrix34(
	Real m00, Real m01, Real m02, Real m03,
	Real m10, Real m11, Real m12, Real m13,
	Real m20, Real m21, Real m22, Real m23)
{
	mEntry[0][0] = m00;
	mEntry[0][1] = m01;
	mEntry[0][2] = m02;
	mEntry[0][3] = m03;
	mEntry[1][0] = m10;
	mEntry[1][1] = m11;
	mEntry[1][2] = m12;
	mEntry[1][3] = m13;
	mEntry[2][0] = m20;
	mEntry[2][1] = m21;
	mEntry[2][2] = m22;
	mEntry[2][3] = m23;
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real>::Matrix34(const Vector3<Real>& rAxis, Real angle,
	const Vector3<Real>& rTranslate)
{
	FromAxisAngle(rAxis, angle, rTranslate);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real>::Matrix34(const Vector3<Real>& rU, const Vector3<Real>& rV,
	const Vector3<Real>& rW, Bool isColumn)
{
	mEntry[0][3] = static_cast<Real>(0.0);
	mEntry[1][3] = static_cast<Real>(0.0);
	mEntry[2][3] = static_cast<Real>(0.0);

	if (isColumn)
	{
		mEntry[0][0] = rU[0];
		mEntry[0][1] = rV[0];
		mEntry[0][2] = rW[0];
		mEntry[1][0] = rU[1];
		mEntry[1][1] = rV[1];
		mEntry[1][2] = rW[1];
		mEntry[2][0] = rU[2];
		mEntry[2][1] = rV[2];
		mEntry[2][2] = rW[2];
	}
	else
	{
		mEntry[0][0] = rU[0];
		mEntry[0][1] = rU[1];
		mEntry[0][2] = rU[2];
		mEntry[1][0] = rV[0];
		mEntry[1][1] = rV[1];
		mEntry[1][2] = rV[2];
		mEntry[2][0] = rW[0];
		mEntry[2][1] = rW[1];
		mEntry[2][2] = rW[2];
	}
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real>::Matrix34(const Vector4<Real>& rU, const Vector4<Real>& rV,
	const Vector4<Real>& rW)
{
	mEntry[0][0] = rU[0];
	mEntry[0][1] = rU[1];
	mEntry[0][2] = rU[2];
	mEntry[0][3] = rU[3];
	mEntry[1][0] = rV[0];
	mEntry[1][1] = rV[1];
	mEntry[1][2] = rV[2];
	mEntry[1][3] = rV[3];
	mEntry[2][0] = rW[0];
	mEntry[2][1] = rW[1];
	mEntry[2][2] = rW[2];
	mEntry[2][3] = rW[3];
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real>::Matrix34(const Vector3<Real>& rU, const Vector3<Real>& rV,
	const Vector3<Real>& rW, const Vector3<Real>& rT)
{
	mEntry[0][0] = rU[0];
	mEntry[0][1] = rV[0];
	mEntry[0][2] = rW[0];
	mEntry[0][3] = rT[0];
	mEntry[1][0] = rU[1];
	mEntry[1][1] = rV[1];
	mEntry[1][2] = rW[1];
	mEntry[1][3] = rT[1];
	mEntry[2][0] = rU[2];
	mEntry[2][1] = rV[2];
	mEntry[2][2] = rW[2];
	mEntry[2][3] = rT[2];
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real>::Matrix34(const Matrix3<Real>& rM, const Vector3<Real>& rV)
{
	mEntry[0][0] = rM[0][0];
	mEntry[0][1] = rM[0][1];
	mEntry[0][2] = rM[0][2];
	mEntry[0][3] = rV[0];
	mEntry[1][0] = rM[1][0];
	mEntry[1][1] = rM[1][1];
	mEntry[1][2] = rM[1][2];
	mEntry[1][3] = rV[1];
	mEntry[2][0] = rM[2][0];
	mEntry[2][1] = rM[2][1];
	mEntry[2][2] = rM[2][2];
	mEntry[2][3] = rV[2];
}

//----------------------------------------------------------------------------			 
template <class Real>
void Matrix34<Real>::SetMatrix3(const Matrix3<Real>& rM)
{
	mEntry[0][0] = rM[0][0];
	mEntry[0][1] = rM[0][1];
	mEntry[0][2] = rM[0][2];
	mEntry[1][0] = rM[1][0];
	mEntry[1][1] = rM[1][1];
	mEntry[1][2] = rM[1][2];
	mEntry[2][0] = rM[2][0];
	mEntry[2][1] = rM[2][1];
	mEntry[2][2] = rM[2][2];
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix34<Real>::MakeZero()
{
	mEntry[0][0] = static_cast<Real>(0.0);
	mEntry[0][1] = static_cast<Real>(0.0);
	mEntry[0][2] = static_cast<Real>(0.0);
	mEntry[0][3] = static_cast<Real>(0.0);

	mEntry[1][0] = static_cast<Real>(0.0);
	mEntry[1][1] = static_cast<Real>(0.0);
	mEntry[1][2] = static_cast<Real>(0.0);
	mEntry[1][3] = static_cast<Real>(0.0);

	mEntry[2][0] = static_cast<Real>(0.0);
	mEntry[2][1] = static_cast<Real>(0.0);
	mEntry[2][2] = static_cast<Real>(0.0);
	mEntry[2][3] = static_cast<Real>(0.0);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix34<Real>::MakeIdentity()
{
	MTXIdentity(mEntry);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix34<Real>::FromAxisAngle(const Vector3<Real>& rAxis,
	Real angle, const Vector3<Real>& rTranslate)
{
	Vector3<Real>* pAxis = const_cast<Vector3<Real>*>(&rAxis);
	MTXRotAxisRad(mEntry, reinterpret_cast<Vec*>(pAxis), angle);

	mEntry[0][3] = rTranslate.X();
	mEntry[1][3] = rTranslate.Y();
	mEntry[2][3] = rTranslate.Z();

// alternative approach without Length/Sqrt()
// 	Real cos = Math<Real>::Cos(angle);
// 	Real sin = Math<Real>::Sin(angle);
// 	Real oneMinusCos = (static_cast<Real>(1.0)) - cos;
// 	Real x2 = rAxis[0] * rAxis[0];
// 	Real y2 = rAxis[1] * rAxis[1];
// 	Real z2 = rAxis[2] * rAxis[2];
// 	Real xyM = rAxis[0] * rAxis[1] * oneMinusCos;
// 	Real xzM = rAxis[0] * rAxis[2] * oneMinusCos;
// 	Real yzM = rAxis[1] * rAxis[2] * oneMinusCos;
// 	Real xSin = rAxis[0] * sin;
// 	Real ySin = rAxis[1] * sin;
// 	Real zSin = rAxis[2] * sin;
// 
// 	mEntry[0][0] = x2 * oneMinusCos + cos;
// 	mEntry[0][1] = xyM - zSin;
// 	mEntry[0][2] = xzM + ySin;
//	mEntry[0][3] = rTranslate.X();
// 
// 	mEntry[1][0] = xyM + zSin;
// 	mEntry[1][1] = y2 * oneMinusCos + cos;
// 	mEntry[1][2] = yzM - xSin;
// 	mEntry[1][3] = rTranslate.Y();
// 
// 	mEntry[2][0] = xzM - ySin;
// 	mEntry[2][1] = yzM + xSin;
// 	mEntry[2][2] = z2 * oneMinusCos + cos;
// 	mEntry[2][3] = rTranslate.Z();
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix34<Real>::operator Real4* ()
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix34<Real>::operator const Real* () const
{
	return mEntry;
}

//----------------------------------------------------------------------------
template <class Real>
inline const Real* Matrix34<Real>::operator[] (Int row) const
{
	return &mEntry[row][0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real* Matrix34<Real>::operator[] (Int row)
{
	return &mEntry[row][0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Matrix34<Real>::operator() (UInt row, UInt col) const
{
	return mEntry[row][col];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Matrix34<Real>::operator() (UInt row, UInt col)
{
	return mEntry[row][col];
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix34<Real>::SetRow(UInt row, const Vector3<Real>& rV)
{
	mEntry[row][0] = rV[0];
	mEntry[row][1] = rV[1];
	mEntry[row][2] = rV[2];
}

//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Matrix34<Real>::GetRow(UInt row) const
{
	return Vector3<Real>(mEntry[row][0], mEntry[row][1], mEntry[row][1]);
}

//----------------------------------------------------------------------------
template <class Real>
void Matrix34<Real>::SetColumn(UInt col, const Vector3<Real>& rV)
{
	mEntry[0][col] = rV.X();
	mEntry[1][col] = rV.Y();
	mEntry[2][col] = rV.Z();
}

//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Matrix34<Real>::GetColumn(UInt col) const
{
	return Vector3<Real>(mEntry[0][col], mEntry[1][col], mEntry[2][col]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix34<Real> Matrix34<Real>::operator* (const Matrix34& rMatrix)
const
{
	Matrix34<Real> result;
	MTXConcat(const_cast<Real4*>(mEntry), const_cast<Matrix34&>(rMatrix),
		result);
	return result;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> Matrix34<Real>::operator* (const Matrix4<Real>& rMatrix) const
{
	return Matrix4<Real>(
		mEntry[0][0] * rMatrix[0][0] +
		mEntry[0][1] * rMatrix[1][0] +
		mEntry[0][2] * rMatrix[2][0] +
		mEntry[0][3] * rMatrix[3][0],

		mEntry[0][0] * rMatrix[0][1] +
		mEntry[0][1] * rMatrix[1][1] +
		mEntry[0][2] * rMatrix[2][1] +
		mEntry[0][3] * rMatrix[3][1],

		mEntry[0][0] * rMatrix[0][2] +
		mEntry[0][1] * rMatrix[1][2] +
		mEntry[0][2] * rMatrix[2][2] +
		mEntry[0][3] * rMatrix[3][2],

		mEntry[0][0] * rMatrix[0][3] +
		mEntry[0][1] * rMatrix[1][3] +
		mEntry[0][2] * rMatrix[2][3] +
		mEntry[0][3] * rMatrix[3][3],

		mEntry[1][0] * rMatrix[0][0] +
		mEntry[1][1] * rMatrix[1][0] +
		mEntry[1][2] * rMatrix[2][0] +
		mEntry[1][3] * rMatrix[3][0],

		mEntry[1][0] * rMatrix[0][1] +
		mEntry[1][1] * rMatrix[1][1] +
		mEntry[1][2] * rMatrix[2][1] +
		mEntry[1][3] * rMatrix[3][1],

		mEntry[1][0] * rMatrix[0][2] +
		mEntry[1][1] * rMatrix[1][2] +
		mEntry[1][2] * rMatrix[2][2] +
		mEntry[1][3] * rMatrix[3][2],

		mEntry[1][0] * rMatrix[0][3] +
		mEntry[1][1] * rMatrix[1][3] +
		mEntry[1][2] * rMatrix[2][3] +
		mEntry[1][3] * rMatrix[3][3],

		mEntry[2][0] * rMatrix[0][0] +
		mEntry[2][1] * rMatrix[1][0] +
		mEntry[2][2] * rMatrix[2][0] +
		mEntry[2][3] * rMatrix[3][0],

		mEntry[2][0] * rMatrix[0][1] +
		mEntry[2][1] * rMatrix[1][1] +
		mEntry[2][2] * rMatrix[2][1] +
		mEntry[2][3] * rMatrix[3][1],

		mEntry[2][0] * rMatrix[0][2] +
		mEntry[2][1] * rMatrix[1][2] +
		mEntry[2][2] * rMatrix[2][2] +
		mEntry[2][3] * rMatrix[3][2],

		mEntry[2][0] * rMatrix[0][3] +
		mEntry[2][1] * rMatrix[1][3] +
		mEntry[2][2] * rMatrix[2][3] +
		mEntry[2][3] * rMatrix[3][3],

		rMatrix[3][0],
		rMatrix[3][1],
		rMatrix[3][2],
		rMatrix[3][3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Matrix34<Real>::operator* (const Vector3<Real>& rV) const
{
	Vector3<Real> result;
	Vector3<Real>* pResult = const_cast<Vector3<Real>*>(&result);
	Vector3<Real>* pVector = const_cast<Vector3<Real>*>(&rV);
	MTXMultVec(const_cast<Real4*>(mEntry), reinterpret_cast<Vec*>(pVector),
		reinterpret_cast<Vec*>(pResult));
	return result;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Matrix34<Real>::operator* (const Vector4<Real>& rV) const
{
	return Vector4<Real>(
		mEntry[0][0] * rV[0] + mEntry[0][1] * rV[1] + mEntry[0][2] * rV[2] + mEntry[0][3] * rV[3],
		mEntry[1][0] * rV[0] + mEntry[1][1] * rV[1] + mEntry[1][2] * rV[2] + mEntry[1][3] * rV[3],
		mEntry[2][0] * rV[0] + mEntry[2][1] * rV[1] + mEntry[2][2] * rV[2] + mEntry[2][3] * rV[3],
		rV[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Matrix34<Real>::Times3(const Vector3<Real>& rV) const
{
	Vector3<Real> result;
	Vector3<Real>* pResult = const_cast<Vector3<Real>*>(&result);
	Vector3<Real>* pVector = const_cast<Vector3<Real>*>(&rV);
	MTXMultVecSR(const_cast<Real4*>(mEntry), reinterpret_cast<Vec*>(pVector),
		reinterpret_cast<Vec*>(pResult));
	return result;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Matrix34<Real>::Times3Row(const Vector3<Real>& rV) const
{
	return Vector3<Real>(
		rV[0] * mEntry[0][0] + rV[1] * mEntry[1][0] + rV[2] * mEntry[2][0],
		rV[0] * mEntry[0][1] + rV[1] * mEntry[1][1] + rV[2] * mEntry[2][1],
		rV[0] * mEntry[0][2] + rV[1] * mEntry[1][2] + rV[2] * mEntry[2][2]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real> Matrix34<Real>::TimesDiagonal(const Vector3<Real>& rDiag) const
{
	return Matrix34<Real>(
		mEntry[0][0]*rDiag[0], mEntry[0][1]*rDiag[1], mEntry[0][2]*rDiag[2],
			mEntry[0][3],
		mEntry[1][0]*rDiag[0], mEntry[1][1]*rDiag[1], mEntry[1][2]*rDiag[2],
			mEntry[1][3],
		mEntry[2][0]*rDiag[0], mEntry[2][1]*rDiag[1], mEntry[2][2]*rDiag[2],
			mEntry[2][3]);
}

//----------------------------------------------------------------------------
template <class Real>
Matrix34<Real> Matrix34<Real>::Inverse() const
{
	Real a0 = mEntry[0][0] * mEntry[1][1] - mEntry[0][1] * mEntry[1][0];
	Real a1 = mEntry[0][0] * mEntry[1][2] - mEntry[0][2] * mEntry[1][0];
	Real a2 = mEntry[0][0] * mEntry[1][3] - mEntry[0][3] * mEntry[1][0];
	Real a3 = mEntry[0][1] * mEntry[1][2] - mEntry[0][2] * mEntry[1][1];
	Real a4 = mEntry[0][1] * mEntry[1][3] - mEntry[0][3] * mEntry[1][1];
	Real a5 = mEntry[0][2] * mEntry[1][3] - mEntry[0][3] * mEntry[1][2];

	Real det = a0 * mEntry[2][2] - a1 * mEntry[2][1] + a3 * mEntry[2][0];
	if (Math<Real>::FAbs(det) <= Math<Real>::ZERO_TOLERANCE)
	{
		return Matrix34<Real>::ZERO;
	}

	Matrix34 inv;
	inv.mEntry[0][0] = + mEntry[1][1] * mEntry[2][2] - mEntry[1][2] * mEntry[2][1];
	inv.mEntry[1][0] = - mEntry[1][0] * mEntry[2][2] + mEntry[1][2] * mEntry[2][0];
	inv.mEntry[2][0] = + mEntry[1][0] * mEntry[2][1] - mEntry[1][1] * mEntry[2][0];
	inv.mEntry[0][1] = - mEntry[0][1] * mEntry[2][2] + mEntry[0][2] * mEntry[2][1];
	inv.mEntry[1][1] = + mEntry[0][0] * mEntry[2][2] - mEntry[0][2] * mEntry[2][0];
	inv.mEntry[2][1] = - mEntry[0][0] * mEntry[2][1] + mEntry[0][1] * mEntry[2][0];
	inv.mEntry[0][2] = + a3;
	inv.mEntry[1][2] = - a1;
	inv.mEntry[2][2] = + a0;
	inv.mEntry[0][3] = - mEntry[2][1] * a5 + mEntry[2][2] * a4 - mEntry[2][3] * a3;
	inv.mEntry[1][3] = + mEntry[2][0] * a5 - mEntry[2][2] * a2 + mEntry[2][3] * a1;
	inv.mEntry[2][3] = - mEntry[2][0] * a4 + mEntry[2][1] * a2 - mEntry[2][3] * a0;

	Real invDet = static_cast<Real>(1.0) / det;
	inv.mEntry[0][0] *= invDet;
	inv.mEntry[0][1] *= invDet;
	inv.mEntry[0][2] *= invDet;
	inv.mEntry[0][3] *= invDet;
	inv.mEntry[1][0] *= invDet;
	inv.mEntry[1][1] *= invDet;
	inv.mEntry[1][2] *= invDet;
	inv.mEntry[1][3] *= invDet;
	inv.mEntry[2][0] *= invDet;
	inv.mEntry[2][1] *= invDet;
	inv.mEntry[2][2] *= invDet;
	inv.mEntry[2][3] *= invDet;

	return inv;
}

//----------------------------------------------------------------------------
template <class Real>
inline Matrix4<Real> operator* (const Matrix4<Real>& rM4, const Matrix34<Real>& rM34)
{
	return Matrix4<Real>(
	rM4[0][0] * rM34[0][0] + rM4[0][1] * rM34[1][0] + rM4[0][2] * rM34[2][0],
	rM4[0][0] * rM34[0][1] + rM4[0][1] * rM34[1][1] + rM4[0][2] * rM34[2][1],
	rM4[0][0] * rM34[0][2] + rM4[0][1] * rM34[1][2] + rM4[0][2] * rM34[2][2],
	rM4[0][0] * rM34[0][3] + rM4[0][1] * rM34[1][3] + rM4[0][2] * rM34[2][3] + rM4[0][3],

	rM4[1][0] * rM34[0][0] + rM4[1][1] * rM34[1][0] + rM4[1][2] * rM34[2][0],
	rM4[1][0] * rM34[0][1] + rM4[1][1] * rM34[1][1] + rM4[1][2] * rM34[2][1],
	rM4[1][0] * rM34[0][2] + rM4[1][1] * rM34[1][2] + rM4[1][2] * rM34[2][2],
	rM4[1][0] * rM34[0][3] + rM4[1][1] * rM34[1][3] + rM4[1][2] * rM34[2][3] + rM4[1][3],

	rM4[2][0] * rM34[0][0] + rM4[2][1] * rM34[1][0] + rM4[2][2] * rM34[2][0],
	rM4[2][0] * rM34[0][1] + rM4[2][1] * rM34[1][1] + rM4[2][2] * rM34[2][1],
	rM4[2][0] * rM34[0][2] + rM4[2][1] * rM34[1][2] + rM4[2][2] * rM34[2][2],
	rM4[2][0] * rM34[0][3] + rM4[2][1] * rM34[1][3] + rM4[2][2] * rM34[2][3] + rM4[2][3],

	rM4[3][0] * rM34[0][0] + rM4[3][1] * rM34[1][0] + rM4[3][2] * rM34[2][0],
	rM4[3][0] * rM34[0][1] + rM4[3][1] * rM34[1][1] + rM4[3][2] * rM34[2][1],
	rM4[3][0] * rM34[0][2] + rM4[3][1] * rM34[1][2] + rM4[3][2] * rM34[2][2],
	rM4[3][0] * rM34[0][3] + rM4[3][1] * rM34[1][3] + rM4[3][2] * rM34[2][3] + rM4[3][3]);
}
