// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Vector3<Real>::Vector3()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Vector3<Real>::Vector3(Real x, Real y, Real z)
{
	mTuple[0] = x;
	mTuple[1] = y;
	mTuple[2] = z;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>::operator Real* ()
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>::operator const Real* () const
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::X() const
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector3<Real>::X()
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Y() const
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector3<Real>::Y()
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Z() const
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector3<Real>::Z()
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator= (const Vector3& rVector)
{
	mTuple[0] = rVector.mTuple[0];
	mTuple[1] = rVector.mTuple[1];
	mTuple[2] = rVector.mTuple[2];

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Vector3<Real>::operator== (const Vector3& rVector) const
{
	return (
		mTuple[0] == rVector.mTuple[0] &&
		mTuple[1] == rVector.mTuple[1] &&
		mTuple[2] == rVector.mTuple[2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Vector3<Real>::operator!= (const Vector3& rVector) const
{
	return !(
		mTuple[0] == rVector.mTuple[0] &&
		mTuple[1] == rVector.mTuple[1] &&
		mTuple[2] == rVector.mTuple[2]);

}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator+ (const Vector3& rVector)
const
{
	return Vector3(
		mTuple[0] + rVector.mTuple[0],
		mTuple[1] + rVector.mTuple[1],
		mTuple[2] + rVector.mTuple[2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator- (const Vector3& rVector)
const
{
	return Vector3(
		mTuple[0] - rVector.mTuple[0],
		mTuple[1] - rVector.mTuple[1],
		mTuple[2] - rVector.mTuple[2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator* (Real scalar) const
{
	return Vector3(
		scalar * mTuple[0],
		scalar * mTuple[1],
		scalar * mTuple[2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator/ (Real scalar) const
{
	Vector3 quot;

	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		quot.mTuple[0] = invScalar * mTuple[0];
		quot.mTuple[1] = invScalar * mTuple[1];
		quot.mTuple[2] = invScalar * mTuple[2];
	}
	else
	{
		quot.mTuple[0] = Math<Real>::MAX_REAL;
		quot.mTuple[1] = Math<Real>::MAX_REAL;
		quot.mTuple[2] = Math<Real>::MAX_REAL;
	}

	return quot;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator- () const
{
	return Vector3(
		-mTuple[0],
		-mTuple[1],
		-mTuple[2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator+= (const Vector3& rVector)
{
	mTuple[0] += rVector.mTuple[0];
	mTuple[1] += rVector.mTuple[1];
	mTuple[2] += rVector.mTuple[2];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator-= (const Vector3& rVector)
{
	mTuple[0] -= rVector.mTuple[0];
	mTuple[1] -= rVector.mTuple[1];
	mTuple[2] -= rVector.mTuple[2];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator*= (Real scalar)
{
	mTuple[0] *= scalar;
	mTuple[1] *= scalar;
	mTuple[2] *= scalar;
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator/= (Real scalar)
{
	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		mTuple[0] *= invScalar;
		mTuple[1] *= invScalar;
		mTuple[2] *= invScalar;
	}
	else
	{
		mTuple[0] = Math<Real>::MAX_REAL;
		mTuple[1] = Math<Real>::MAX_REAL;
		mTuple[2] = Math<Real>::MAX_REAL;
	}

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Length() const
{
	return Math<Real>::Sqrt(
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1] +
		mTuple[2] * mTuple[2]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::SquaredLength() const
{
	return
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1] +
		mTuple[2] * mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Dot(const Vector3& rVector) const
{
	return
		mTuple[0] * rVector.mTuple[0] +
		mTuple[1] * rVector.mTuple[1] +
		mTuple[2] * rVector.mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Normalize()
{
	Real length = Length();

	if (length > Math<Real>::ZERO_TOLERANCE)
	{
		Real invLength = (static_cast<Real>(1.0)) / length;
		mTuple[0] *= invLength;
		mTuple[1] *= invLength;
		mTuple[2] *= invLength;
	}
	else
	{
		length = static_cast<Real>(0.0);
		mTuple[0] = static_cast<Real>(0.0);
		mTuple[1] = static_cast<Real>(0.0);
		mTuple[2] = static_cast<Real>(0.0);
	}

	return length;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::Cross(const Vector3& rVector) const
{
	return Vector3(
		mTuple[1]*rVector.mTuple[2] - mTuple[2]*rVector.mTuple[1],
		mTuple[2]*rVector.mTuple[0] - mTuple[0]*rVector.mTuple[2],
		mTuple[0]*rVector.mTuple[1] - mTuple[1]*rVector.mTuple[0]);
}

//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::Orthonormalize(Vector3& rU, Vector3& rV, Vector3& rW)
{
	// If the input vectors are v0, v1, and v2, then the Gram-Schmidt
	// orthonormalization produces vectors u0, u1, and u2 as follows,
	//
	//   u0 = v0/|v0|
	//   u1 = (v1-(u0*v1)u0)/|v1-(u0*v1)u0|
	//   u2 = (v2-(u0*v2)u0-(u1*v2)u1)/|v2-(u0*v2)u0-(u1*v2)u1|
	//
	// where |A| indicates length of vector A and A*B indicates dot
	// product of vectors A and B.

	// compute u0
	rU.Normalize();

	// compute u1
	Real dot0 = rU.Dot(rV); 
	rV -= dot0*rU;
	rV.Normalize();

	// compute u2
	Real dot1 = rV.Dot(rW);
	dot0 = rU.Dot(rW);
	rW -= dot0*rU + dot1*rV;
	rW.Normalize();
}

//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::GenerateOrthonormalBasis(Vector3& rU, Vector3& rV,
	Vector3& rW)
{
	rW.Normalize();
	GenerateComplementBasis(rU, rV, rW);
}

//----------------------------------------------------------------------------
template <class Real>
void Vector3<Real>::GenerateComplementBasis(Vector3& rU, Vector3& rV,
	const Vector3& rW)
{
	if (Math<Real>::FAbs(rW.mTuple[0]) >= Math<Real>::FAbs(rW.mTuple[1]))
	{
		// W.x or W.z is the largest magnitude component, swap them
		Real invLength = Math<Real>::InvSqrt(rW.mTuple[0] * rW.mTuple[0] +
			rW.mTuple[2] * rW.mTuple[2]);
		rU.mTuple[0] = -rW.mTuple[2] * invLength;
		rU.mTuple[1] = static_cast<Real>(0.0);
		rU.mTuple[2] = +rW.mTuple[0] * invLength;
		rV.mTuple[0] = rW.mTuple[1] * rU.mTuple[2];
		rV.mTuple[1] = rW.mTuple[2]*rU.mTuple[0] - rW.mTuple[0]*rU.mTuple[2];
		rV.mTuple[2] = -rW.mTuple[1] * rU.mTuple[0];
	}
	else
	{
		// W.y or W.z is the largest magnitude component, swap them
		Real invLength = Math<Real>::InvSqrt(rW.mTuple[1] * rW.mTuple[1] +
			rW.mTuple[2] * rW.mTuple[2]);
		rU.mTuple[0] = static_cast<Real>(0.0);
		rU.mTuple[1] = +rW.mTuple[2] * invLength;
		rU.mTuple[2] = -rW.mTuple[1] * invLength;
		rV.mTuple[0] = rW.mTuple[1]*rU.mTuple[2] - rW.mTuple[2]*rU.mTuple[1];
		rV.mTuple[1] = -rW.mTuple[0] * rU.mTuple[2];
		rV.mTuple[2] = rW.mTuple[0] * rU.mTuple[1];
	}
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> operator* (Real scalar, const Vector3<Real>& rVector)
{
	return Vector3<Real>(
		scalar * rVector[0],
		scalar * rVector[1],
		scalar * rVector[2]);
}
