// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2013. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIIMOTEBUTTONS_H
#define WIREWIIMOTEBUTTONS_H

#include "WireButtons.h"

namespace Wire
{

class WiiMoteButtons : public Buttons
{
	WIRE_DECLARE_RTTI;

public:
	WiiMoteButtons(const InputDevice* pParent);
	virtual ~WiiMoteButtons();

	virtual Bool GetButton(Button button) const;

};

}

#endif
