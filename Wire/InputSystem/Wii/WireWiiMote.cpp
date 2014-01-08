// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWiiMote.h"
#include "WireWiiMoteButtons.h"
#include "WireWiiMoteDigitalPad.h"
#include "WireWiiMoteIR.h"
#include "WireWiiMoteShake.h"
#include "WireWiiMoteSwing.h"
#include "WireWiiMoteTilt.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, WiiMote, MainInputDevice);

WiiMote::WiiMote(Int channel)
	: mChannel(channel)
{
}

WiiMote::~WiiMote()
{
}

UInt WiiMote::GetChannel() const
{
	return mChannel;
}

void WiiMote::SetUp()
{
	RegisterCapability(WIRE_NEW WiiMoteButtons(this));
	RegisterCapability(WIRE_NEW WiiMoteDigitalPad(this));
	RegisterCapability(WIRE_NEW WiiMoteIR(this));
	RegisterCapability(WIRE_NEW WiiMoteShake(this));
	RegisterCapability(WIRE_NEW WiiMoteSwing(this));
	RegisterCapability(WIRE_NEW WiiMoteTilt(this));
}
