// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32KeyMappedIR.h"
#include "WireWin32InputDataBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32KeyMappedIR, IR);

Win32KeyMappedIR::Win32KeyMappedIR(const InputDevice* pParent)
	: IR(pParent)
{
}

Win32KeyMappedIR::~Win32KeyMappedIR()
{
}

Float Win32KeyMappedIR::GetBackward() const
{
	return 0;
}

Float Win32KeyMappedIR::GetDown() const
{
	return -static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetMouseY();
}

Float Win32KeyMappedIR::GetForward() const
{
	return 0;
}

Float Win32KeyMappedIR::GetLeft() const
{
	return -static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetMouseX();
}

Float Win32KeyMappedIR::GetRight() const
{
	return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetMouseX();
}

Float Win32KeyMappedIR::GetUp() const
{
	return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetMouseY();
}
