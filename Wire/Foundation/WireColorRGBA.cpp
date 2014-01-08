// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireColorRGBA.h"

using namespace Wire;

const ColorRGBA ColorRGBA::BLACK(0.0F, 0.0F, 0.0F, 1.0F);
const ColorRGBA ColorRGBA::WHITE(1.0F, 1.0F, 1.0F, 1.0F);
const ColorRGBA ColorRGBA::RED(1.0F, 0.0F, 0.0F, 1.0F);
const ColorRGBA ColorRGBA::GREEN(0.0F, 1.0F, 0.0F, 1.0F);
const ColorRGBA ColorRGBA::BLUE(0.0F, 0.0F, 1.0F, 1.0F);

//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA()
{
	mTuple[0] = 0.0F;
	mTuple[1] = 0.0F;
	mTuple[2] = 0.0F;
	mTuple[3] = 0.0F;
}

//----------------------------------------------------------------------------
ColorRGBA::ColorRGBA(Float red, Float green, Float blue, Float alpha)
{
	mTuple[0] = red;
	mTuple[1] = green;
	mTuple[2] = blue;
	mTuple[3] = alpha;
}

//----------------------------------------------------------------------------
void ColorRGBA::Saturate()
{
	for (UInt i = 0; i < 4; i++)
	{
		mTuple[i] = mTuple[i] > 1.0F ? 1.0F : mTuple[i];
	}
}
