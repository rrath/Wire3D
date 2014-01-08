// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireDx9VertexBuffer.h"

#include "WireDx9VertexFormat.h"
#include "WireVertexBuffer.h"

using namespace Wire;

//----------------------------------------------------------------------------
PdrVertexBuffer::PdrVertexBuffer(Renderer* pRenderer, const VertexBuffer*
	pVertexBuffer)
{
	WIRE_ASSERT(pVertexBuffer);
	const UInt vertexSize = pVertexBuffer->GetAttributes().GetVertexSize();
	CreateBuffer(pRenderer, vertexSize * pVertexBuffer->GetQuantity(),
		pVertexBuffer->GetUsage());

	// Copy the vertex buffer data from system memory to video memory.
	Update(pVertexBuffer);
}

//----------------------------------------------------------------------------
PdrVertexBuffer::PdrVertexBuffer(Renderer* pRenderer, UInt size,
	Buffer::UsageType usage)
{
	CreateBuffer(pRenderer, size, usage);
}

//----------------------------------------------------------------------------
PdrVertexBuffer::~PdrVertexBuffer()
{
	mpBuffer->Release();
}

//----------------------------------------------------------------------------
void PdrVertexBuffer::CreateBuffer(Renderer* pRenderer, UInt size,
	Buffer::UsageType usage)
{
	mBufferSize = size;
	const DWORD d3dUsage = PdrRendererData::USAGES[usage];
	const D3DPOOL pool = PdrRendererData::POOLS[usage];
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	HRESULT hr;
	hr = rDevice->CreateVertexBuffer(mBufferSize, d3dUsage, 0, pool,
		&mpBuffer, NULL);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void PdrVertexBuffer::Update(const VertexBuffer* pVertexBuffer)
{
	Update(pVertexBuffer, pVertexBuffer->GetQuantity());
}

//----------------------------------------------------------------------------
void PdrVertexBuffer::Update(const VertexBuffer* pVertexBuffer, UInt count,
	UInt offset)
{
	WIRE_ASSERT(count <= pVertexBuffer->GetQuantity());

	const UInt vertexSize = pVertexBuffer->GetAttributes().GetVertexSize();
	WIRE_ASSERT(vertexSize > 0);
	WIRE_ASSERT(mBufferSize == (vertexSize * pVertexBuffer->GetQuantity()));

	Buffer::LockingMode lockingMode = pVertexBuffer->GetUsage() ==
		Buffer::UT_STATIC ? Buffer::LM_READ_WRITE : Buffer::LM_WRITE_ONLY;
	const UInt rawOffset = offset * vertexSize;
	UChar* pBuffer = reinterpret_cast<UChar*>(Lock(lockingMode)) + rawOffset;

	size_t size = count * vertexSize;
	const UChar* pDst = reinterpret_cast<const UChar*>(pVertexBuffer->
		GetData()) + rawOffset;
	System::Memcpy(pBuffer, size, pDst, size);

	Unlock();
}
