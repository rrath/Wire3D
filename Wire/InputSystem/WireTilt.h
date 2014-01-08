// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETILT_H
#define WIRETILT_H

#include "WireInputCapability.h"

namespace Wire
{

class Tilt : public InputCapability
{
	WIRE_DECLARE_RTTI;

public:
	Tilt(const InputDevice* pParent);
	virtual ~Tilt();

	virtual Float GetBackward() const = 0;
	virtual Float GetForward() const = 0;
	virtual Float GetLeft() const = 0;
	virtual Float GetRight() const = 0;

};

}

#endif
