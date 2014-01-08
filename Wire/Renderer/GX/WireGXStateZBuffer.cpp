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

const UChar PdrRendererData::ZBUFFER_COMPARE[StateZBuffer::CF_QUANTITY] = 
{
	GX_NEVER,     // StateZBuffer::CF_NEVER
	GX_LESS,      // StateZBuffer::CF_LESS
	GX_EQUAL,     // StateZBuffer::CF_EQUAL
	GX_LEQUAL,    // StateZBuffer::CF_LEQUAL
	GX_GREATER,   // StateZBuffer::CF_GREATER
	GX_NEQUAL,    // StateZBuffer::CF_NOTEQUAL
	GX_GEQUAL,    // StateZBuffer::CF_GEQUAL
	GX_ALWAYS,    // StateZBuffer::CF_ALWAYS
};

//----------------------------------------------------------------------------
void Renderer::SetState(StateZBuffer* pState)
{
	WIRE_ASSERT(pState);
	mspStates[State::ZBUFFER] = pState;

	UChar enable = pState->Enabled ? GX_TRUE : GX_FALSE;
	UChar writable = pState->Writable ? GX_TRUE : GX_FALSE;

	GXSetZMode(enable, PdrRendererData::ZBUFFER_COMPARE[pState->Compare],
		writable);
}
