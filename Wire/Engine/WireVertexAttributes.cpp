// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireVertexAttributes.h"

using namespace Wire;

//----------------------------------------------------------------------------
VertexAttributes::VertexAttributes()
	:
	mChannelQuantity(0),
	mPositionChannels(0),
	mNormalChannels(0),
	mKey(0)
{
	ResetOffsets();
}

//----------------------------------------------------------------------------
VertexAttributes::~VertexAttributes()
{
}

//----------------------------------------------------------------------------
void VertexAttributes::ResetOffsets()
{
	mPositionOffset = -1;
	mNormalOffset = -1;
}

//----------------------------------------------------------------------------
void VertexAttributes::SetPositionChannels(UInt positionChannels)
{
	WIRE_ASSERT(positionChannels <= 3);

	mPositionChannels = positionChannels;
	UpdateOffsets();
}

//----------------------------------------------------------------------------
void VertexAttributes::SetColorChannels(UInt colorChannels, UInt unit)
{
	WIRE_ASSERT(colorChannels <= 4);

	if (mColorChannels.GetQuantity() <= unit)
	{
		mColorChannels.SetElement(unit, colorChannels);
	}
	else
	{
		mColorChannels[unit] = colorChannels;
	}

	UpdateOffsets();
}

//----------------------------------------------------------------------------
void VertexAttributes::SetNormalChannels(UInt normalChannels)
{
	WIRE_ASSERT(normalChannels <= 4);

	mNormalChannels = normalChannels;
	UpdateOffsets();
}

//----------------------------------------------------------------------------
void VertexAttributes::SetTCoordChannels(UInt tCoordChannels, UInt unit)
{
	WIRE_ASSERT(tCoordChannels <= 4);

	if (mTCoordChannels.GetQuantity() <= unit)
	{
		mTCoordChannels.SetElement(unit, tCoordChannels);
	}
	else
	{
		mTCoordChannels[unit] = tCoordChannels;
	}

	UpdateOffsets();
}

//----------------------------------------------------------------------------
void VertexAttributes::UpdateOffsets()
{
	mChannelQuantity = 0;
	mKey = 0;

	ResetOffsets();	

	if (mPositionChannels > 0)
	{
		mPositionOffset = mChannelQuantity;
		mChannelQuantity += mPositionChannels;
		mKey |= 1;
	}

	if (mNormalChannels > 0)
	{
		mNormalOffset = mChannelQuantity;
		mChannelQuantity += mNormalChannels;
		mKey |= 2;
	}

	if (mColorChannels.GetQuantity() > 0)
	{
		WIRE_ASSERT(mColorChannels.GetQuantity() <= 3); /* 2 bits for key */
		mKey |= mColorChannels.GetQuantity() << 2;

		for (UInt i = 0; i < mColorChannels.GetQuantity(); i++)
		{
			if (mColorChannels[i] > 0)
			{
				mColorOffset.SetElement(i, mChannelQuantity);

				// ignoring float channels, use 1 channel of 32 bits instead
				WIRE_ASSERT(sizeof(UInt) == sizeof(Float));
				mChannelQuantity += 1;
			}
		}
	}

	if (mTCoordChannels.GetQuantity() > 0)
	{
		mKey |= mTCoordChannels.GetQuantity() << 4;
		for (UInt i = 0; i < mTCoordChannels.GetQuantity(); i++)
		{
			if (mTCoordChannels[i] > 0)
			{
				mTCoordOffset.SetElement(i, mChannelQuantity);
				mChannelQuantity += mTCoordChannels[i];
			}
		}
	}
}
