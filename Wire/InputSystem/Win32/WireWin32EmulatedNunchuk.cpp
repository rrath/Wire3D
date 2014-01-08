// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32EmulatedNunchuk.h"
#include "WireWin32KeyMappedAnalogPad.h"
#include "WireWin32KeyMappedButtons.h"
#include "WireWin32KeyMappedShake.h"
#include "WireWin32KeyMappedSwing.h"
#include "WireWin32KeyMappedTilt.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32EmulatedNunchuk, Nunchuk);

Win32EmulatedNunchuk::Win32EmulatedNunchuk(MainInputDevice* pMainInputDevice)
	:
	Nunchuk(pMainInputDevice)
{
}

Win32EmulatedNunchuk::~Win32EmulatedNunchuk()
{
}

void Win32EmulatedNunchuk::SetUp()
{
	RegisterCapability(WIRE_NEW Win32KeyMappedAnalogPad(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedButtons(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedShake(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedSwing(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedTilt(this));
}
