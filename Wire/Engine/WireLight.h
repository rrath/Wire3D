// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRELIGHT_H
#define WIRELIGHT_H

#include "WireColorRGB.h"
#include "WireObject.h"
#include "WireTHashTable.h"
#include "WireTInstanceID.h"
#include "WireVector3.h"

namespace Wire
{

class Light : public Object
{
	WIRE_DECLARE_RTTI;

public:
	enum LightType
	{
		LT_DIRECTIONAL,
		LT_POINT,
		LT_SPOT,
		LT_QUANTITY
	};

	Light(LightType type = LT_DIRECTIONAL);
	virtual ~Light();

	TInstanceID<Light> ID;

	LightType Type;

	ColorRGB Ambient;	// default: BLACK
	ColorRGB Color;		// default: WHITE

	// position and direction in world coordinates
	Vector3F Position;	// default: ZERO
	Vector3F Direction;	// default: -UNIT_Z

	// Distance attenuation, typically specified as a modulator
	//   m = 1/(C + L*d + Q/(d*d))
	// where C is the constant coefficient, L is the linear coefficient,
	// Q is the quadratic coefficient, R is the reference distance (range)
	// and d is the distance from the light position to the vertex position. 
	Float Constant;		// default: 1
	Float Linear;		// default: 0
	Float Quadric;		// default: 0

	// parameters for spot lights
	Float Angle;		// default: Math::PI
	Float Exponent;		// default: 1

	// default behavior is to light all layers
	UInt64 LayerMask;	// default: 0xFFFFFFFFFFFFFFFF

	Bool Enabled;		// default: true

	static UInt GetSetID(TArray<Pointer<Light> >* pLights);

private:
	static THashTable<UInt64, UInt> mSetIDs;
};

typedef Pointer<Light> LightPtr;

}

#endif
