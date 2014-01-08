// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREALPHASTATE_H
#define WIREALPHASTATE_H

#include "WireTInstanceID.h"
#include "WireState.h"

namespace Wire
{

class StateAlpha : public State
{
	WIRE_DECLARE_RTTI;
	WIRE_DECLARE_INITIALIZE;
	WIRE_DECLARE_TERMINATE;

public:
	enum SrcBlendMode
	{
		SBM_ZERO,
		SBM_ONE,
		SBM_DST_COLOR,
		SBM_ONE_MINUS_DST_COLOR,
		SBM_SRC_ALPHA,
		SBM_ONE_MINUS_SRC_ALPHA,
		SBM_DST_ALPHA,
		SBM_ONE_MINUS_DST_ALPHA,
		SBM_QUANTITY
	};

	enum DstBlendMode
	{
		DBM_ZERO,
		DBM_ONE,
		DBM_SRC_COLOR,
		DBM_ONE_MINUS_SRC_COLOR,
		DBM_SRC_ALPHA,
		DBM_ONE_MINUS_SRC_ALPHA,
		DBM_DST_ALPHA,
		DBM_ONE_MINUS_DST_ALPHA,
		DBM_QUANTITY
	};

	StateAlpha();
	virtual ~StateAlpha();

	inline virtual StateType GetStateType() const { return ALPHA; }

	Bool BlendEnabled;      // default: false
	SrcBlendMode SrcBlend;  // default: SBM_SRC_ALPHA
	DstBlendMode DstBlend;  // default: DBM_ONE_MINUS_SRC_ALPHA

	TInstanceID<StateAlpha> ID;
};

typedef Pointer<StateAlpha> StateAlphaPtr;

WIRE_REGISTER_INITIALIZE(StateAlpha);
WIRE_REGISTER_TERMINATE(StateAlpha);

}

#endif
