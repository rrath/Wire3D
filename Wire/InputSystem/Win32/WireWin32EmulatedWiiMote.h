// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIN32EMULATEDWIIMOTE_H
#define WIREWIN32EMULATEDWIIMOTE_H

#include "WireMainInputDevice.h"

namespace Wire
{

class Win32EmulatedWiiMote : public MainInputDevice
{
	WIRE_DECLARE_RTTI;

public:
	Win32EmulatedWiiMote();
	virtual ~Win32EmulatedWiiMote();

	virtual void SetUp();

};

}

#endif
