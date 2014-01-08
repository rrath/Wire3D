// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGLOBALSTATE_H
#define WIREGLOBALSTATE_H

#include "WireMain.h"
#include "WireObject.h"
#include "WireTHashTable.h"

namespace Wire
{

class State : public Object
{
	WIRE_DECLARE_RTTI;

public:
	// abstract base class
	virtual ~State();

	// supported global states
	enum StateType
	{
		ALPHA,
		CULL,
		FOG,
		MATERIAL,
		WIREFRAME,
		ZBUFFER,
		MAX_STATE_TYPE
	};

	virtual StateType GetStateType() const = 0;

	static Pointer<State> Default[MAX_STATE_TYPE];
	static void Init(Pointer<State> states[State::MAX_STATE_TYPE]);
	static UInt GetSetID(Pointer<State> states[MAX_STATE_TYPE]);

protected:
	State();

private:
	static THashTable<UInt64, UInt> mSetIDs;
};

typedef Pointer<State> StatePtr;

}

#endif
