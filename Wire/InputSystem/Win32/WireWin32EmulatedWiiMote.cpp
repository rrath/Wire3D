// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32EmulatedWiiMote.h"
#include "WireWin32KeyMappedDigitalPad.h"
#include "WireWin32KeyMappedButtons.h"
#include "WireWin32KeyMappedIR.h"
#include "WireWin32KeyMappedShake.h"
#include "WireWin32KeyMappedSwing.h"
#include "WireWin32KeyMappedTilt.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32EmulatedWiiMote, MainInputDevice);

Win32EmulatedWiiMote::Win32EmulatedWiiMote()
{
}

Win32EmulatedWiiMote::~Win32EmulatedWiiMote()
{
}

void Win32EmulatedWiiMote::SetUp()
{
	RegisterCapability(WIRE_NEW Win32KeyMappedDigitalPad(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedButtons(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedIR(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedShake(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedSwing(this));
	RegisterCapability(WIRE_NEW Win32KeyMappedTilt(this));
}
