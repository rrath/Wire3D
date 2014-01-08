// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Vector4<Real>::Vector4()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Vector4<Real>::Vector4(Real x, Real y, Real z, Real w)
{
	mTuple[0] = x;
	mTuple[1] = y;
	mTuple[2] = z;
	mTuple[3] = w;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real>::operator Real* ()
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real>::operator const Real* () const
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::X() const
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector4<Real>::X()
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::Y() const
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector4<Real>::Y()
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::Z() const
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector4<Real>::Z()
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::W() const
{
	return mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector4<Real>::W()
{
	return mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real>& Vector4<Real>::operator= (const Vector4& rVector)
{
	mTuple[0] = rVector.mTuple[0];
	mTuple[1] = rVector.mTuple[1];
	mTuple[2] = rVector.mTuple[2];
	mTuple[3] = rVector.mTuple[3];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Vector4<Real>::operator== (const Vector4& rVector) const
{
	return (
		mTuple[0] == rVector.mTuple[0] &&
		mTuple[1] == rVector.mTuple[1] &&
		mTuple[2] == rVector.mTuple[2] &&
		mTuple[3] == rVector.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Vector4<Real>::operator!= (const Vector4& rVector) const
{
	return !(
		mTuple[0] == rVector.mTuple[0] &&
		mTuple[1] == rVector.mTuple[1] &&
		mTuple[2] == rVector.mTuple[2] &&
		mTuple[3] == rVector.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Vector4<Real>::operator+ (const Vector4& rVector) const
{
	return Vector4(
		mTuple[0] + rVector.mTuple[0],
		mTuple[1] + rVector.mTuple[1],
		mTuple[2] + rVector.mTuple[2],
		mTuple[3] + rVector.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Vector4<Real>::operator- (const Vector4& rVector) const
{
	return Vector4(
		mTuple[0] - rVector.mTuple[0],
		mTuple[1] - rVector.mTuple[1],
		mTuple[2] - rVector.mTuple[2],
		mTuple[3] - rVector.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Vector4<Real>::operator* (Real scalar) const
{
	return Vector4(
		scalar * mTuple[0],
		scalar * mTuple[1],
		scalar * mTuple[2],
		scalar * mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Vector4<Real>::operator/ (Real scalar) const
{
	Vector4 quot;

	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		quot.mTuple[0] = invScalar * mTuple[0];
		quot.mTuple[1] = invScalar * mTuple[1];
		quot.mTuple[2] = invScalar * mTuple[2];
		quot.mTuple[3] = invScalar * mTuple[3];
	}
	else
	{
		quot.mTuple[0] = Math<Real>::MAX_REAL;
		quot.mTuple[1] = Math<Real>::MAX_REAL;
		quot.mTuple[2] = Math<Real>::MAX_REAL;
		quot.mTuple[3] = Math<Real>::MAX_REAL;
	}

	return quot;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> Vector4<Real>::operator- () const
{
	return Vector4(
		-mTuple[0],
		-mTuple[1],
		-mTuple[2],
		-mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real>& Vector4<Real>::operator+= (const Vector4& rVector)
{
	mTuple[0] += rVector.mTuple[0];
	mTuple[1] += rVector.mTuple[1];
	mTuple[2] += rVector.mTuple[2];
	mTuple[3] += rVector.mTuple[3];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real>& Vector4<Real>::operator-= (const Vector4& rVector)
{
	mTuple[0] -= rVector.mTuple[0];
	mTuple[1] -= rVector.mTuple[1];
	mTuple[2] -= rVector.mTuple[2];
	mTuple[3] -= rVector.mTuple[3];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real>& Vector4<Real>::operator*= (Real scalar)
{
	mTuple[0] *= scalar;
	mTuple[1] *= scalar;
	mTuple[2] *= scalar;
	mTuple[3] *= scalar;
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real>& Vector4<Real>::operator/= (Real scalar)
{
	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		mTuple[0] *= invScalar;
		mTuple[1] *= invScalar;
		mTuple[2] *= invScalar;
		mTuple[3] *= invScalar;
	}
	else
	{
		mTuple[0] = Math<Real>::MAX_REAL;
		mTuple[1] = Math<Real>::MAX_REAL;
		mTuple[2] = Math<Real>::MAX_REAL;
		mTuple[3] = Math<Real>::MAX_REAL;
	}

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::Length() const
{
	return Math<Real>::Sqrt(
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1] +
		mTuple[2] * mTuple[2] +
		mTuple[3] * mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::SquaredLength() const
{
	return
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1] +
		mTuple[2] * mTuple[2] +
		mTuple[3] * mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::Dot(const Vector4& rVector) const
{
	return
		mTuple[0] * rVector.mTuple[0] +
		mTuple[1] * rVector.mTuple[1] +
		mTuple[2] * rVector.mTuple[2] +
		mTuple[3] * rVector.mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector4<Real>::Normalize()
{
	Real length = Length();

	if (length > Math<Real>::ZERO_TOLERANCE)
	{
		Real invLength = (static_cast<Real>(1.0)) / length;
		mTuple[0] *= invLength;
		mTuple[1] *= invLength;
		mTuple[2] *= invLength;
		mTuple[3] *= invLength;
	}
	else
	{
		length = static_cast<Real>(0.0);
		mTuple[0] = static_cast<Real>(0.0);
		mTuple[1] = static_cast<Real>(0.0);
		mTuple[2] = static_cast<Real>(0.0);
		mTuple[3] = static_cast<Real>(0.0);
	}

	return length;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector4<Real> operator* (Real scalar, const Vector4<Real>& rVector)
{
	return Vector4<Real>(
		scalar * rVector[0],
		scalar * rVector[1],
		scalar * rVector[2],
		scalar * rVector[3]);
}
