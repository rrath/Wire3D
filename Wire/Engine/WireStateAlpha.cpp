// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireStateAlpha.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, StateAlpha, State);
WIRE_IMPLEMENT_INITIALIZE(StateAlpha);
WIRE_IMPLEMENT_TERMINATE(StateAlpha);

//----------------------------------------------------------------------------
void StateAlpha::Initialize()
{
	Default[ALPHA] = WIRE_NEW StateAlpha;
}

//----------------------------------------------------------------------------
void StateAlpha::Terminate()
{
	Default[ALPHA] = NULL;
}

//----------------------------------------------------------------------------
StateAlpha::StateAlpha()
	:
	BlendEnabled(false),
	SrcBlend(SBM_SRC_ALPHA),
	DstBlend(DBM_ONE_MINUS_SRC_ALPHA)
{
}

//----------------------------------------------------------------------------
StateAlpha::~StateAlpha()
{
}
