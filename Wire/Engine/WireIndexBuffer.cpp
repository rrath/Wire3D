// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireIndexBuffer.h"

#include "WireRenderer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, IndexBuffer, Buffer);

//----------------------------------------------------------------------------
IndexBuffer::IndexBuffer(UInt quantity, UsageType usage)
	:
	Buffer(usage),
	mQuantity(quantity)
{
	mpIndices = WIRE_NEW UShort[mQuantity];
}

//----------------------------------------------------------------------------
IndexBuffer::IndexBuffer(UShort* pIndices, UInt quantity, UsageType usage)
	:
	Buffer(usage),
	mQuantity(quantity),
	mpIndices(pIndices)
{
}

//----------------------------------------------------------------------------
IndexBuffer::IndexBuffer(const IndexBuffer* pIndexBuffer)
	:
	Buffer(pIndexBuffer->GetUsage()),
	mQuantity(pIndexBuffer->mQuantity)
{
	mpIndices = WIRE_NEW UShort[mQuantity];
	size_t size = mQuantity*sizeof(UShort);
	System::Memcpy(mpIndices, size, pIndexBuffer->mpIndices, size);
}

//----------------------------------------------------------------------------
IndexBuffer::~IndexBuffer()
{
	// Inform all renderers using this index buffer that it is being
	// destroyed. This allows the renderer to free up any associated
	// resources.
	Renderer::UnbindAll(this);

	WIRE_DELETE[] mpIndices;
}

//----------------------------------------------------------------------------
void IndexBuffer::Copy(UShort* pDst, UShort offset, UInt activeIndexCount,
	UInt startIndex)
{
	WIRE_ASSERT(startIndex+activeIndexCount <= mQuantity);

	if (offset == 0)
	{
		if (pDst != GetData())
		{
			size_t size = activeIndexCount * sizeof(UShort);
			System::Memcpy(pDst, size, GetData()+startIndex, size);
		}

		return;
	}

	UShort* pSrc = GetData() + startIndex;
	if ((activeIndexCount % 3) == 0)
	{
		for (UInt i = 0; i < activeIndexCount; i+=3)
		{
			*pDst++ = *pSrc++ + offset;
			*pDst++ = *pSrc++ + offset;
			*pDst++ = *pSrc++ + offset;
		}
	}
	else
	{
		for (UInt i = 0; i < activeIndexCount; i++)
		{
			*pDst++ = *pSrc++ + offset;
		}
	}
}
