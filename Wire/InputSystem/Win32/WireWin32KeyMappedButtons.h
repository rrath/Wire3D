// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIN32KEYMAPPEDBUTTONS_H
#define WIREWIN32KEYMAPPEDBUTTONS_H

#include "WireButtons.h"

namespace Wire
{

class Win32KeyMappedButtons : public Buttons
{
	WIRE_DECLARE_RTTI;

public:
	Win32KeyMappedButtons(const InputDevice* pParent);
	virtual ~Win32KeyMappedButtons();

	virtual Bool GetButton(Button button) const;

};

}

#endif
