// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireStateFog.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, StateFog, State);
WIRE_IMPLEMENT_INITIALIZE(StateFog);
WIRE_IMPLEMENT_TERMINATE(StateFog);

//----------------------------------------------------------------------------
void StateFog::Initialize()
{
	Default[FOG] = WIRE_NEW StateFog;
}

//----------------------------------------------------------------------------
void StateFog::Terminate()
{
	Default[FOG] = NULL;
}

//----------------------------------------------------------------------------
StateFog::StateFog()
	:
	Enabled(false),
	Color(ColorRGB::BLACK),
	DensityFunc(DF_LINEAR),
	Start(0.0F),
	End(1.0F)
{
}

//----------------------------------------------------------------------------
StateFog::~StateFog()
{
}
