// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREDX9RENDERERDATA_H
#define WIREDX9RENDERERDATA_H

#include "WireMatrix4.h"
#include "WireTArray.h"
#include "WireTHashTable.h"
#include <d3d9.h>
#include <d3dx9.h> // only used for D3DXCompileShader

namespace Wire
{

class Renderer;
class RenderText;

class PdrRendererData
{
public:
	PdrRendererData(Renderer* pRenderer);

	// Platform dependent renderer statistics
	inline void AppendStatistics(RenderText*) {}
	inline void ResetStatistics() {}

	// Platform-dependent data
	IDirect3D9* D3D;
	IDirect3DDevice9* D3DDevice;
	D3DPRESENT_PARAMETERS Present;

	Matrix4F View;
	Matrix4F Projection;

	Matrix4F ViewProjection;
	Matrix4F ModelView;
	Matrix4F ModelViewProjection;

	struct StateAlpha
	{
		StateAlpha()
			:
			SRCBLEND(D3DBLEND_ZERO),
			DESTBLEND(D3DBLEND_ZERO),
			BLENDFACTOR(0),
			ALPHABLENDENABLE(FALSE),
			IsValid(false) {}

		static const DWORD ALPHA_SRC_BLEND[];
		static const DWORD ALPHA_DST_BLEND[];	
		DWORD SRCBLEND;
		DWORD DESTBLEND;
		DWORD BLENDFACTOR;
		DWORD ALPHABLENDENABLE;
		Bool IsValid;
	} AlphaState;

	struct StateCull
	{
		StateCull()
			:
			CULLMODE(D3DCULL_CW),
			IsValid(false) {}

		static const DWORD CULL_TYPE[];
		DWORD CULLMODE;
		Bool IsValid;
	} CullState;

	struct StateFog
	{
		StateFog()
			:
			FOGENABLE(FALSE),
			FOGSTART(0),
			FOGEND(0x3f800000),
			FOGCOLOR(0xff000000),
			FOGDENSITY(0x3f800000),
			FOGVERTEXMODE(D3DFOG_LINEAR),
			IsValid(false) {}

		static const DWORD FOG_DENSITY[];
		DWORD FOGENABLE;
		DWORD FOGSTART;
		DWORD FOGEND;
		DWORD FOGCOLOR;
		DWORD FOGDENSITY;
		DWORD FOGVERTEXMODE;
		Bool IsValid;
	} FogState;

	struct StateLight
	{
		StateLight()
			:
			AMBIENT(0xff000000),
			LIGHTING(FALSE),
			IsValid(false) {}

		DWORD AMBIENT;
		DWORD LIGHTING;
		Bool IsValid;
	} LightState;

	struct StateSampler
	{
		StateSampler()
			:
			MAXANISOTROPY(0x00000001),
			MAGFILTER(D3DTEXF_LINEAR),
			MINFILTER(D3DTEXF_LINEAR),
			MIPFILTER(D3DTEXF_LINEAR),
			BORDERCOLOR(0xff000000),
			ADDRESSU(D3DTADDRESS_CLAMP),
			ADDRESSV(D3DTADDRESS_CLAMP),
			IsValid(false) {}

		static const DWORD TEX_MIN_FILTER[];
		static const DWORD TEX_MIP_FILTER[];
		static const DWORD TEX_WRAP_MODE[];
		DWORD MAXANISOTROPY;
		DWORD MAGFILTER;
		DWORD MINFILTER;
		DWORD MIPFILTER;
		DWORD BORDERCOLOR;
		DWORD ADDRESSU;
		DWORD ADDRESSV;
		Bool IsValid;
	};
	TArray<StateSampler> SamplerStates;

	struct StateTextureStage
	{
		StateTextureStage()
			:
			COLOROP(D3DTOP_SELECTARG1),
			COLORARG0(0),
			COLORARG1(D3DTA_TEXTURE),
			COLORARG2(D3DTA_DIFFUSE),
			ALPHAOP(D3DTOP_SELECTARG1),
			ALPHAARG0(D3DTA_DIFFUSE),
			ALPHAARG1(D3DTA_TEXTURE),
			ALPHAARG2(D3DTA_DIFFUSE),
			IsValid(false) {}

		static const DWORD TEX_BLEND[];
		DWORD COLOROP;
		DWORD COLORARG0;
		DWORD COLORARG1;
		DWORD COLORARG2;
		DWORD ALPHAOP;
		DWORD ALPHAARG0;
		DWORD ALPHAARG1;
		DWORD ALPHAARG2;
		Bool IsValid;
	};
	TArray<StateTextureStage> TextureStageStates;

	struct StateWireframe
	{
		StateWireframe()
			:
			FILLMODE(D3DFILL_SOLID),
			IsValid(false) {}

		DWORD FILLMODE;
		Bool IsValid;
	} WireframeState;

	struct StateZBuffer
	{
		StateZBuffer()
			:
			ZFUNC(D3DCMP_LESSEQUAL),
			ZWRITEENABLE(TRUE),
			IsValid(false) {}

		static const DWORD ZBUFFER_COMPARE[];
		DWORD ZFUNC;
		DWORD ZWRITEENABLE;
		Bool IsValid;
	} ZBufferState;

	static const D3DFORMAT IMAGE2D_FORMAT[];
	static const DWORD BUFFER_LOCKING[];
	static const D3DPOOL POOLS[];
	static const DWORD USAGES[];

	Bool Supports32BitIndices;
	Bool SupportsMinFAniso;
	Bool SupportsMagFAniso;
	Bool IsDeviceLost;
	Bool UsesRenormalizeNormals;

private:
	Renderer* mpRenderer;
};

}

#endif
