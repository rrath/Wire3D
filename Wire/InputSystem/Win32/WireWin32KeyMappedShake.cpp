// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32KeyMappedShake.h"
#include "WireWin32InputDataBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32KeyMappedShake, Shake);

Win32KeyMappedShake::Win32KeyMappedShake(const InputDevice* pParent)
	: Shake(pParent)
{
}

Win32KeyMappedShake::~Win32KeyMappedShake()
{
}

Float Win32KeyMappedShake::GetX() const
{
	return 0;
}

Float Win32KeyMappedShake::GetY() const
{
	return 0;
}

Float Win32KeyMappedShake::GetZ() const
{
	return 0;
}
