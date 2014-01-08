// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32KeyMappedAnalogPad.h"
#include "WireWin32InputDataBuffer.h"
#include <Windows.h>

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32KeyMappedAnalogPad, AnalogPad);

Win32KeyMappedAnalogPad::Win32KeyMappedAnalogPad(const InputDevice* pParent)
	: AnalogPad(pParent)
{
}

Win32KeyMappedAnalogPad::~Win32KeyMappedAnalogPad()
{
}

Float Win32KeyMappedAnalogPad::GetUp() const
{
	// 'w' key
	return (static_cast<const Win32InputDataBuffer*>(GetParent()->
		GetDataBuffer())->GetKey(0x57)) ? 1.0F : 0;
}

Float Win32KeyMappedAnalogPad::GetLeft() const
{
	// 'a' key
	return (static_cast<const Win32InputDataBuffer*>(GetParent()->
		GetDataBuffer())->GetKey(0x41)) ? 1.0F : 0;
}

Float Win32KeyMappedAnalogPad::GetDown() const
{
	// 's' key
	return (static_cast<const Win32InputDataBuffer*>(GetParent()->
		GetDataBuffer())->GetKey(0x53)) ? 1.0F : 0;
}

Float Win32KeyMappedAnalogPad::GetRight() const
{
	// 'd' key
	return (static_cast<const Win32InputDataBuffer*>(GetParent()->
		GetDataBuffer())->GetKey(0x44)) ? 1.0F : 0;
}
