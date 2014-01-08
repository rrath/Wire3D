// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireEffect.h"

#include "WireRenderer.h"
#include "WireRenderObject.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Effect, Object);

//----------------------------------------------------------------------------
Effect::Effect()
{
}

//----------------------------------------------------------------------------
Effect::~Effect()
{
}

//----------------------------------------------------------------------------
void Effect::Draw(Renderer* pRenderer, Object* const pVisible[],
	Transformation* const pTransformations[], UInt min, UInt max,
	Bool restoreState)
{
	// The default drawing function for effects. Essentially, this draws
	// all the RenderObjects, as if no effect was applied. Override to obtain
	// a different behavior.
	for (UInt i = min; i <= max; i++)
	{
		// NULL transformation means start or end of a nested Effect
		if (pTransformations[i])
		{
			WIRE_ASSERT(DynamicCast<RenderObject>(pVisible[i]));
			RenderObject* pRenderObject = StaticCast<RenderObject>(pVisible[i]);
			pRenderer->Draw(pRenderObject, *(pTransformations[i]), restoreState);
		}
	}

	// For draw call batching the following approach can be used:
// 	RenderObject* const* pRenderObjects = reinterpret_cast<RenderObject*
// 		const*>(pVisible);
// 	pRenderer->Draw(pRenderObjects, pTransformations, min, max+1);
}
