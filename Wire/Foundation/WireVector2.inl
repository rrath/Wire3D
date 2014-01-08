// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Vector2<Real>::Vector2()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Vector2<Real>::Vector2(Real x, Real y)
{
	mTuple[0] = x;
	mTuple[1] = y;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real>::operator Real* ()
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real>::operator const Real* () const
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector2<Real>::X() const
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector2<Real>::X()
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector2<Real>::Y() const
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector2<Real>::Y()
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real>& Vector2<Real>::operator= (const Vector2& rVector)
{
	mTuple[0] = rVector.mTuple[0];
	mTuple[1] = rVector.mTuple[1];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Vector2<Real>::operator== (const Vector2& rVector) const
{
	return (
		mTuple[0] == rVector.mTuple[0] &&
		mTuple[1] == rVector.mTuple[1]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Vector2<Real>::operator!= (const Vector2& rVector) const
{
	return !(
		mTuple[0] == rVector.mTuple[0] &&
		mTuple[1] == rVector.mTuple[1]);

}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> Vector2<Real>::operator+ (const Vector2& rVector) const
{
	return Vector2(
		mTuple[0] + rVector.mTuple[0],
		mTuple[1] + rVector.mTuple[1]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> Vector2<Real>::operator- (const Vector2& rVector) const
{
	return Vector2(
		mTuple[0] - rVector.mTuple[0],
		mTuple[1] - rVector.mTuple[1]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> Vector2<Real>::operator* (Real scalar) const
{
	return Vector2(
		scalar * mTuple[0],
		scalar * mTuple[1]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> Vector2<Real>::operator/ (Real scalar) const
{
	Vector2 quot;

	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		quot.mTuple[0] = invScalar * mTuple[0];
		quot.mTuple[1] = invScalar * mTuple[1];
	}
	else
	{
		quot.mTuple[0] = Math<Real>::MAX_REAL;
		quot.mTuple[1] = Math<Real>::MAX_REAL;
	}

	return quot;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> Vector2<Real>::operator- () const
{
	return Vector2(
		-mTuple[0],
		-mTuple[1]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real>& Vector2<Real>::operator+= (const Vector2& rVector)
{
	mTuple[0] += rVector.mTuple[0];
	mTuple[1] += rVector.mTuple[1];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real>& Vector2<Real>::operator-= (const Vector2& rVector)
{
	mTuple[0] -= rVector.mTuple[0];
	mTuple[1] -= rVector.mTuple[1];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real>& Vector2<Real>::operator*= (Real scalar)
{
	mTuple[0] *= scalar;
	mTuple[1] *= scalar;
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real>& Vector2<Real>::operator/= (Real scalar)
{
	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		mTuple[0] *= invScalar;
		mTuple[1] *= invScalar;
	}
	else
	{
		mTuple[0] = Math<Real>::MAX_REAL;
		mTuple[1] = Math<Real>::MAX_REAL;
	}

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector2<Real>::Length() const
{
	return Math<Real>::Sqrt(
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector2<Real>::SquaredLength() const
{
	return
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector2<Real>::Dot(const Vector2& rVector) const
{
	return
		mTuple[0] * rVector.mTuple[0] +
		mTuple[1] * rVector.mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector2<Real>::Normalize()
{
	Real length = Length();

	if (length > Math<Real>::ZERO_TOLERANCE)
	{
		Real invLength = (static_cast<Real>(1.0)) / length;
		mTuple[0] *= invLength;
		mTuple[1] *= invLength;
	}
	else
	{
		length = static_cast<Real>(0.0);
		mTuple[0] = static_cast<Real>(0.0);
		mTuple[1] = static_cast<Real>(0.0);
	}

	return length;
}

//----------------------------------------------------------------------------
template <class Real>
void Vector2<Real>::Orthonormalize(Vector2& rU, Vector2& rV)
{
	// If the input vectors are v0 and v1, then the Gram-Schmidt
	// orthonormalization produces vectors u0 and u1 as follows,
	//
	//   u0 = v0/|v0|
	//   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
	//
	// where |A| indicates length of vector A and A*B indicates dot
	// product of vectors A and B.

	// compute u0
	rU.Normalize();

	// compute u1
	Real dot0 = rU.Dot(rV); 
	rV -= rU * dot0;
	rV.Normalize();
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector2<Real> operator* (Real scalar, const Vector2<Real>& rVector)
{
	return Vector2<Real>(
		scalar * rVector[0],
		scalar * rVector[1]);
}
