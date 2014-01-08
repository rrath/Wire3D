// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENOISEPERLIN_H
#define WIRENOISEPERLIN_H

#include "WireMain.h"
#include "WireMath.h"

namespace Wire
{

// The base class contains the permutation table for all template instances.
class NoisePerlinBase
{
	WIRE_DECLARE_INITIALIZE;

protected:
	static const Int s_Permutation[];
	static Int s_P[];
};

WIRE_REGISTER_INITIALIZE(NoisePerlinBase);

//----------------------------------------------------------------------------
template <class Real>
class NoisePerlin : public NoisePerlinBase
{
public:
	Real Noise(Real x, Real y);
	Real Noise(Real x, Real y, Real z);

private:
	Real Fade(Real t);
	Real Grad(Int hash, Real x, Real y, Real z);
	inline Real Lerp(Real t, Real a, Real b);
};

#include "WireNoisePerlin.inl"

typedef NoisePerlin<Float> NoisePerlinF;
typedef NoisePerlin<Double> NoisePerlinD;

}

#endif
