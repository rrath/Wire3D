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

const DWORD PdrRendererData::StateCull::CULL_TYPE[Wire::StateCull::
	CM_QUANTITY] = 
{
	D3DCULL_NONE,
	D3DCULL_CCW,  // StateCull::CM_FRONT(front CCW -> cull backface CCW in DX)
	D3DCULL_CW,   // StateCull::CM_BACK (front CW -> cull backface CW in DX)
};

//----------------------------------------------------------------------------
void Renderer::SetState(StateCull* pState)
{
	WIRE_ASSERT(pState);
	mspStates[State::CULL] = pState;

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;

	PdrRendererData::StateCull& rState = mpData->CullState;

	if (!rState.IsValid)
	{
		hr = rDevice->SetRenderState(D3DRS_CULLMODE, rState.CULLMODE);
		WIRE_ASSERT(SUCCEEDED(hr));

		rState.IsValid = true;
	}

	if (pState->Enabled)
	{
		DWORD cullMode = PdrRendererData::StateCull::CULL_TYPE[pState->
			CullFace];
		if (rState.CULLMODE != cullMode)
		{
			hr = rDevice->SetRenderState(D3DRS_CULLMODE, cullMode);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.CULLMODE = cullMode;
		}
	}
	else
	{
		if (rState.CULLMODE != D3DCULL_NONE)
		{
			hr = rDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.CULLMODE = D3DCULL_NONE;
		}
	}
}
