// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREZBUFFERSTATE_H
#define WIREZBUFFERSTATE_H

#include "WireState.h"
#include "WireTInstanceID.h"

namespace Wire
{

class StateZBuffer : public State
{
	WIRE_DECLARE_RTTI;
	WIRE_DECLARE_INITIALIZE;
	WIRE_DECLARE_TERMINATE;

public:
	StateZBuffer();
	virtual ~StateZBuffer();

	inline virtual StateType GetStateType() const { return ZBUFFER; }

	enum CompareFunction
	{
		CF_NEVER,
		CF_LESS,
		CF_EQUAL,
		CF_LEQUAL,
		CF_GREATER,
		CF_NOTEQUAL,
		CF_GEQUAL,
		CF_ALWAYS,
		CF_QUANTITY
	};

	Bool Enabled;				// default: true
	Bool Writable;				// default: true
	CompareFunction Compare;	// default: CF_LEQUAL

	TInstanceID<StateZBuffer> ID;
};

typedef Pointer<StateZBuffer> StateZBufferPtr;

WIRE_REGISTER_INITIALIZE(StateZBuffer);
WIRE_REGISTER_TERMINATE(StateZBuffer);

}

#endif
