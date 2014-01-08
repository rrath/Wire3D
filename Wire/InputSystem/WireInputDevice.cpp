// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireInputDevice.h"
#include "WireInputCapability.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, InputDevice, Object);

InputDevice::InputDevice() :
	mCapabilities(8, 8)
{
}

InputDevice::~InputDevice()
{
}

const TArray<InputCapabilityPtr>& InputDevice::GetCapabilities() const
{
	return mCapabilities;
}

Bool InputDevice::HasCapability(const Rtti& rCapabilityType) const
{
	return GetCapability(rCapabilityType) != NULL;
}

const InputDataBuffer* InputDevice::GetDataBuffer() const
{
	return mpDataBuffer;
}

void InputDevice::SetDataBuffer(const InputDataBuffer* pDataBuffer)
{
	mpDataBuffer = pDataBuffer;
}

const InputCapability* InputDevice::GetCapability(const Rtti& rCapabilityType) const
{
	for (UInt i = 0; i < mCapabilities.GetQuantity(); i++)
	{
		if (mCapabilities[i]->IsDerived(rCapabilityType))
		{
			return mCapabilities[i];
		}
	}

	return NULL;
}

void InputDevice::RegisterCapability(InputCapability* pInputCapability)
{
	mCapabilities.Append(pInputCapability);
}
