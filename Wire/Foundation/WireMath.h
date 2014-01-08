// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREMATH_H
#define WIREMATH_H

#include "WireSystem.h"

namespace Wire
{

template <class Real>
class Math
{
public:
	static Real ACos(Real value);
	static Real ASin(Real value);
	static Real ATan(Real value);
    static Real ATan2(Real y, Real x);
	static Real Ceil(Real value);
	static Real Cos(Real value);
	static Real FAbs(Real value);
	static Real Floor(Real value);
	static Real FMod(Real x, Real y);
	static Real InvSqrt(Real value);
	static Real Round(Real value);
	static Real Sin(Real value);
	static Real Sqrt(Real value);
	static Real Tan(Real value);

	static Real Min(Real value1, Real value2);
	static Real Max(Real value1, Real value2);

	static Real Clamp(Real min, Real value, Real max);

	// common constants
	static const Real MAX_REAL;
	static const Real PI;
	static const Real TWO_PI;
	static const Real HALF_PI;
	static const Real DEG_TO_RAD;
	static const Real RAD_TO_DEG;
	static const Real ZERO_TOLERANCE;
};

#include "WireMath.inl"

typedef Math<Float> MathF;
typedef Math<Double> MathD;

}

#endif
