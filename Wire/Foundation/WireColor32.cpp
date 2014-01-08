// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireColor32.h"
#include "WireSystem.h"

using namespace Wire;

const Color32 Color32::BLACK(0, 0, 0, 0xff);
const Color32 Color32::WHITE(0xff, 0xff, 0xff, 0xff);
const Color32 Color32::RED(0xff, 0, 0, 0xff);
const Color32 Color32::GREEN(0, 0xff, 0, 0xff);
const Color32 Color32::BLUE(0, 0, 0xff, 0xff);

//----------------------------------------------------------------------------
Color32::Color32()
{
	mTuple.m32 = 0;
}

//----------------------------------------------------------------------------
Color32::Color32(UChar red, UChar green, UChar blue, UChar alpha)
{
	mTuple.m8[WIRE_RGBA_R] = red;
	mTuple.m8[WIRE_RGBA_G] = green;
	mTuple.m8[WIRE_RGBA_B] = blue;
	mTuple.m8[WIRE_RGBA_A] = alpha;
}
