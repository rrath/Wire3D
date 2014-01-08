// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRECULLSTATE_H
#define WIRECULLSTATE_H

#include "WireState.h"
#include "WireTInstanceID.h"

namespace Wire
{

class StateCull : public State
{
	WIRE_DECLARE_RTTI;
	WIRE_DECLARE_INITIALIZE;
	WIRE_DECLARE_TERMINATE;

public:
	StateCull();
	virtual ~StateCull();

	virtual StateType GetStateType() const { return CULL; }

	enum CullMode
	{
		CM_OFF,		// do not cull any triangles
		CM_BACK,	// cull back-facing triangles
		CM_FRONT,	// cull front-facing triangles
		CM_QUANTITY
	};

	Bool Enabled;         // default: true
	CullMode CullFace;    // default: CM_BACK

	TInstanceID<StateCull> ID;
};

typedef Pointer<StateCull> StateCullPtr;

WIRE_REGISTER_INITIALIZE(StateCull);
WIRE_REGISTER_TERMINATE(StateCull);

}

#endif
