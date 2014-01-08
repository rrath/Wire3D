// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireDx9RendererData.h"

using namespace Wire;

const DWORD PdrRendererData::StateZBuffer::ZBUFFER_COMPARE[Wire::
	StateZBuffer::CF_QUANTITY] = 
{
	D3DCMP_NEVER,           // StateZBuffer::CF_NEVER
	D3DCMP_LESS,            // StateZBuffer::CF_LESS
	D3DCMP_EQUAL,           // StateZBuffer::CF_EQUAL
	D3DCMP_LESSEQUAL,       // StateZBuffer::CF_LEQUAL
	D3DCMP_GREATER,         // StateZBuffer::CF_GREATER
	D3DCMP_NOTEQUAL,        // StateZBuffer::CF_NOTEQUAL
	D3DCMP_GREATEREQUAL,    // StateZBuffer::CF_GEQUAL
	D3DCMP_ALWAYS,          // StateZBuffer::CF_ALWAYS
};

//----------------------------------------------------------------------------
void Renderer::SetState(StateZBuffer* pState)
{
	WIRE_ASSERT(pState);
	mspStates[State::ZBUFFER] = pState;

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;

	PdrRendererData::StateZBuffer& rState = mpData->ZBufferState;

	if (!rState.IsValid)
	{
		hr = rDevice->SetRenderState(D3DRS_ZFUNC, rState.ZFUNC);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetRenderState(D3DRS_ZWRITEENABLE, rState.
			ZWRITEENABLE);
		WIRE_ASSERT(SUCCEEDED(hr));

		rState.IsValid = true;
	}

	if (pState->Enabled)
	{
		DWORD zFunc = PdrRendererData::StateZBuffer::ZBUFFER_COMPARE[
			pState->Compare];

		if (rState.ZFUNC != zFunc)
		{
			hr = rDevice->SetRenderState(D3DRS_ZFUNC, zFunc);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.ZFUNC = zFunc;
		}
	}
	else
	{
		if (rState.ZFUNC != D3DCMP_ALWAYS)
		{
			hr = rDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.ZFUNC = D3DCMP_ALWAYS;
		}
	}

	if (pState->Writable)
	{
		if (rState.ZWRITEENABLE != TRUE)
		{
			hr = rDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.ZWRITEENABLE = TRUE;
		}
	}
	else
	{
		if (rState.ZWRITEENABLE != FALSE)
		{
			hr = rDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.ZWRITEENABLE = FALSE;
		}
	}
}
