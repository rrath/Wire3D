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

//----------------------------------------------------------------------------
void Renderer::SetState(StateWireframe* pState)
{
	WIRE_ASSERT(pState);
	mspStates[State::WIREFRAME] = pState;

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;

	PdrRendererData::StateWireframe& rState = mpData->WireframeState;

	if (!rState.IsValid)
	{
		hr = rDevice->SetRenderState(D3DRS_FILLMODE, rState.FILLMODE);
		WIRE_ASSERT(SUCCEEDED(hr));

		rState.IsValid = true;
	}

    if (pState->Enabled)
    {
		if (rState.FILLMODE != D3DFILL_WIREFRAME)
		{
			hr = rDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.FILLMODE = D3DFILL_WIREFRAME;
		}
    }
    else
    {
		if (rState.FILLMODE != D3DFILL_SOLID)
		{
			hr = rDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.FILLMODE = D3DFILL_SOLID;
		}
    }
}
