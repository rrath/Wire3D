// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIN32KEYMAPPEDANALOGPAD_H
#define WIREWIN32KEYMAPPEDANALOGPAD_H

#include "WireAnalogPad.h"

namespace Wire
{

class Win32KeyMappedAnalogPad : public AnalogPad
{
	WIRE_DECLARE_RTTI;

public:
	Win32KeyMappedAnalogPad(const InputDevice* pParent);
	virtual ~Win32KeyMappedAnalogPad();

	virtual Float GetDown() const;
	virtual Float GetLeft() const;
	virtual Float GetRight() const;
	virtual Float GetUp() const;

};

}

#endif
