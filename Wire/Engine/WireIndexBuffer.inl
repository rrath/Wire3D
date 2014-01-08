// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt IndexBuffer::GetQuantity() const
{
	return mQuantity;
}

//----------------------------------------------------------------------------
inline UShort* IndexBuffer::GetData()
{
	return mpIndices;
}

//----------------------------------------------------------------------------
inline const UShort* IndexBuffer::GetData() const
{
	return mpIndices;
}

//----------------------------------------------------------------------------
inline UShort IndexBuffer::operator[] (UInt i) const
{
	WIRE_ASSERT(i < mQuantity);
	return mpIndices[i];
}
//----------------------------------------------------------------------------
inline UShort& IndexBuffer::operator[] (UInt i)
{
	WIRE_ASSERT(i < mQuantity);
	return mpIndices[i];
}
