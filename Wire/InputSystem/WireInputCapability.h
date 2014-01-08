// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREINPUTCAPABILITY_H
#define WIREINPUTCAPABILITY_H

#include "WireInputDevice.h"

namespace Wire
{

class InputCapability : public Object
{
	WIRE_DECLARE_RTTI;

public:
	InputCapability(const InputDevice* pParent);
	virtual ~InputCapability();

protected:
	const InputDevice* GetParent() const;

private:
	const InputDevice* mpParent;

};

typedef Pointer<InputCapability> InputCapabilityPtr;

}

#endif
