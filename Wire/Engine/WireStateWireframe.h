// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIREFRAMESTATE_H
#define WIREWIREFRAMESTATE_H

#include "WireState.h"
#include "WireTInstanceID.h"

namespace Wire
{

class StateWireframe : public State
{
	WIRE_DECLARE_RTTI;
	WIRE_DECLARE_INITIALIZE;
	WIRE_DECLARE_TERMINATE;

public:
	StateWireframe();
	virtual ~StateWireframe();

	inline virtual StateType GetStateType() const { return WIREFRAME; }

	Bool Enabled;         // default: false

	TInstanceID<StateWireframe> ID;
};

typedef Pointer<StateWireframe> StateWireframePtr;

WIRE_REGISTER_INITIALIZE(StateWireframe);
WIRE_REGISTER_TERMINATE(StateWireframe);

}

#endif
