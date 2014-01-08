// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireStateMaterial.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, StateMaterial, State);
WIRE_IMPLEMENT_INITIALIZE(StateMaterial);
WIRE_IMPLEMENT_TERMINATE(StateMaterial);

//----------------------------------------------------------------------------
void StateMaterial::Initialize()
{
	Default[MATERIAL] = WIRE_NEW StateMaterial;
}

//----------------------------------------------------------------------------
void StateMaterial::Terminate()
{
	Default[MATERIAL] = NULL;
}

//----------------------------------------------------------------------------
StateMaterial::StateMaterial()
	:
	Ambient(0.2F, 0.2F, 0.2F, 1.0F)
{
}

//----------------------------------------------------------------------------
StateMaterial::~StateMaterial()
{
}
