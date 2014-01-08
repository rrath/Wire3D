// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireDx9IndexBuffer.h"

#include "WireIndexBuffer.h"

using namespace Wire;

//----------------------------------------------------------------------------
PdrIndexBuffer::PdrIndexBuffer(Renderer* pRenderer, const IndexBuffer*
	pIndexBuffer)
{
	UInt size = pIndexBuffer->GetQuantity() * sizeof(UShort);
	CreateBuffer(pRenderer, size, pIndexBuffer->GetUsage());

	Update(pIndexBuffer);
}

//----------------------------------------------------------------------------
PdrIndexBuffer::PdrIndexBuffer(Renderer* pRenderer, UInt size,
	Buffer::UsageType usage)
{
	CreateBuffer(pRenderer, size, usage);
}

//----------------------------------------------------------------------------
PdrIndexBuffer::~PdrIndexBuffer()
{
	mpBuffer->Release();
}

//----------------------------------------------------------------------------
void PdrIndexBuffer::Update(const IndexBuffer* pIndexBuffer)
{
	Update(pIndexBuffer, pIndexBuffer->GetQuantity());
}

//----------------------------------------------------------------------------
void PdrIndexBuffer::Update(const IndexBuffer* pIndexBuffer, UInt count,
	UInt offset)
{
	WIRE_ASSERT(offset+count <= pIndexBuffer->GetQuantity());
	WIRE_ASSERT((mBufferSize/sizeof(UShort)) == pIndexBuffer->GetQuantity());

	Buffer::LockingMode lockingMode = pIndexBuffer->GetUsage() ==
		Buffer::UT_STATIC ? Buffer::LM_READ_WRITE : Buffer::LM_WRITE_ONLY;
	UShort* pBuffer = reinterpret_cast<UShort*>(Lock(lockingMode)) + offset;

	size_t size = count * sizeof(UShort);
	const UShort* pDst = pIndexBuffer->GetData() + offset;
	System::Memcpy(pBuffer, size, pDst, size);

	Unlock();
}

//----------------------------------------------------------------------------
void PdrIndexBuffer::CreateBuffer(Renderer* pRenderer, UInt size, 
	Buffer::UsageType usage)
{
	mBufferSize = size;
	const DWORD d3dUsage = PdrRendererData::USAGES[usage];
	const D3DPOOL pool = PdrRendererData::POOLS[usage];
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	HRESULT hr;
	hr = rDevice->CreateIndexBuffer(mBufferSize, d3dUsage, D3DFMT_INDEX16,
		pool, &mpBuffer, NULL);
	WIRE_ASSERT(SUCCEEDED(hr));
}
