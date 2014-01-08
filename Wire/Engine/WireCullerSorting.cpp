// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireCullerSorting.h"

#include "WireRenderObject.h"
#include "WireStateAlpha.h"

using namespace Wire;

//----------------------------------------------------------------------------
CullerSorting::CullerSorting(const Camera* pCamera, UInt maxQuantity,
	UInt growBy)
	:
	Culler(pCamera, maxQuantity, growBy)
{
	mVisibleSets.Append(WIRE_NEW VisibleSet(maxQuantity, growBy));
}

//----------------------------------------------------------------------------
CullerSorting::~CullerSorting()
{
}

//----------------------------------------------------------------------------
void CullerSorting::ComputeVisibleSet(Spatial* pScene)
{
	Culler::ComputeVisibleSet(pScene);

	WIRE_ASSERT(mVisibleSets.GetQuantity() >= 2);
	mVisibleSets[0]->Sort();
	mVisibleSets[1]->Sort();
}

//----------------------------------------------------------------------------
void CullerSorting::Insert(Object* pObject, Transformation* pTransformation,
	const Vector3F& rPosition)
{
	WIRE_ASSERT(mVisibleSets.GetQuantity() >= 2);

	RenderObject* pRenderObject = DynamicCast<RenderObject>(pObject);
	if (!pRenderObject)
	{
		GetVisibleSet(0)->Insert(pObject, NULL);
		GetVisibleSet(1)->Insert(pObject, NULL);
		return;
	}

	UInt64 key = GetKey(pRenderObject, rPosition);
	StateAlpha* pAlpha = StaticCast<StateAlpha>(pRenderObject->GetStates()[
		State::ALPHA]);
	if (pAlpha)
	{
		if (pAlpha->BlendEnabled)
		{
			GetVisibleSet(1)->Insert(pObject, pTransformation, key);
		}
		else
		{
			GetVisibleSet(0)->Insert(pObject, pTransformation, key);
		}
	}
	else
	{
		GetVisibleSet(0)->Insert(pObject, pTransformation, key);
	}
}

//----------------------------------------------------------------------------
UInt64 CullerSorting::GetKey(RenderObject* pRenderObject, const Vector3F&
	rPosition)
{
	WIRE_ASSERT(pRenderObject);
	UInt64 key = 0;

	// number of bits we use for the sorting key (MSB to LSB)
	enum
	{
		STATESET = 24,
		MATERIAL = 16,
		DEPTH = 24
	};
	
	 // The sum of the ranges must fit in the key
	WIRE_ASSERT((STATESET + MATERIAL + DEPTH) <= sizeof(key) * 8);

	Float z = GetCamera()->GetDVector().Dot(rPosition);
	const Float far = GetCamera()->GetDMax();
	z += far;
	z /= far*3;
	z = z < 0 ? 0 : z;
	z = z >= 1.0F ? 1.0F - MathF::ZERO_TOLERANCE : z;

	key = static_cast<UInt64>(z * (1<<DEPTH));
	StateAlpha* pStateAlpha = StaticCast<StateAlpha>(pRenderObject->
		GetStates()[State::ALPHA]);
	if (pStateAlpha && pStateAlpha->BlendEnabled)
	{
		key = (((1<<DEPTH)-1) - key);
	}

	WIRE_ASSERT(key < (1<<DEPTH));

	// The following asserts let you know when you have created more materials
	// and state sets than the key can handle. 
	Material* pMaterial = pRenderObject->GetMaterial();
	if (pMaterial)
	{
		WIRE_ASSERT(pMaterial->ID < (1<<MATERIAL));
		key |= pMaterial->ID << DEPTH;
	}

	// If StateSetID is MAX_UINT, it wasn't initialized (call UpdateRS() once,
	// or initialize manually)
	WIRE_ASSERT(pRenderObject->GetStateSetID() < (1<<STATESET));
	key |= static_cast<UInt64>(pRenderObject->GetStateSetID()) << (MATERIAL + DEPTH);

	return key;
}
