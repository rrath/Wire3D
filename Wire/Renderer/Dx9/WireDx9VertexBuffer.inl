// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline void PdrVertexBuffer::Enable(Renderer* pRenderer, UInt vertexSize,
	UInt streamIndex)
{
	WIRE_ASSERT(vertexSize > 0);
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	HRESULT hr;
	hr = rDevice->SetStreamSource(streamIndex, mpBuffer, 0, vertexSize);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
inline void PdrVertexBuffer::Disable(Renderer* pRenderer, UInt streamIndex)
{
	HRESULT hr;
	hr = pRenderer->GetRendererData()->D3DDevice->SetStreamSource(streamIndex,
		NULL, 0, 0);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void* PdrVertexBuffer::Lock(Buffer::LockingMode mode)
{
	void* pBuffer = NULL;
	HRESULT hr;
	hr = mpBuffer->Lock(0, 0, &pBuffer,
		PdrRendererData::BUFFER_LOCKING[mode]);
	WIRE_ASSERT(SUCCEEDED(hr));
	return pBuffer;
}

//----------------------------------------------------------------------------
inline void PdrVertexBuffer::Unlock()
{
	HRESULT hr;
	hr = mpBuffer->Unlock();
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
inline UInt PdrVertexBuffer::GetBufferSize() const
{
	return mBufferSize;
}
