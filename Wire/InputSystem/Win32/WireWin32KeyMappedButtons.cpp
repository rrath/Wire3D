// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWin32KeyMappedButtons.h"
#include "WireWin32InputDataBuffer.h"
#include <Windows.h>

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Win32KeyMappedButtons, Buttons);

Win32KeyMappedButtons::Win32KeyMappedButtons(const InputDevice* pParent)
	: Buttons(pParent)
{
}

Win32KeyMappedButtons::~Win32KeyMappedButtons()
{
}

Bool Win32KeyMappedButtons::GetButton(Button button) const
{
	switch (button)
	{
	case BUTTON_A:
		// left mouse button
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_LBUTTON);
	case BUTTON_B:
		// right mouse button
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_RBUTTON);
	case BUTTON_C:
		// 'c' key
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(0x43);
	case BUTTON_HOME:
		// 'esc' key
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_ESCAPE);
	case BUTTON_MINUS:
		// '-' key
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_OEM_MINUS);
	case BUTTON_PLUS:
		// '+' key
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_OEM_PLUS);
	case BUTTON_Z:
		// 'z' key
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(VK_SHIFT);
	case BUTTON_1:
		// '1' key
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(0x31);
	case BUTTON_2:
		// '2' key
		return static_cast<const Win32InputDataBuffer*>(GetParent()->GetDataBuffer())->GetKey(0x32);
	}

	return false;
}
