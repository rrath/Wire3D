// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREQUATERNION_H
#define WIREQUATERNION_H

#include "WireTypes.h"
#include "WireMatrix3.h"

namespace Wire
{

template <class Real>
class Quaternion
{
public:
	// A quaternion is q = w + x*i + y*j + z*k where (w,x,y,z) is not
	// necessarily a unit length vector in 4D.

	// construction
	Quaternion();  // uninitialized
	Quaternion(Real w, Real x, Real y, Real z);
	Quaternion(const Quaternion& rQ);

 	// quaternion for the input rotation matrix
 	Quaternion(const Matrix3<Real>& rRot);
 
 	// quaternion for the rotation of the axis-angle pair
 	Quaternion(const Vector3<Real>& rAxis, Real angle);
 
 	// quaternion for the rotation matrix with specified columns
 	Quaternion(const Vector3<Real> rotColumns[3]);

	// member access:  0 = w, 1 = x, 2 = y, 3 = z
	inline operator const Real* () const;
	inline operator Real* ();
	inline Real operator[] (UInt i) const;
	inline Real& operator[] (UInt i);
	inline Real W() const;
	inline Real& W();
	inline Real X() const;
	inline Real& X();
	inline Real Y() const;
	inline Real& Y();
	inline Real Z() const;
	inline Real& Z();

	// assignment
	inline Quaternion& operator= (const Quaternion& rQ);

	// comparison
	inline Bool operator== (const Quaternion& rQ) const;
	inline Bool operator!= (const Quaternion& rQ) const;

	// arithmetic operations
	inline Quaternion operator+ (const Quaternion& rQ) const;
	inline Quaternion operator- (const Quaternion& rQ) const;
	inline Quaternion operator* (const Quaternion& rQ) const;
	inline Quaternion operator* (Real scalar) const;
	inline Quaternion operator/ (Real scalar) const;
	inline Quaternion operator- () const;

	// arithmetic updates
	inline Quaternion& operator+= (const Quaternion& rQ);
	inline Quaternion& operator-= (const Quaternion& rQ);
	inline Quaternion& operator*= (Real scalar);
	inline Quaternion& operator/= (Real scalar);

	// conversion between quaternions, matrices, and axis-angle
	Quaternion& FromRotationMatrix(const Matrix3<Real>& rRot);
	void ToRotationMatrix(Matrix3<Real>& rRot) const;
	Quaternion& FromAxisAngle(const Vector3<Real>& rAxis, Real angle);
	void ToAxisAngle(Vector3<Real>& rAxis, Real& rAngle) const;

	// functions of a quaternion
	inline Real Length() const; 
	inline Real SquaredLength() const;
	inline Real Dot(const Quaternion& rQ) const;
	inline Real Normalize();
	Quaternion Inverse() const;  // apply to non-zero quaternion
	Quaternion Conjugate() const;
	Quaternion Exp() const;  // apply to quaternion with w = 0
	Quaternion Log() const;  // apply to unit-length quaternion

	// rotation of a vector by a quaternion
	Vector3<Real> Rotate(const Vector3<Real>& rVector) const;

	// spherical linear interpolation
	Quaternion& Slerp(Real t, const Quaternion& rP, const Quaternion& rQ);

	// Intermediate terms for spherical quadratic interpolation.
	Quaternion& Intermediate(const Quaternion& rQ0, const Quaternion& rQ1,
		const Quaternion& rQ2);

	// Spherical quadratic interpolation.
	Quaternion& Squad(Real t, const Quaternion& rQ0, const Quaternion& rA0,
		const Quaternion& rA1, const Quaternion& rQ1);

	static const Quaternion IDENTITY;
	static const Quaternion ZERO;

private:
	Real mTuple[4];
};

template <class Real>
inline Quaternion<Real> operator* (Real scalar, const Quaternion<Real>& rQ);

#include "WireQuaternion.inl"

typedef Quaternion<Float> QuaternionF;
typedef Quaternion<Double> QuaternionD;

}

#endif
