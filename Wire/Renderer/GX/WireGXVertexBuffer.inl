// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline void PdrVertexBuffer::Disable(Renderer*, UInt)
{
}

//----------------------------------------------------------------------------
inline void* PdrVertexBuffer::Lock(Buffer::LockingMode, UInt,
	UInt offsetToLock)
{
	return reinterpret_cast<UChar*>(mpBuffer) + offsetToLock;
}

//----------------------------------------------------------------------------
inline void PdrVertexBuffer::Unlock()
{
	// TODO: only apply to locked size and offset
	DCStoreRange(mpBuffer, mSize);
}

//----------------------------------------------------------------------------
inline UInt PdrVertexBuffer::GetSize() const
{
	return mSize;
}
