// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRESHAKE_H
#define WIRESHAKE_H

#include "WireInputCapability.h"

namespace Wire
{

class Shake : public InputCapability
{
	WIRE_DECLARE_RTTI;

public:
	Shake(const InputDevice* pParent);
	virtual ~Shake();

	virtual Float GetX() const = 0;
	virtual Float GetY() const = 0;
	virtual Float GetZ() const = 0;

};

}

#endif
