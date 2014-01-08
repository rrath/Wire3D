// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREDIGITALPAD_H
#define WIREDIGITALPAD_H

#include "WireInputCapability.h"

namespace Wire
{

class DigitalPad : public InputCapability
{
	WIRE_DECLARE_RTTI;

public:
	DigitalPad(const InputDevice* pParent);
	virtual ~DigitalPad();

	virtual Bool GetDown() const = 0;
	virtual Bool GetLeft() const = 0;
	virtual Bool GetRight() const = 0;
	virtual Bool GetUp() const = 0;

};

}

#endif
