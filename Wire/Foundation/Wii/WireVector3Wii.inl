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
	mTuple.x = x;
	mTuple.y = y;
	mTuple.z = z;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>::operator Real* ()
{
	return reinterpret_cast<Real*>(&mTuple);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>::operator const Real* () const
{
	return reinterpret_cast<Real*>(const_cast<Vec*>(&mTuple));
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::X() const
{
	return mTuple.x;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector3<Real>::X()
{
	return mTuple.x;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Y() const
{
	return mTuple.y;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector3<Real>::Y()
{
	return mTuple.y;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Z() const
{
	return mTuple.z;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Vector3<Real>::Z()
{
	return mTuple.z;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator= (const Vector3& rVector)
{
	mTuple.x = rVector.mTuple.x;
	mTuple.y = rVector.mTuple.y;
	mTuple.z = rVector.mTuple.z;
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
Bool Vector3<Real>::operator== (const Vector3& rVector) const
{
	return (
		mTuple.x == rVector.mTuple.x &&
		mTuple.y == rVector.mTuple.y &&
		mTuple.z == rVector.mTuple.z);
}

//----------------------------------------------------------------------------
template <class Real>
Bool Vector3<Real>::operator!= (const Vector3& rVector) const
{
	return !(
		mTuple.x == rVector.mTuple.x &&
		mTuple.y == rVector.mTuple.y &&
		mTuple.z == rVector.mTuple.z);

}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator+ (const Vector3& rVector)
	const
{
	Vector3 result;
	VECAdd(const_cast<Vec*>(&mTuple), const_cast<Vec*>(&rVector.
		mTuple), &result.mTuple);
	return result;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator- (const Vector3& rVector)
	const
{
	Vector3 result;
	VECSubtract(const_cast<Vec*>(&mTuple), const_cast<Vec*>(&rVector.
		mTuple), &result.mTuple);
	return result;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator* (Real scalar) const
{
	Vector3 result;
	VECScale(const_cast<Vec*>(&mTuple), &result.mTuple, scalar);
	return result;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator/ (Real scalar) const
{
	Vector3 quot;

	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		VECScale(const_cast<Vec*>(&mTuple), &quot.mTuple, invScalar);
	}
	else
	{
		quot.mTuple.x = Math<Real>::MAX_REAL;
		quot.mTuple.y = Math<Real>::MAX_REAL;
		quot.mTuple.z = Math<Real>::MAX_REAL;
	}

	return quot;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::operator- () const
{
	return Vector3(
		-mTuple.x,
		-mTuple.y,
		-mTuple.z);
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator+= (const Vector3& rVector)
{
	VECAdd(&mTuple, const_cast<Vec*>(&rVector.mTuple), &mTuple);
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator-= (const Vector3& rVector)
{
	VECSubtract(&mTuple, const_cast<Vec*>(&rVector.mTuple), &mTuple);
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator*= (Real scalar)
{
	VECScale(&mTuple, &mTuple, scalar);
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real>& Vector3<Real>::operator/= (Real scalar)
{
	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0)) / scalar;
		VECScale(&mTuple, &mTuple, invScalar);
	}
	else
	{
		mTuple.x = Math<Real>::MAX_REAL;
		mTuple.y = Math<Real>::MAX_REAL;
		mTuple.z = Math<Real>::MAX_REAL;
	}

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Length() const
{
//	return VECMag(&mTuple);			// No OGC equivalent exists :(

	return Math<Real>::Sqrt(
		mTuple.x * mTuple.x +
		mTuple.y * mTuple.y +
		mTuple.z * mTuple.z);
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::SquaredLength() const
{
//	return VECSquareMag(&mTuple);	// No OGC equivalent exists :(

	return
		mTuple.x * mTuple.x +
		mTuple.y * mTuple.y +
		mTuple.z * mTuple.z;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Dot(const Vector3& rVector) const
{
	return VECDotProduct(const_cast<Vec*>(&mTuple), 
		const_cast<Vec*>(&rVector.mTuple));
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Vector3<Real>::Normalize()
{
	Real length = Length();

	if (length > Math<Real>::ZERO_TOLERANCE)
	{
		Real invLength = (static_cast<Real>(1.0)) / length;
		mTuple.x *= invLength;
		mTuple.y *= invLength;
		mTuple.z *= invLength;
	}
	else
	{
		length = static_cast<Real>(0.0);
		mTuple.x = static_cast<Real>(0.0);
		mTuple.y = static_cast<Real>(0.0);
		mTuple.z = static_cast<Real>(0.0);
	}

	return length;
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> Vector3<Real>::Cross(const Vector3& rVector) const
{
	Vector3 result;
	VECCrossProduct(const_cast<Vec*>(&mTuple), 
		const_cast<Vec*>(&rVector.mTuple), &result.mTuple);
	return result;
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
	if (Math<Real>::FAbs(rW.mTuple.x) >= Math<Real>::FAbs(rW.mTuple.y))
	{
		// W.x or W.z is the largest magnitude component, swap them
		Real invLength = Math<Real>::InvSqrt(rW.mTuple.x * rW.mTuple.x +
			rW.mTuple.z * rW.mTuple.z);
		rU.mTuple.x = -rW.mTuple.z * invLength;
		rU.mTuple.y = static_cast<Real>(0.0);
		rU.mTuple.z = +rW.mTuple.x * invLength;
		rV.mTuple.x = rW.mTuple.y * rU.mTuple.z;
		rV.mTuple.y = rW.mTuple.z*rU.mTuple.x - rW.mTuple.x*rU.mTuple.z;
		rV.mTuple.z = -rW.mTuple.y * rU.mTuple.x;
	}
	else
	{
		// W.y or W.z is the largest magnitude component, swap them
		Real invLength = Math<Real>::InvSqrt(rW.mTuple.y * rW.mTuple.y +
			rW.mTuple.z * rW.mTuple.z);
		rU.mTuple.x = static_cast<Real>(0.0);
		rU.mTuple.y = +rW.mTuple.z * invLength;
		rU.mTuple.z = -rW.mTuple.y * invLength;
		rV.mTuple.x = rW.mTuple.y*rU.mTuple.z - rW.mTuple.z*rU.mTuple.y;
		rV.mTuple.y = -rW.mTuple.x * rU.mTuple.z;
		rV.mTuple.z = rW.mTuple.x * rU.mTuple.y;
	}
}

//----------------------------------------------------------------------------
template <class Real>
inline Vector3<Real> operator* (Real scalar, const Vector3<Real>& rVector)
{
	Vector3<Real> result;
	VECScale(reinterpret_cast<Vec*>(const_cast<Vector3<Real>*>(&rVector)),
		reinterpret_cast<Vec*>(&result), scalar);
	return result;
}
