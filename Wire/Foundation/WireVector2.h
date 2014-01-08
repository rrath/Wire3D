// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREVECTOR2_H
#define WIREVECTOR2_H

#include "WireTypes.h"
#include "WireMath.h"

namespace Wire
{

template <class Real>
class Vector2
{
public:
	// construction
	Vector2();  // uninitialized
	Vector2(Real x, Real y);

	// coordinate access
	inline operator Real* ();
	inline operator const Real* () const;
	inline Real X() const;
	inline Real& X();
	inline Real Y() const;
	inline Real& Y();

	// assignment
	inline Vector2& operator= (const Vector2& rVector);

	// comparison
	inline Bool operator== (const Vector2& rVector) const;
	inline Bool operator!= (const Vector2& rVector) const;

	// arithmetic operations
	inline Vector2 operator+ (const Vector2& rVector) const;
	inline Vector2 operator- (const Vector2& rVector) const;
	inline Vector2 operator* (Real scalar) const;
	inline Vector2 operator/ (Real scalar) const;
	inline Vector2 operator- () const;

	// arithmetic updates
	inline Vector2& operator+= (const Vector2& rVector);
	inline Vector2& operator-= (const Vector2& rVector);
	inline Vector2& operator*= (Real scalar);
	inline Vector2& operator/= (Real scalar);

	// vector operations
	inline Real Length() const;
	inline Real SquaredLength() const;
	inline Real Dot(const Vector2& rVector) const;
	inline Real Normalize();

	// Gram-Schmidt orthonormalization. Take linearly independent vectors U
	// and V and compute an orthonormal set (unit length, mutually
	// perpendicular).
	static void Orthonormalize(Vector2& rU, Vector2& rV);

	static const Vector2 ZERO;    // (0,0)
	static const Vector2 UNIT_X;  // (1,0)
	static const Vector2 UNIT_Y;  // (0,1)
	static const Vector2 ONE;     // (1,1)

private:
	Real mTuple[2];
};

// arithmetic operations
template <class Real>
Vector2<Real> operator* (Real scalar, const Vector2<Real>& rVector);

#include "WireVector2.inl"

typedef Vector2<Float> Vector2F;
typedef Vector2<Double> Vector2D;

}

#endif
