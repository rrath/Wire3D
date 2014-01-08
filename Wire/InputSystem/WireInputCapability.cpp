// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireInputCapability.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, InputCapability, Object);

InputCapability::InputCapability(const InputDevice* pParent) 
	: mpParent(pParent)
{
}

InputCapability::~InputCapability()
{
}

const InputDevice* InputCapability::GetParent() const
{
	return mpParent;
}
