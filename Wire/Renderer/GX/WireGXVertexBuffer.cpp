// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireGXVertexBuffer.h"

#include "WireGXRendererData.h"
#include "WireRenderer.h"
#include "WireVertexBuffer.h"
#include <malloc.h>

using namespace Wire;

//----------------------------------------------------------------------------
PdrVertexBuffer::PdrVertexBuffer(Renderer*, const VertexBuffer* pVertexBuffer)
{
	WIRE_ASSERT(pVertexBuffer);
	const UInt vertexSize = pVertexBuffer->GetAttributes().GetVertexSize();
	CreateBuffer(NULL, vertexSize * pVertexBuffer->GetQuantity(),
		pVertexBuffer->GetUsage());

	Update(pVertexBuffer);
}

//----------------------------------------------------------------------------
PdrVertexBuffer::PdrVertexBuffer(Renderer*, UInt size, Buffer::UsageType
	usage)
{
	CreateBuffer(NULL, size, usage);
}

//----------------------------------------------------------------------------
PdrVertexBuffer::~PdrVertexBuffer()
{
	free(mpBuffer);	// allocated using memalign, not using new
}

//----------------------------------------------------------------------------
void PdrVertexBuffer::CreateBuffer(Renderer*, UInt size,
	Buffer::UsageType usage)
{
	mBufferSize = size;
	mpBuffer = memalign(32, mBufferSize);
	WIRE_ASSERT(mpBuffer);
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
	WIRE_ASSERT(pVertexBuffer);
	WIRE_ASSERT((offset + count) <= pVertexBuffer->GetQuantity());

	const UInt vertexSize = pVertexBuffer->GetAttributes().GetVertexSize();
	WIRE_ASSERT(vertexSize > 0);
	WIRE_ASSERT(mBufferSize == (vertexSize * pVertexBuffer->GetQuantity()));
	const UInt rawOffset = offset * vertexSize;

	UChar* pBuffer = reinterpret_cast<UChar*>(Lock(Buffer::LM_WRITE_ONLY)) +
		rawOffset;
	size_t size = count * vertexSize;
	const UChar* pDst = reinterpret_cast<const UChar*>(pVertexBuffer->
		GetData()) + rawOffset;
	System::Memcpy(pBuffer, size, pDst, size);
	Unlock();
}

//----------------------------------------------------------------------------
void PdrVertexBuffer::Enable(Renderer* pRenderer, UInt vertexSize,
	UInt streamIndex)
{
	WIRE_ASSERT(vertexSize > 0);
	WIRE_ASSERT(pRenderer);
	// the vertex format needs to be set by the Renderer beforehand
	WIRE_ASSERT(pRenderer->GetRendererData()->PdrVFormat);

	const TArray<PdrVertexFormat::VertexElement>& rDeclaration =
		pRenderer->GetRendererData()->PdrVFormat->GetDeclaration();

	for (UInt i = 0; i < rDeclaration.GetQuantity(); i++)
	{
		if (rDeclaration[i].StreamIndex == streamIndex)
		{
			void* pArray = reinterpret_cast<void*>((rDeclaration[i].Offset +
				reinterpret_cast<UChar*>(mpBuffer)));
			GXSetArray(rDeclaration[i].Attr, pArray, vertexSize);
		}
	}
}
