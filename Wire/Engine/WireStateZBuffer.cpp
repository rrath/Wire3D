// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireStateZBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, StateZBuffer, State);
WIRE_IMPLEMENT_INITIALIZE(StateZBuffer);
WIRE_IMPLEMENT_TERMINATE(StateZBuffer);

//----------------------------------------------------------------------------
void StateZBuffer::Initialize()
{
	Default[ZBUFFER] = WIRE_NEW StateZBuffer;
}

//----------------------------------------------------------------------------
void StateZBuffer::Terminate()
{
	Default[ZBUFFER] = NULL;
}

//----------------------------------------------------------------------------
StateZBuffer::StateZBuffer()
	:
	Enabled(true),
	Writable(true),
	Compare(CF_LEQUAL)
{
}

//----------------------------------------------------------------------------
StateZBuffer::~StateZBuffer()
{
}
