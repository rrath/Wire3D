// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32KeyMappedSwing.h"
#include "WireWin32InputDataBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32KeyMappedSwing, Swing);

Win32KeyMappedSwing::Win32KeyMappedSwing(const InputDevice* pParent)
	: Swing(pParent)
{
}

Win32KeyMappedSwing::~Win32KeyMappedSwing()
{
}

Float Win32KeyMappedSwing::GetBackward() const
{
	return 0;
}

Float Win32KeyMappedSwing::GetDown() const
{
	return 0;
}

Float Win32KeyMappedSwing::GetForward() const
{
	return 0;
}

Float Win32KeyMappedSwing::GetLeft() const
{
	return 0;
}

Float Win32KeyMappedSwing::GetRight() const
{
	return 0;
}

Float Win32KeyMappedSwing::GetUp() const
{
	return 0;
}
