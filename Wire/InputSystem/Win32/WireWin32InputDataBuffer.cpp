// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32InputDataBuffer.h"

#include "WireMath.h"
#include "WireSystem.h"

using namespace Wire;

Win32InputDataBuffer::Win32InputDataBuffer()
	: mMouseX(MathF::MAX_REAL), mMouseY(MathF::MAX_REAL), mMouseWheel(0)
{
	const UInt keysCount = 256;
	mpKeys = WIRE_NEW Bool[keysCount];
	System::Memset(mpKeys, 0, sizeof(Bool)*keysCount);
}

Win32InputDataBuffer::~Win32InputDataBuffer()
{
	WIRE_DELETE[] mpKeys;
}

Bool Win32InputDataBuffer::GetKey(UInt key) const
{
	return mpKeys[key];
}

Float Win32InputDataBuffer::GetMouseX() const
{
	return mMouseX;
}

Float Win32InputDataBuffer::GetMouseY() const
{
	return mMouseY;
}

Float Win32InputDataBuffer::GetMouseWheel() const
{
	return mMouseWheel;
}

void Win32InputDataBuffer::SetKeyDown(UInt key)
{
	mpKeys[key] = true;
}

void Win32InputDataBuffer::SetKeyUp(UInt key)
{
	mpKeys[key] = false;
}

void Win32InputDataBuffer::SetMouseX(Float mouseX)
{
	mMouseX = mouseX;
}

void Win32InputDataBuffer::SetMouseY(Float mouseY)
{
	mMouseY = mouseY;
}

void Win32InputDataBuffer::CopyFrom(const Win32InputDataBuffer* pOther)
{
	mMouseX = pOther->mMouseX;
	mMouseY = pOther->mMouseY;
	mMouseWheel = pOther->mMouseWheel;
	System::Memcpy(mpKeys, 256, pOther->mpKeys, 256);
}

void Win32InputDataBuffer::IncrementMouseWheel(Float delta)
{
	mMouseWheel += delta;
}
