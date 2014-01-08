// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREEFFECT_H
#define WIREEFFECT_H

#include "WireObject.h"

namespace Wire
{

class Renderer;
class Transformation;

class Effect : public Object
{
	WIRE_DECLARE_RTTI;

public:
	enum
	{
		MAX_SIMULTANEOUS_EFFECTS = 64
	};

	// abstract base class
	virtual ~Effect();

	// Override this function to obtain whatever drawing behavior your effect
	// requires. If this is not overridden, the default behavior is to
	// draw all the RenderObjects in pVisible.
	virtual void Draw(Renderer* pRenderer, Object* const pVisible[],
		Transformation* const pTransformations[], UInt min, UInt max,
		Bool restoreState);

protected:
 	Effect();
};

typedef Pointer<Effect> EffectPtr;

}

#endif
