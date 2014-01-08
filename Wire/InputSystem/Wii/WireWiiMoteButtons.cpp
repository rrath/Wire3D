// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWiiMoteButtons.h"

#include "WireWiiMote.h"
#include "WireWiiInputDataBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, WiiMoteButtons, Buttons);

WiiMoteButtons::WiiMoteButtons(const InputDevice* pParent)
	: Buttons(pParent)
{
}

WiiMoteButtons::~WiiMoteButtons()
{
}

Bool WiiMoteButtons::GetButton(Button button) const
{
	if (GetParent()->GetDataBuffer() == NULL)
	{
		return false;
	}

	const WPADData* pData = static_cast<const WiiInputDataBuffer*>(GetParent()->GetDataBuffer())->GetData();

	if (pData == NULL)
	{
		return false;
	}

	switch (button)
	{
	case BUTTON_A:
		return (pData->btns_h & WPAD_BUTTON_A) != 0;

	case BUTTON_B:
		return (pData->btns_h & WPAD_BUTTON_B) != 0;

	case BUTTON_1:
		return (pData->btns_h & WPAD_BUTTON_1) != 0;

	case BUTTON_2:
		return (pData->btns_h & WPAD_BUTTON_2) != 0;

	case BUTTON_MINUS:
		return (pData->btns_h & WPAD_BUTTON_MINUS) != 0;

	case BUTTON_PLUS:
		return (pData->btns_h & WPAD_BUTTON_PLUS) != 0;

	case BUTTON_HOME:
		return (pData->btns_h & WPAD_BUTTON_HOME) != 0;

	case BUTTON_Z:
	case BUTTON_C:
		return false;

	default:
		WIRE_ASSERT(false /* Unknown button */);
		return false;
	}
}
