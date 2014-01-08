// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWiiMoteSwing.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, WiiMoteSwing, Swing);
	
WiiMoteSwing::WiiMoteSwing(const InputDevice* pParent)
	: Swing(pParent)
{
}

WiiMoteSwing::~WiiMoteSwing()
{
}

Float WiiMoteSwing::GetBackward() const
{
	return 0;
}

Float WiiMoteSwing::GetDown() const
{
	return 0;
}

Float WiiMoteSwing::GetForward() const
{
	return 0;
}

Float WiiMoteSwing::GetLeft() const
{
	return 0;
}

Float WiiMoteSwing::GetRight() const
{
	return 0;
}

Float WiiMoteSwing::GetUp() const
{
	return 0;
}
