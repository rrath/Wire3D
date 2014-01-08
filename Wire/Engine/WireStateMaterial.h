// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREMATERIALSTATE_H
#define WIREMATERIALSTATE_H

#include "WireColorRGBA.h"
#include "WireState.h"
#include "WireTInstanceID.h"

namespace Wire
{

class StateMaterial : public State
{
	WIRE_DECLARE_RTTI;
	WIRE_DECLARE_INITIALIZE;
	WIRE_DECLARE_TERMINATE;

public:
	StateMaterial();
	virtual ~StateMaterial();

	inline virtual StateType GetStateType() const { return MATERIAL; }

    ColorRGBA Ambient;   // default:  ColorRGBA(0.2, 0.2, 0.2, 1)

	TInstanceID<StateMaterial> ID;
};

typedef Pointer<StateMaterial> StateMaterialPtr;

WIRE_REGISTER_INITIALIZE(StateMaterial);
WIRE_REGISTER_TERMINATE(StateMaterial);

}

#endif
