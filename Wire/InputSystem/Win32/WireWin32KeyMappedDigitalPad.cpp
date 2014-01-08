// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32KeyMappedDigitalPad.h"
#include "WireWin32InputDataBuffer.h"
#include <Windows.h>

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32KeyMappedDigitalPad, DigitalPad);

Win32KeyMappedDigitalPad::Win32KeyMappedDigitalPad(const InputDevice* pParent)
	: DigitalPad(pParent)
{
}

Win32KeyMappedDigitalPad::~Win32KeyMappedDigitalPad()
{
}

Bool Win32KeyMappedDigitalPad::GetDown() const
{
	return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_DOWN);
}

Bool Win32KeyMappedDigitalPad::GetLeft() const
{
	return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_LEFT);
}

Bool Win32KeyMappedDigitalPad::GetRight() const
{
	return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_RIGHT);
}

Bool Win32KeyMappedDigitalPad::GetUp() const
{
	return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_UP);
}
