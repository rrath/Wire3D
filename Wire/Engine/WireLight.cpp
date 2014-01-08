// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireLight.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Light, Object);

THashTable<UInt64, UInt> Light::mSetIDs;

//----------------------------------------------------------------------------
Light::Light(LightType type)
	:
	Type(type),
	Ambient(ColorRGB::BLACK),
	Color(ColorRGB::WHITE),
	Position(Vector3F::ZERO),
	Direction(-Vector3F::UNIT_Z),
	Constant(1),
	Linear(0),
	Quadric(0),
	Angle(MathF::PI),
	Exponent(1.0F),
	LayerMask(static_cast<UInt64>(~0)),
	Enabled(true)
{
}

//----------------------------------------------------------------------------
Light::~Light()
{
}

//----------------------------------------------------------------------------
UInt Light::GetSetID(TArray<LightPtr>* pLights)
{
	enum
	{
		BITSPERLIGHT = 8,
		MAXLIGHTS = 8,
	};

	WIRE_ASSERT((BITSPERLIGHT * MAXLIGHTS) <= (sizeof(UInt64)*8));
	WIRE_ASSERT(pLights->GetQuantity() <= MAXLIGHTS);

	UInt64 setID = 0;
	for (UInt i = 0; i < pLights->GetQuantity(); i++)
	{
		UInt64 id = (*pLights)[i]->ID;
		WIRE_ASSERT(id < (1 << BITSPERLIGHT));
		id = id << (BITSPERLIGHT*i);
		setID |= id;
	}

	UInt* pValue = mSetIDs.Find(setID);
	if (pValue)
	{
		return *pValue;
	}

	UInt value = mSetIDs.GetQuantity();
	mSetIDs.Insert(setID, value);
	return value;
}
