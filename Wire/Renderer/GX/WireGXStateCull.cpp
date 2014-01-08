// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireGXRendererData.h"

using namespace Wire;

const UChar PdrRendererData::CULL_TYPE[StateCull::CM_QUANTITY] = 
{
	GX_CULL_NONE,
	GX_CULL_BACK,
	GX_CULL_FRONT
};

//----------------------------------------------------------------------------
void Renderer::SetState(StateCull* pState)
{
	WIRE_ASSERT(pState);
	mspStates[State::CULL] = pState;

	if (pState->Enabled)
	{
		GXSetCullMode(PdrRendererData::CULL_TYPE[pState->CullFace]);
	}
	else
	{
		GXSetCullMode(GX_CULL_NONE);
	}
}
