// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIN32KEYMAPPEDDIGITALPAD_H
#define WIREWIN32KEYMAPPEDDIGITALPAD_H

#include "WireDigitalPad.h"

namespace Wire
{

class Win32KeyMappedDigitalPad : public DigitalPad
{
	WIRE_DECLARE_RTTI;

public:
	Win32KeyMappedDigitalPad(const InputDevice* pParent);
	virtual ~Win32KeyMappedDigitalPad();

	virtual Bool GetDown() const;
	virtual Bool GetLeft() const;
	virtual Bool GetRight() const;
	virtual Bool GetUp() const;

};

}

#endif
