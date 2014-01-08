// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireDx9VertexFormat.h"

#include "WireRenderer.h"
#include "WireVertexBuffer.h"

using namespace Wire;

//----------------------------------------------------------------------------
PdrVertexFormat::PdrVertexFormat(Renderer* pRenderer, const TArray<
	VertexBufferPtr>& rVertexBuffers)
	:
	mpDeclaration(NULL)
{
	TArray<D3DVERTEXELEMENT9> elements(8);
	UInt colorsCount = 0;
	UInt uvSetCount = 0;

	for (UInt i = 0; i < rVertexBuffers.GetQuantity(); i++)
	{
		if (i >= pRenderer->GetMaxVertexStreams())
		{
			/* More vertex buffers supplied than streams supported */
			WIRE_ASSERT(false);
			break;
		}

		AddAttributes(i, rVertexBuffers[i]->GetAttributes(), elements,
			colorsCount, uvSetCount);
	}

	D3DVERTEXELEMENT9 sentinel = D3DDECL_END();
	elements.Append(sentinel);

	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	HRESULT hr;
	hr = rDevice->CreateVertexDeclaration(&elements[0], &mpDeclaration);
	WIRE_ASSERT(SUCCEEDED(hr));
	WIRE_ASSERT(mpDeclaration);
}

//----------------------------------------------------------------------------
PdrVertexFormat::~PdrVertexFormat()
{
	if (mpDeclaration)
	{
		mpDeclaration->Release();
	}
}

//----------------------------------------------------------------------------
void PdrVertexFormat::AddAttributes(UInt streamIndex, const VertexAttributes&
	rAttributes, TArray<D3DVERTEXELEMENT9> &elements, UInt& rColorsCount,
	UInt& rUvSetCount) 
{
	D3DVERTEXELEMENT9 element;
	element.Stream = static_cast<WORD>(streamIndex);
	element.Method = D3DDECLMETHOD_DEFAULT;

	UInt channels = 0;
	UInt vertexSize = 0;

	if (rAttributes.GetPositionChannels() > 0)
	{
		channels = rAttributes.GetPositionChannels();
		element.Offset = static_cast<WORD>(vertexSize);
		vertexSize += channels * sizeof(Float);
		element.Type = static_cast<BYTE>(D3DDECLTYPE_FLOAT1 + channels - 1);
		element.Usage = D3DDECLUSAGE_POSITION;
		element.UsageIndex = 0;
		elements.Append(element);
	}

	if (rAttributes.GetNormalChannels() > 0)
	{
		channels = rAttributes.GetNormalChannels();
		element.Offset = static_cast<WORD>(vertexSize);
		vertexSize += channels * sizeof(Float);
		element.Type = static_cast<BYTE>(D3DDECLTYPE_FLOAT1 + channels - 1);
		element.Usage = D3DDECLUSAGE_NORMAL;
		element.UsageIndex = 0;
		elements.Append(element);
	}

	for (UInt unit = 0; unit < rAttributes.GetColorChannelQuantity(); unit++)
	{
		channels = rAttributes.GetColorChannels(unit);
		if (channels > 0)
		{
			element.Offset = static_cast<WORD>(vertexSize);
			vertexSize += sizeof(DWORD);
			element.Type = D3DDECLTYPE_D3DCOLOR;
			element.Usage = D3DDECLUSAGE_COLOR;
			element.UsageIndex = static_cast<BYTE>(unit + rColorsCount);
			elements.Append(element);		
		}
	}

	rColorsCount += rAttributes.GetColorChannelQuantity();

	for (UInt unit = 0; unit < rAttributes.GetTCoordChannelQuantity(); unit++)
	{
		channels = rAttributes.GetTCoordChannels(unit);
		if (channels > 0)
		{
			element.Offset = static_cast<WORD>(vertexSize);
			vertexSize += channels * sizeof(Float);
			element.Type = static_cast<BYTE>(D3DDECLTYPE_FLOAT1 + channels-1);
			element.Usage = D3DDECLUSAGE_TEXCOORD;
			element.UsageIndex = static_cast<BYTE>(unit + rUvSetCount);
			elements.Append(element);
		}
	}

	rUvSetCount += rAttributes.GetTCoordChannelQuantity();

	WIRE_ASSERT(channels > 0);
	WIRE_ASSERT(vertexSize == rAttributes.GetVertexSize());
}
