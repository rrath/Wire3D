// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRandom.h"

#include "WireSystem.h"

using namespace Wire;

//----------------------------------------------------------------------------
Random::Random(UInt seed)
{
	Init(seed);
}

//----------------------------------------------------------------------------
UInt Random::Get()
{
	ULongLong c = 2111111111*mX[3] + 1492 * mX[2] + 1776 * mX[1] + 
		5115 * mX[0] + mX[4];

	mX[4] = (c>>32) & 0xFFFFFFFF;
	mX[3] = mX[2];
	mX[2] = mX[1];
	mX[1] = mX[0];
	mX[0] = c & 0xFFFFFFFF;

	return static_cast<UInt>(mX[0]);
}

//----------------------------------------------------------------------------
Float Random::GetFloat()
{
	return static_cast<Float>(Get()) / System::MAX_UINT;
}

//----------------------------------------------------------------------------
Double Random::GetDouble()
{
	return static_cast<Double>(Get()) / System::MAX_UINT;
}

//----------------------------------------------------------------------------
void Random::Init(UInt seed)
{
	UInt s = seed;

	// make random numbers and put them into the buffer
	for (UInt i = 0; i < 5; i++)
	{
		s = s * 29943829 - 1;
		mX[i] = s;
	}

	// randomize some more
	for (UInt i = 0; i < 19; i++)
	{
		Get();
	}
}
