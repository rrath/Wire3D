// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireStateCull.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, StateCull, State);
WIRE_IMPLEMENT_INITIALIZE(StateCull);
WIRE_IMPLEMENT_TERMINATE(StateCull);

//----------------------------------------------------------------------------
void StateCull::Initialize()
{
	Default[CULL] = WIRE_NEW StateCull;
}

//----------------------------------------------------------------------------
void StateCull::Terminate()
{
	Default[CULL] = NULL;
}

//----------------------------------------------------------------------------
StateCull::StateCull()
	:
	Enabled(true),
	CullFace(CM_BACK)
{
}

//----------------------------------------------------------------------------
StateCull::~StateCull()
{
}
