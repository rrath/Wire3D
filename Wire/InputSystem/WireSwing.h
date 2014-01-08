// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRESWING_H
#define WIRESWING_H

#include "WireInputCapability.h"

namespace Wire
{

class Swing : public InputCapability
{
	WIRE_DECLARE_RTTI;

public:
	Swing(const InputDevice* pParent);
	virtual ~Swing();

	virtual Float GetBackward() const = 0;
	virtual Float GetDown() const = 0;
	virtual Float GetForward() const = 0;
	virtual Float GetLeft() const = 0;
	virtual Float GetRight() const = 0;
	virtual Float GetUp() const = 0;

};

}

#endif
