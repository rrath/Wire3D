// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireGXIndexBuffer.h"

#include "WireIndexBuffer.h"
#include "WireGXDisplayList.h"
#include <malloc.h>

using namespace Wire;

//----------------------------------------------------------------------------
PdrIndexBuffer::PdrIndexBuffer(Renderer* pRenderer, const IndexBuffer*
	pIndexBuffer)
{
	CreateBuffer(NULL, pIndexBuffer->GetQuantity() * sizeof(UShort),
		pIndexBuffer->GetUsage());

	Update(pIndexBuffer);
}

//----------------------------------------------------------------------------
PdrIndexBuffer::PdrIndexBuffer(Renderer*, UInt size, Buffer::UsageType usage)
{
	CreateBuffer(NULL, size, usage);
}

//----------------------------------------------------------------------------
PdrIndexBuffer::~PdrIndexBuffer()
{
	free(mpBuffer);	// allocated using memalign, not using new
}

//----------------------------------------------------------------------------
void PdrIndexBuffer::CreateBuffer(Renderer* pRenderer, UInt size,
	Buffer::UsageType usage)
{
	mBufferSize = size;
	mpBuffer = reinterpret_cast<UShort*>(memalign(32, mBufferSize));
	WIRE_ASSERT(mpBuffer);
}

//----------------------------------------------------------------------------
void PdrIndexBuffer::Update(const IndexBuffer* pIndexBuffer)
{
	WIRE_ASSERT(pIndexBuffer);
	Update(pIndexBuffer, pIndexBuffer->GetQuantity(), 0);
}

//----------------------------------------------------------------------------
void PdrIndexBuffer::Update(const IndexBuffer* pIndexBuffer, UInt count,
	UInt offset)
{
	WIRE_ASSERT(pIndexBuffer);
	WIRE_ASSERT((offset + count) <= pIndexBuffer->GetQuantity());

	const UInt indexSize = sizeof(UShort);
	WIRE_ASSERT(mBufferSize == (indexSize * pIndexBuffer->GetQuantity()));
	const UInt rawOffset = offset * indexSize;
	size_t size = count * indexSize;
	WIRE_ASSERT(mBufferSize >= (size + rawOffset));

	void* pBuffer = Lock(Buffer::LM_WRITE_ONLY, size, rawOffset);
	System::Memcpy(pBuffer, size, (pIndexBuffer->GetData()+offset), size);
	Unlock();
}
