// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWiiMoteShake.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, WiiMoteShake, Shake);

WiiMoteShake::WiiMoteShake(const InputDevice* pParent)
	: Shake(pParent)
{
}

WiiMoteShake::~WiiMoteShake()
{
}

Float WiiMoteShake::GetX() const
{
	return 0;
}

Float WiiMoteShake::GetY() const
{
	return 0;
}

Float WiiMoteShake::GetZ() const
{
	return 0;
}
