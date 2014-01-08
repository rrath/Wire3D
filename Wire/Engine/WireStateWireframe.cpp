// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireStateWireframe.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, StateWireframe, State);
WIRE_IMPLEMENT_INITIALIZE(StateWireframe);
WIRE_IMPLEMENT_TERMINATE(StateWireframe);

//----------------------------------------------------------------------------
void StateWireframe::Initialize()
{
	Default[WIREFRAME] = WIRE_NEW StateWireframe;
}

//----------------------------------------------------------------------------
void StateWireframe::Terminate()
{
	Default[WIREFRAME] = NULL;
}

//----------------------------------------------------------------------------
StateWireframe::StateWireframe()
	:
	Enabled(false)
{
}

//----------------------------------------------------------------------------
StateWireframe::~StateWireframe()
{
}
