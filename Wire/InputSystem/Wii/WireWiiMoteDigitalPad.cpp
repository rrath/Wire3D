// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWiiMoteDigitalPad.h"

#include "WireWiiInputDataBuffer.h"
#include "WireWiiMote.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, WiiMoteDigitalPad, DigitalPad);

WiiMoteDigitalPad::WiiMoteDigitalPad(const InputDevice* pParent)
	: DigitalPad(pParent)
{
}

WiiMoteDigitalPad::~WiiMoteDigitalPad()
{
}

Bool WiiMoteDigitalPad::GetDown() const
{
	const WPADData* pData = static_cast<const WiiInputDataBuffer*>(GetParent()->GetDataBuffer())->GetData();

	if (pData == NULL)
	{
		return false;
	}

	return (pData->btns_h & WPAD_BUTTON_DOWN) != 0;
}

Bool WiiMoteDigitalPad::GetLeft() const
{
	const WPADData* pData = static_cast<const WiiInputDataBuffer*>(GetParent()->GetDataBuffer())->GetData();

	if (pData == NULL)
	{
		return false;
	}

	return (pData->btns_h & WPAD_BUTTON_LEFT) != 0;
}

Bool WiiMoteDigitalPad::GetRight() const
{
	const WPADData* pData = static_cast<const WiiInputDataBuffer*>(GetParent()->GetDataBuffer())->GetData();

	if (pData == NULL)
	{
		return false;
	}

	return (pData->btns_h & WPAD_BUTTON_RIGHT) != 0;
}

Bool WiiMoteDigitalPad::GetUp() const
{
	const WPADData* pData = static_cast<const WiiInputDataBuffer*>(GetParent()->GetDataBuffer())->GetData();

	if (pData == NULL)
	{
		return false;
	}

	return (pData->btns_h & WPAD_BUTTON_UP) != 0;
}
