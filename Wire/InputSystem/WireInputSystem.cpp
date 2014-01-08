// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireInputSystem.h"

using namespace Wire;

InputSystem::InputSystem()
{
}

InputSystem::~InputSystem()
{
}

const InputDeviceExtension* InputSystem::GetDeviceExtension(UInt index) const
{
	return mDeviceExtensions[index];
}

UInt InputSystem::GetDeviceExtensionsCount() const
{
	return mDeviceExtensions.GetQuantity();
}

const MainInputDevice* InputSystem::GetMainDevice(UInt index) const
{
	return mMainDevices[index];
}

UInt InputSystem::GetMainDevicesCount() const
{
	return mMainDevices.GetQuantity();
}

const InputDevice* InputSystem::GetDevice(UInt index) const
{
	return mDevices[index];
}

UInt InputSystem::GetDevicesCount() const
{
	return mDevices.GetQuantity();
}

void InputSystem::AddDevice(InputDevice* pDevice)
{
	mDevices.Append(pDevice);
	
	if (pDevice->IsDerived(MainInputDevice::TYPE))
	{
		MainInputDevice* pMainDevice = DynamicCast<MainInputDevice>(pDevice);
		mMainDevices.Append(pMainDevice);
		const TArray<Pointer<InputDeviceExtension> >& rExtensions = pMainDevice->GetExtensions();
		for (UInt i = 0; i < rExtensions.GetQuantity(); i++)
		{
			AddDevice(rExtensions[i]);
		}
	}

	else if (pDevice->IsDerived(InputDeviceExtension::TYPE))
	{
		InputDeviceExtension* pExtension = DynamicCast<InputDeviceExtension>(pDevice);
		mDeviceExtensions.Append(pExtension);
	}
}

void InputSystem::RemoveDevice(InputDevice* pDevice)
{
	if (pDevice->IsDerived(MainInputDevice::TYPE))
	{
		MainInputDevice* pMainDevice = DynamicCast<MainInputDevice>(pDevice);
		mMainDevices.Remove(pMainDevice);
		const TArray<Pointer<InputDeviceExtension> >& rExtensions = pMainDevice->GetExtensions();
		for (UInt i = 0; i < rExtensions.GetQuantity(); i++)
		{
			RemoveDevice(rExtensions[i]);
		}
	}

	else if (pDevice->IsDerived(InputDeviceExtension::TYPE))
	{
		InputDeviceExtension* pExtension = DynamicCast<InputDeviceExtension>(pDevice);
		mDeviceExtensions.Remove(pExtension);
	}

	mDevices.Remove(pDevice);
}
