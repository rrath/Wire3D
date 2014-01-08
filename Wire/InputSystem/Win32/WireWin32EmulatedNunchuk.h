// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIN32EMULATEDNUNCHUK_H
#define WIREWIN32EMULATEDNUNCHUK_H

#include "WireNunchuk.h"

namespace Wire
{

class Win32EmulatedNunchuk : public Nunchuk
{
	WIRE_DECLARE_RTTI;

public:
	Win32EmulatedNunchuk(MainInputDevice* pMainInputDevice);
	virtual ~Win32EmulatedNunchuk();

	virtual void SetUp();

};

}

#endif
