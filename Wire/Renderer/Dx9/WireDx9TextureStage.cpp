// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireDx9RendererData.h"
#include "WireImage2D.h"

using namespace Wire;

const DWORD PdrRendererData::StateTextureStage::TEX_BLEND[
	Material::BM_QUANTITY * 8] =
{
	// Material::BM_REPLACE
	1, D3DTOP_SELECTARG1, D3DTA_TEXTURE, 0,
	1, D3DTOP_SELECTARG1, D3DTA_TEXTURE, 0,

	// Material::BM_MODULATE
	2, D3DTOP_MODULATE, D3DTA_TEXTURE, 0,
	2, D3DTOP_MODULATE, D3DTA_TEXTURE, 0,

	// Material::BM_PASS
	1, D3DTOP_SELECTARG1, D3DTA_CURRENT, 0,
	1, D3DTOP_SELECTARG1, D3DTA_CURRENT, 0,

	// Material::BM_BLEND
	3, D3DTOP_LERP, D3DTA_TFACTOR, D3DTA_TEXTURE,
	2, D3DTOP_MODULATE, D3DTA_TEXTURE, 0,

	// Material::BM_DECAL
	2, D3DTOP_BLENDTEXTUREALPHA, D3DTA_TEXTURE, 0,
	1, D3DTOP_SELECTARG1, D3DTA_CURRENT, 0,

	// Material::BM_ADD
 	2, D3DTOP_ADD, D3DTA_TEXTURE, 0,
 	2, D3DTOP_ADD, D3DTA_TEXTURE, 0
};

//----------------------------------------------------------------------------
void Renderer::EnableTextureStage(Material::BlendMode blendMode, UInt unit,
	Bool hasAlpha)
{
	IDirect3DDevice9*& rDevice = GetRendererData()->D3DDevice;
	HRESULT hr;

	PdrRendererData::StateTextureStage& rState = mpData->TextureStageStates[
		unit];

	if (!rState.IsValid)
	{
		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLOROP, rState.
			COLOROP);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLORARG0, rState.
			COLORARG0);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLORARG1, rState.
			COLORARG1);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLORARG2, rState.
			COLORARG2);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAOP, rState.
			ALPHAOP);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAARG0, rState.
			ALPHAARG0);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAARG1, rState.
			ALPHAARG1);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAARG2, rState.
			ALPHAARG2);
		WIRE_ASSERT(SUCCEEDED(hr));

		rState.IsValid = true;
	}

	UInt idx = blendMode * 8;
	const DWORD* const pTexBlend = PdrRendererData::StateTextureStage::
		TEX_BLEND;

	const UInt argColorCount = pTexBlend[idx];
	DWORD colorOp = pTexBlend[idx+1];
	if (rState.COLOROP != colorOp)
	{
		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLOROP, colorOp);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.COLOROP = colorOp;
	}

	DWORD colorA1 = pTexBlend[idx+2];
	if (rState.COLORARG1 != colorA1)
	{
		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLORARG1, colorA1);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.COLORARG1 = colorA1;
	}

	if (argColorCount > 1)
	{
		DWORD op = (unit > 0) ? D3DTA_CURRENT : D3DTA_DIFFUSE;
		if (rState.COLORARG2 != op)
		{
			hr = rDevice->SetTextureStageState(unit, D3DTSS_COLORARG2, op);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.COLORARG2 = op;
		}

		if (argColorCount > 2)
		{
			DWORD colorA0 = pTexBlend[idx+3];
			if (rState.COLORARG0 != colorA0)
			{
				hr = rDevice->SetTextureStageState(unit, D3DTSS_COLORARG0,
					colorA0);
				WIRE_ASSERT(SUCCEEDED(hr));
				rState.COLORARG0 = colorA0;
			}
		}
	}

	if (!hasAlpha)
	{
		idx = Material::BM_PASS * 8;
	}

	const UInt argAlphaCount = pTexBlend[idx+4];
	DWORD alphaOp = pTexBlend[idx+5];
	if (rState.ALPHAOP != alphaOp)
	{
		hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAOP, alphaOp);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.ALPHAOP = alphaOp;
	}

	DWORD alphaA0 = pTexBlend[idx+6];
	if (rState.ALPHAARG1 != alphaA0)
	{
		hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAARG1, alphaA0);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.ALPHAARG1 = alphaA0;
	}

	if (argAlphaCount > 1)
	{
		DWORD op = (unit > 0) ? D3DTA_CURRENT : D3DTA_DIFFUSE;

		if (rState.ALPHAARG2 != op)
		{
			hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAARG2, op);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.ALPHAARG2 = op;
		}

		if (argAlphaCount > 2)
		{
			DWORD alphaA0 = (unit > 0) ? D3DTA_CURRENT : D3DTA_DIFFUSE;
			if (rState.ALPHAARG0 != alphaA0)
			{
				hr = rDevice->SetTextureStageState(unit, D3DTSS_ALPHAARG0,
					alphaA0);
				WIRE_ASSERT(SUCCEEDED(hr));
				rState.ALPHAARG0 = alphaA0;
			}
		}
	}
}

//----------------------------------------------------------------------------
void Renderer::DisableTextureStage(UInt unit)
{
	IDirect3DDevice9*& rDevice = GetRendererData()->D3DDevice;
	HRESULT hr;

	PdrRendererData::StateTextureStage& rState = GetRendererData()->
		TextureStageStates[unit];
	if (!rState.IsValid)
	{
		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLOROP, rState.
			COLOROP);
		WIRE_ASSERT(SUCCEEDED(hr));
	}

	if (rState.COLOROP != D3DTOP_DISABLE)
	{
		hr = rDevice->SetTextureStageState(unit, D3DTSS_COLOROP,
			D3DTOP_DISABLE);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.COLOROP = D3DTOP_DISABLE;
	}
}
