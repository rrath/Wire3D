// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireGXVertexFormat.h"

#include "WireGXRendererData.h"
#include "WireVertexBuffer.h"


using namespace Wire;

//----------------------------------------------------------------------------
PdrVertexFormat::PdrVertexFormat(Renderer* pRenderer, const TArray<
	VertexBufferPtr>& rVertexBuffers)
{
	UInt colorsCount = 0;
	UInt uvSetCount = 0;

	for (UInt i = 0; i < rVertexBuffers.GetQuantity(); i++)
	{
		if (i >= pRenderer->GetMaxVertexStreams())
		{
			/* More vertex buffers defined than streams supported*/
			WIRE_ASSERT(false);
			break;
		}

		AddAttributes(i, rVertexBuffers[i]->GetAttributes(), colorsCount,
			uvSetCount);
	}
}

//----------------------------------------------------------------------------
void PdrVertexFormat::Enable(Renderer* pRenderer)
{
	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GXClearVtxDesc();

	for (UInt i = 0; i < mDeclaration.GetQuantity(); i++)
	{
		GXSetVtxDesc(mDeclaration[i].Attr, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, mDeclaration[i].Attr, mDeclaration[i].
			CompCnt, mDeclaration[i].CompType, 0);
	}

	WIRE_ASSERT(pRenderer);
	pRenderer->GetRendererData()->PdrVFormat = this;
}

//----------------------------------------------------------------------------
void PdrVertexFormat::Disable(Renderer* pRenderer)
{
	WIRE_ASSERT(pRenderer);
	pRenderer->GetRendererData()->PdrVFormat = NULL;
}

//----------------------------------------------------------------------------
void PdrVertexFormat::AddAttributes(UInt streamIndex, const VertexAttributes&
	rAttributes, UInt& rColorsCount, UInt& rUvSetCount)
{
	VertexElement element;
	element.StreamIndex = streamIndex;
	UInt vertexSize = 0;

	UInt channels = rAttributes.GetPositionChannels();
	if (channels > 0)
	{
		WIRE_ASSERT(channels == 3);
		element.Offset = vertexSize;
		vertexSize += channels * sizeof(Float);
		element.Attr = GX_VA_POS;
		element.CompCnt = GX_POS_XYZ;
		element.CompType = GX_F32;
		mDeclaration.Append(element);
	}

	channels = rAttributes.GetNormalChannels();
	if (channels > 0)
	{
		WIRE_ASSERT(channels == 3);
		element.Offset = vertexSize;
		vertexSize += channels * sizeof(Float);
		element.Attr = GX_VA_NRM;
		element.CompCnt = GX_NRM_XYZ;
		element.CompType = GX_F32;
		mDeclaration.Append(element);
	}

	for (UInt unit = 0; unit < rAttributes.GetColorChannelQuantity(); unit++)
	{
		channels = rAttributes.GetColorChannels(unit);
		if (channels > 0)
		{
			element.Offset = vertexSize;
			vertexSize += sizeof(UInt);
			element.Attr = GX_VA_CLR0 + unit + rColorsCount;
			element.CompCnt = GX_CLR_RGBA;
			element.CompType = GX_RGBA8;
			mDeclaration.Append(element);
		}
	}

	rColorsCount += rAttributes.GetColorChannelQuantity();

	for (UInt unit = 0; unit < rAttributes.GetTCoordChannelQuantity(); unit++)
	{
		channels = rAttributes.GetTCoordChannels(unit);
		if (channels > 0)
		{
			WIRE_ASSERT(channels == 2);
			element.Offset = vertexSize;
			vertexSize += channels * sizeof(Float);
			element.Attr = GX_VA_TEX0 + unit + rUvSetCount;
			element.CompCnt = GX_TEX_ST;
			element.CompType = GX_F32;
			mDeclaration.Append(element);
		}
	}

	rUvSetCount += rAttributes.GetTCoordChannelQuantity();

	WIRE_ASSERT(vertexSize == rAttributes.GetVertexSize());
}
