// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREBUTTONS_H
#define WIREBUTTONS_H

#include "WireInputCapability.h"

namespace Wire
{

class Buttons : public InputCapability
{
	WIRE_DECLARE_RTTI;

public:
	enum Button
	{
		BUTTON_A,
		BUTTON_B,
		BUTTON_C,
		BUTTON_Z,
		BUTTON_1,
		BUTTON_2,
		BUTTON_HOME,
		BUTTON_PLUS,
		BUTTON_MINUS
	};

	Buttons(const InputDevice* pParent);
	virtual ~Buttons();

	virtual Bool GetButton(Button button) const = 0;

};

}

#endif
