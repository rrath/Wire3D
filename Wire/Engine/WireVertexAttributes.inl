// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetChannelQuantity() const
{
	return mChannelQuantity;
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetColorChannelQuantity() const
{
	return mColorChannels.GetQuantity();
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetTCoordChannelQuantity() const
{
	return mTCoordChannels.GetQuantity();
}

//----------------------------------------------------------------------------
inline Bool VertexAttributes::HasPosition() const
{
	return mPositionChannels > 0;
}

//----------------------------------------------------------------------------
inline Bool VertexAttributes::HasColor(UInt unit) const
{
	if (unit >= mColorChannels.GetQuantity())
	{
		return false;
	}

	return mColorChannels[unit] > 0;
}

//----------------------------------------------------------------------------
inline Bool VertexAttributes::HasNormal() const
{
	return mNormalChannels > 0;
}

//----------------------------------------------------------------------------
inline Bool VertexAttributes::HasTCoord(UInt unit) const
{
	if (unit >= mTCoordChannels.GetQuantity())
	{
		return false;
	}

	return mTCoordChannels[unit] > 0;
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetPositionChannels() const
{
	return mPositionChannels;
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetColorChannels(UInt unit) const
{
	WIRE_ASSERT(unit < mColorChannels.GetQuantity());
	return mColorChannels[unit];
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetNormalChannels() const
{
	return mNormalChannels;
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetTCoordChannels(UInt unit) const
{
	WIRE_ASSERT(unit < mTCoordChannels.GetQuantity());
	return mTCoordChannels[unit];
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetPositionOffset() const
{
	return mPositionOffset;
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetColorOffset(UInt unit) const
{
	WIRE_ASSERT(unit < mColorOffset.GetQuantity());
	return mColorOffset[unit];
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetNormalOffset() const
{
	return mNormalOffset;
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetTCoordOffset(UInt unit) const
{
	WIRE_ASSERT(unit < mTCoordOffset.GetQuantity());
	return mTCoordOffset[unit];
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetVertexSize() const
{
	return mChannelQuantity * sizeof(Float);
}

//----------------------------------------------------------------------------
inline UInt VertexAttributes::GetKey() const
{
	return mKey;
}
