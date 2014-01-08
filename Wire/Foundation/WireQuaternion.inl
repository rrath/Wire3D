// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>::Quaternion()
{
	// uninitialized for performance in array construction
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>::Quaternion(Real w, Real x, Real y, Real z)
{
	mTuple[0] = w;
	mTuple[1] = x;
	mTuple[2] = y;
	mTuple[3] = z;
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>::Quaternion(const Quaternion& rQ)
{
	mTuple[0] = rQ.mTuple[0];
	mTuple[1] = rQ.mTuple[1];
	mTuple[2] = rQ.mTuple[2];
	mTuple[3] = rQ.mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>::Quaternion(const Matrix3<Real>& rRot)
{
	FromRotationMatrix(rRot);
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>::Quaternion(const Vector3<Real>& rAxis, Real angle)
{
	FromAxisAngle(rAxis, angle);
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>::Quaternion(const Vector3<Real> rotColumns[3])
{
	FromRotationMatrix(rotColumns);
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real>::operator const Real* () const
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real>::operator Real* ()
{
	return mTuple;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::operator[] (UInt i) const
{
	return mTuple[i];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Quaternion<Real>::operator[] (UInt i)
{
	return mTuple[i];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::W() const
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Quaternion<Real>::W()
{
	return mTuple[0];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::X() const
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Quaternion<Real>::X()
{
	return mTuple[1];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::Y() const
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Quaternion<Real>::Y()
{
	return mTuple[2];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::Z() const
{
	return mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real& Quaternion<Real>::Z()
{
	return mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real>& Quaternion<Real>::operator= (const Quaternion& rQ)
{
	mTuple[0] = rQ.mTuple[0];
	mTuple[1] = rQ.mTuple[1];
	mTuple[2] = rQ.mTuple[2];
	mTuple[3] = rQ.mTuple[3];
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Quaternion<Real>::operator== (const Quaternion& rQ) const
{
	return (
		mTuple[0] == rQ.mTuple[0] &&
		mTuple[1] == rQ.mTuple[1] &&
		mTuple[2] == rQ.mTuple[2] &&
		mTuple[3] == rQ.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Bool Quaternion<Real>::operator!= (const Quaternion& rQ) const
{
	return !(
		mTuple[0] == rQ.mTuple[0] &&
		mTuple[1] == rQ.mTuple[1] &&
		mTuple[2] == rQ.mTuple[2] &&
		mTuple[3] == rQ.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real> Quaternion<Real>::operator+ (const Quaternion& rQ)
	const
{
	return Quaternion(
		mTuple[0] + rQ.mTuple[0],
		mTuple[1] + rQ.mTuple[1],
		mTuple[2] + rQ.mTuple[2],
		mTuple[3] + rQ.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real> Quaternion<Real>::operator- (const Quaternion& rQ)
	const
{
	return Quaternion(
		mTuple[0] - rQ.mTuple[0],
		mTuple[1] - rQ.mTuple[1],
		mTuple[2] - rQ.mTuple[2],
		mTuple[3] - rQ.mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real> Quaternion<Real>::operator* (const Quaternion& rQ)
	const
{
	// NOTE:  Multiplication is not generally commutative, so in most
	// cases p*q != q*p.
	Quaternion prod;

	prod.mTuple[0] =
		mTuple[0] * rQ.mTuple[0] -
		mTuple[1] * rQ.mTuple[1] -
		mTuple[2] * rQ.mTuple[2] -
		mTuple[3] * rQ.mTuple[3];

	prod.mTuple[1] =
		mTuple[0] * rQ.mTuple[1] +
		mTuple[1] * rQ.mTuple[0] +
		mTuple[2] * rQ.mTuple[3] -
		mTuple[3] * rQ.mTuple[2];

	prod.mTuple[2] =
		mTuple[0] * rQ.mTuple[2] +
		mTuple[2] * rQ.mTuple[0] +
		mTuple[3] * rQ.mTuple[1] -
		mTuple[1] * rQ.mTuple[3];

	prod.mTuple[3] =
		mTuple[0] * rQ.mTuple[3] +
		mTuple[3] * rQ.mTuple[0] +
		mTuple[1] * rQ.mTuple[2] -
		mTuple[2] * rQ.mTuple[1];

	return prod;
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real> Quaternion<Real>::operator* (Real scalar) const
{
	return Quaternion(
		scalar * mTuple[0],
		scalar * mTuple[1],
		scalar * mTuple[2],
		scalar * mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real> Quaternion<Real>::operator/ (Real scalar) const
{
	Quaternion quot;

	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0))/scalar;
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
inline Quaternion<Real> Quaternion<Real>::operator- () const
{
	return Quaternion(-mTuple[0], -mTuple[1], -mTuple[2], -mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real>& Quaternion<Real>::operator+= (const Quaternion& rQ)
{
	mTuple[0] += rQ.mTuple[0];
	mTuple[1] += rQ.mTuple[1];
	mTuple[2] += rQ.mTuple[2];
	mTuple[3] += rQ.mTuple[3];

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real>& Quaternion<Real>::operator-= (const Quaternion& rQ)
{
	mTuple[0] -= rQ.mTuple[0];
	mTuple[1] -= rQ.mTuple[1];
	mTuple[2] -= rQ.mTuple[2];
	mTuple[3] -= rQ.mTuple[3];

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real>& Quaternion<Real>::operator*= (Real scalar)
{
	mTuple[0] *= scalar;
	mTuple[1] *= scalar;
	mTuple[2] *= scalar;
	mTuple[3] *= scalar;

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real>& Quaternion<Real>::operator/= (Real scalar)
{
	if (scalar != static_cast<Real>(0.0))
	{
		Real invScalar = (static_cast<Real>(1.0))/scalar;
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
Quaternion<Real>& Quaternion<Real>::FromRotationMatrix(
	const Matrix3<Real>& rRot)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
	// article "Quaternion Calculus and Fast Animation".

	Real trace = rRot(0,0) + rRot(1,1) + rRot(2,2);
	Real root;

	if (trace > static_cast<Real>(0.0))
	{
		// |w| > 1/2, may as well choose w > 1/2
		root = Math<Real>::Sqrt(trace + static_cast<Real>(1.0));  // 2w
		mTuple[0] = (static_cast<Real>(0.5))*root;
		root = (static_cast<Real>(0.5))/root;  // 1/(4w)
		mTuple[1] = (rRot(2,1)-rRot(1,2))*root;
		mTuple[2] = (rRot(0,2)-rRot(2,0))*root;
		mTuple[3] = (rRot(1,0)-rRot(0,1))*root;
	}
	else
	{
		// |w| <= 1/2
		Int i = 0;
		if (rRot(1,1) > rRot(0,0))
		{
			i = 1;
		}

		if (rRot(2,2) > rRot(i,i))
		{
			i = 2;
		}

		static const UInt s_next[3] = { 1, 2, 0 };
		UInt j = s_next[i];
		UInt k = s_next[j];

		root = Math<Real>::Sqrt(rRot(i,i)-rRot(j,j)-rRot(k,k)+
			static_cast<Real>(1.0));
		Real* pQuat[3] = { &mTuple[1], &mTuple[2], &mTuple[3] };
		*pQuat[i] = (static_cast<Real>(0.5))*root;
		root = (static_cast<Real>(0.5))/root;
		mTuple[0] = (rRot(k,j)-rRot(j,k))*root;
		*pQuat[j] = (rRot(j,i)+rRot(i,j))*root;
		*pQuat[k] = (rRot(k,i)+rRot(i,k))*root;
	}

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
void Quaternion<Real>::ToRotationMatrix(Matrix3<Real>& rRot) const
{
	Real tx  = static_cast<Real>(2.0) * mTuple[1];
	Real ty  = static_cast<Real>(2.0) * mTuple[2];
	Real tz  = static_cast<Real>(2.0) * mTuple[3];
	Real twx = tx * mTuple[0];
	Real twy = ty * mTuple[0];
	Real twz = tz * mTuple[0];
	Real txx = tx * mTuple[1];
	Real txy = ty * mTuple[1];
	Real txz = tz * mTuple[1];
	Real tyy = ty * mTuple[2];
	Real tyz = tz * mTuple[2];
	Real tzz = tz * mTuple[3];

	rRot(0,0) = static_cast<Real>(1.0) - (tyy + tzz);
	rRot(0,1) = txy - twz;
	rRot(0,2) = txz + twy;
	rRot(1,0) = txy + twz;
	rRot(1,1) = static_cast<Real>(1.0) - (txx + tzz);
	rRot(1,2) = tyz - twx;
	rRot(2,0) = txz - twy;
	rRot(2,1) = tyz + twx;
	rRot(2,2) = static_cast<Real>(1.0) - (txx + tyy);
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>& Quaternion<Real>::FromAxisAngle(const Vector3<Real>& rAxis,
	Real angle)
{
	WIRE_ASSERT(
		(rAxis.Length() < static_cast<Real>(1.0) + Math<Real>::ZERO_TOLERANCE) &&
		(rAxis.Length() > static_cast<Real>(1.0) - Math<Real>::ZERO_TOLERANCE));

	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	Real halfAngle = (static_cast<Real>(0.5)) * angle;
	Real sin = Math<Real>::Sin(halfAngle);
	mTuple[0] = Math<Real>::Cos(halfAngle);
	mTuple[1] = sin*rAxis[0];
	mTuple[2] = sin*rAxis[1];
	mTuple[3] = sin*rAxis[2];

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
void Quaternion<Real>::ToAxisAngle(Vector3<Real>& rAxis, Real& rAngle) const
{
	// The quaternion representing the rotation is
	//   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

	Real sqrLength = mTuple[1]*mTuple[1] + mTuple[2]*mTuple[2] +
		mTuple[3]*mTuple[3];

	if (sqrLength > Math<Real>::ZERO_TOLERANCE)
	{
		rAngle = (static_cast<Real>(2.0)) * Math<Real>::ACos(mTuple[0]);
		Real invLength = Math<Real>::InvSqrt(sqrLength);
		rAxis[0] = mTuple[1] * invLength;
		rAxis[1] = mTuple[2] * invLength;
		rAxis[2] = mTuple[3] * invLength;
	}
	else
	{
		// angle is 0 (mod 2*pi), so any axis will do
		rAngle = static_cast<Real>(0.0);
		rAxis[0] = static_cast<Real>(1.0);
		rAxis[1] = static_cast<Real>(0.0);
		rAxis[2] = static_cast<Real>(0.0);
	}
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::Length() const
{
	return Math<Real>::Sqrt(
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1] +
		mTuple[2] * mTuple[2] +
		mTuple[3] * mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::SquaredLength() const
{
	return
		mTuple[0] * mTuple[0] +
		mTuple[1] * mTuple[1] +
		mTuple[2] * mTuple[2] +
		mTuple[3] * mTuple[3];
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::Dot(const Quaternion& rQ) const
{
	Real dot = mTuple[0] * rQ.mTuple[0];
	dot += (mTuple[1] * rQ.mTuple[1]);
	dot += (mTuple[2] * rQ.mTuple[2]);
	dot += (mTuple[3] * rQ.mTuple[3]);

	return dot;
}

//----------------------------------------------------------------------------
template <class Real>
inline Real Quaternion<Real>::Normalize()
{
	Real length = Length();

	if (length > Math<Real>::ZERO_TOLERANCE)
	{
		Real invLength = (static_cast<Real>(1.0))/length;
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
Quaternion<Real> Quaternion<Real>::Inverse() const
{
	Quaternion inverse;

	Real norm = static_cast<Real>(0.0);
	for (UInt i = 0; i < 4; i++)
	{
		norm += mTuple[i] * mTuple[i];
	}

	if (norm > static_cast<Real>(0.0))
	{
		Real invNorm = static_cast<Real>(1.0) / norm;
		inverse.mTuple[0] = mTuple[0] * invNorm;
		inverse.mTuple[1] = -mTuple[1] * invNorm;
		inverse.mTuple[2] = -mTuple[2] * invNorm;
		inverse.mTuple[3] = -mTuple[3] * invNorm;
	}
	else
	{
		// return an invalid result to flag the error
		inverse.mTuple[0] = static_cast<Real>(0.0);
		inverse.mTuple[1] = static_cast<Real>(0.0);
		inverse.mTuple[2] = static_cast<Real>(0.0);
		inverse.mTuple[3] = static_cast<Real>(0.0);
	}

	return inverse;
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real> Quaternion<Real>::Conjugate() const
{
	return Quaternion(mTuple[0], -mTuple[1], -mTuple[2], -mTuple[3]);
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real> Quaternion<Real>::Exp() const
{
	// If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// exp(q) = cos(A)+sin(A)*(x*i+y*j+z*k). If sin(A) is near zero,
	// use exp(q) = cos(A)+A*(x*i+y*j+z*k) since A/sin(A) has limit 1.

	Quaternion result;

	Real angle = Math<Real>::Sqrt(mTuple[1] * mTuple[1] +
		mTuple[2] * mTuple[2] + mTuple[3] * mTuple[3]);

	Real sin = Math<Real>::Sin(angle);
	result.mTuple[0] = Math<Real>::Cos(angle);

	if (Math<Real>::FAbs(sin) >= Math<Real>::ZERO_TOLERANCE)
	{
		Real coeff = sin / angle;
		result.mTuple[1] = coeff * mTuple[1];
		result.mTuple[2] = coeff * mTuple[2];
		result.mTuple[3] = coeff * mTuple[3];
	}
	else
	{
		result.mTuple[1] = mTuple[1];
		result.mTuple[2] = mTuple[2];
		result.mTuple[3] = mTuple[3];
	}

	return result;
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real> Quaternion<Real>::Log() const
{
	// If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
	// log(q) = A*(x*i+y*j+z*k). If sin(A) is near zero, use log(q) =
	// sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

	Quaternion result;
	result.mTuple[0] = static_cast<Real>(0.0);

	if (Math<Real>::FAbs(mTuple[0]) < static_cast<Real>(1.0))
	{
		Real angle = Math<Real>::ACos(mTuple[0]);
		Real sin = Math<Real>::Sin(angle);
		if (Math<Real>::FAbs(sin) >= Math<Real>::ZERO_TOLERANCE)
		{
			Real coeff = angle / sin;

			result.mTuple[1] = coeff * mTuple[1];
			result.mTuple[2] = coeff * mTuple[2];
			result.mTuple[3] = coeff * mTuple[3];

			return result;
		}
	}

	result.mTuple[1] = mTuple[1];
	result.mTuple[2] = mTuple[2];
	result.mTuple[3] = mTuple[3];

	return result;
}

//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Quaternion<Real>::Rotate(const Vector3<Real>& rVector)
const
{
	// Given a vector u = (x0,y0,z0) and a unit length quaternion
	// q = <w,x,y,z>, the vector v = (x1,y1,z1) which represents the
	// rotation of u by q is v = q*u*q^{-1} where * indicates quaternion
	// multiplication and where u is treated as the quaternion <0,x0,y0,z0>.
	// Note that q^{-1} = <w,-x,-y,-z>, so no real work is required to
	// invert q.  Now
	//
	//   q*u*q^{-1} = q*<0,x0,y0,z0>*q^{-1}
	//     = q*(x0*i+y0*j+z0*k)*q^{-1}
	//     = x0*(q*i*q^{-1})+y0*(q*j*q^{-1})+z0*(q*k*q^{-1})
	//
	// As 3-vectors, q*i*q^{-1}, q*j*q^{-1}, and 2*k*q^{-1} are the columns
	// of the rotation matrix computed in Quaternion<Real>::ToRotationMatrix.
	// The vector v is obtained as the product of that rotation matrix with
	// vector u. As such, the quaternion representation of a rotation
	// matrix requires less space than the matrix and more time to compute
	// the rotated vector. Typical space-time tradeoff...

	Matrix3<Real> rot;
	ToRotationMatrix(rot);
	return rot * rVector;
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>& Quaternion<Real>::Slerp(Real t, const Quaternion& rP,
	const Quaternion& rQ)
{
	Real cos = rP.Dot(rQ);
	Real angle = Math<Real>::ACos(cos);
//	Real angle = Math<Real>::ACos(Math<Real>::FAbs(cos));

	if (Math<Real>::FAbs(angle) >= Math<Real>::ZERO_TOLERANCE)
	{
		Real sin = Math<Real>::Sin(angle);
		Real invSin = ((Real)1.0)/sin;
		Real tAngle = t * angle;
		Real coeff0 = Math<Real>::Sin(angle - tAngle) * invSin;
		Real coeff1 = Math<Real>::Sin(tAngle) * invSin;
// 		if (cos < 0)
// 		{
// 			coeff1 = -coeff1;
// 		}

		mTuple[0] = coeff0 * rP.mTuple[0] + coeff1 * rQ.mTuple[0];
		mTuple[1] = coeff0 * rP.mTuple[1] + coeff1 * rQ.mTuple[1];
		mTuple[2] = coeff0 * rP.mTuple[2] + coeff1 * rQ.mTuple[2];
		mTuple[3] = coeff0 * rP.mTuple[3] + coeff1 * rQ.mTuple[3];
	}
	else
	{
		mTuple[0] = rP.mTuple[0];
		mTuple[1] = rP.mTuple[1];
		mTuple[2] = rP.mTuple[2];
		mTuple[3] = rP.mTuple[3];
	}

	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>& Quaternion<Real>::Intermediate(const Quaternion& rQ0,
	const Quaternion& rQ1, const Quaternion& rQ2)
{
	// assert:  Q0, Q1, Q2 all unit-length
	Quaternion q1Inv = rQ1.Conjugate();
	Quaternion p0 = q1Inv*rQ0;
	Quaternion p2 = q1Inv*rQ2;
	Quaternion arg = -((Real)0.25)*(p0.Log() + p2.Log());
	Quaternion a = rQ1*arg.Exp();
	*this = a;
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
Quaternion<Real>& Quaternion<Real>::Squad(Real t, const Quaternion& rQ0,
	const Quaternion& rA0, const Quaternion& rA1, const Quaternion& rQ1)
{
	Real slerpT = ((Real)2)*t*((Real)1 - t);
	Quaternion slerpP = Slerp(t, rQ0, rQ1);
	Quaternion slerpQ = Slerp(t, rA0, rA1);
	return Slerp(slerpT, slerpP, slerpQ);
}

//----------------------------------------------------------------------------
template <class Real>
inline Quaternion<Real> operator* (Real scalar, const Quaternion<Real>& rQ)
{
	return rQ*scalar;
}
