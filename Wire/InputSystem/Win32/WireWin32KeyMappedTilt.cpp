// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32KeyMappedTilt.h"
#include "WireWin32InputDataBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32KeyMappedTilt, Tilt);

Win32KeyMappedTilt::Win32KeyMappedTilt(const InputDevice* pParent)
	: Tilt(pParent)
{
}

Win32KeyMappedTilt::~Win32KeyMappedTilt()
{
}

Float Win32KeyMappedTilt::GetBackward() const
{
	return 0;
}

Float Win32KeyMappedTilt::GetForward() const
{
	return 0;
}

Float Win32KeyMappedTilt::GetLeft() const
{
	return -static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetMouseWheel();
}

Float Win32KeyMappedTilt::GetRight() const
{
	return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetMouseWheel();
}
