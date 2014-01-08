// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireState.h"

#include "WireStateAlpha.h"
#include "WireStateCull.h"
#include "WireStateFog.h"
#include "WireStateMaterial.h"
#include "WireStateWireframe.h"
#include "WireStateZBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, State, Object);

StatePtr State::Default[MAX_STATE_TYPE];

THashTable<UInt64, UInt> State::mSetIDs;

//----------------------------------------------------------------------------
State::State()
{
}

//----------------------------------------------------------------------------
State::~State()
{
}

//----------------------------------------------------------------------------
void State::Init(Pointer<State> states[State::MAX_STATE_TYPE])
{
	for (UInt i = 0; i < MAX_STATE_TYPE; i++)
	{
		states[i] = Default[i];
	}
}

//----------------------------------------------------------------------------
UInt State::GetSetID(StatePtr states[MAX_STATE_TYPE])
{
	enum
	{
		ALPHABITS = 10,
		CULLBITS = 10,
		FOGBITS = 10,
		MATERIALBITS = 14,
		WIREFRAMEBITS = 10,
		ZBUFFERBITS = 10
	};

	WIRE_ASSERT((ALPHABITS + CULLBITS + FOGBITS + MATERIALBITS + WIREFRAMEBITS
		+ ZBUFFERBITS) == (sizeof(UInt64)*8));

	UInt64 setID = StaticCast<StateAlpha>(states[ALPHA])->ID;
	WIRE_ASSERT(setID < (1 << ALPHABITS));

	UInt64 cullID = StaticCast<StateCull>(states[CULL])->ID;
	WIRE_ASSERT(cullID < (1 << CULLBITS));
	setID |= cullID << ALPHABITS;

	UInt64 fogID = StaticCast<StateFog>(states[FOG])->ID;
	WIRE_ASSERT(fogID < (1 << FOGBITS));
	setID |= fogID << (ALPHABITS + CULLBITS);

	UInt64 materialID = StaticCast<StateMaterial>(states[MATERIAL])->ID;
	WIRE_ASSERT(materialID < (1 << MATERIALBITS));
	setID |= materialID << (ALPHABITS + CULLBITS + FOGBITS);

	UInt64 wireframeID = StaticCast<StateWireframe>(states[WIREFRAME])->ID;
	WIRE_ASSERT(wireframeID < (1 << WIREFRAMEBITS));
	setID |= wireframeID << (ALPHABITS + CULLBITS + FOGBITS + MATERIALBITS);

	UInt64 zbufferID = StaticCast<StateZBuffer>(states[ZBUFFER])->ID;
	WIRE_ASSERT(zbufferID < (1 << ZBUFFERBITS));
	setID |= zbufferID << (ALPHABITS + CULLBITS + FOGBITS + MATERIALBITS +
		WIREFRAMEBITS);

	UInt* pValue = mSetIDs.Find(setID);
	if (pValue)
	{
		return *pValue;
	}

	UInt value = mSetIDs.GetQuantity();
	mSetIDs.Insert(setID, value);
	return value;
}
