// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline void PdrIndexBuffer::Enable(Renderer* pRenderer)
{
	PdrRendererData* pData = pRenderer->GetRendererData();
	pData->PdrIBuffer = this;
}

//----------------------------------------------------------------------------
inline void PdrIndexBuffer::Disable(Renderer* pRenderer)
{
	PdrRendererData* pData = pRenderer->GetRendererData();
	pData->PdrIBuffer = NULL;
}

//----------------------------------------------------------------------------
inline void* PdrIndexBuffer::Lock(Buffer::LockingMode)
{
	return mpBuffer;
}

//----------------------------------------------------------------------------
inline void PdrIndexBuffer::Unlock()
{
}

//----------------------------------------------------------------------------
inline UInt PdrIndexBuffer::GetBufferSize() const
{
	return mBufferSize;
}

//----------------------------------------------------------------------------
inline UShort* PdrIndexBuffer::GetBuffer()
{
	return mpBuffer;
}

//----------------------------------------------------------------------------
inline const UShort* PdrIndexBuffer::GetBuffer() const
{
	return mpBuffer;
}
