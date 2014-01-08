// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWiiNunchuk.h"
#include "WireNunchukAnalogPad.h"
#include "WireNunchukButtons.h"
#include "WireNunchukShake.h"
#include "WireNunchukSwing.h"
#include "WireNunchukTilt.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, WiiNunchuk, Nunchuk);

WiiNunchuk::WiiNunchuk(MainInputDevice* pMainInputDevice)
	:
	Nunchuk(pMainInputDevice)
{
}

WiiNunchuk::~WiiNunchuk()
{
}

void WiiNunchuk::SetUp()
{
	RegisterCapability(WIRE_NEW NunchukAnalogPad(this));
	RegisterCapability(WIRE_NEW NunchukButtons(this));
	RegisterCapability(WIRE_NEW NunchukShake(this));
	RegisterCapability(WIRE_NEW NunchukSwing(this));
	RegisterCapability(WIRE_NEW NunchukTilt(this));
}
