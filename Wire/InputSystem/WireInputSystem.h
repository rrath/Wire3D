// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREINPUTSYSTEM_H
#define WIREINPUTSYSTEM_H

#include "WireAnalogPad.h"
#include "WireButtons.h"
#include "WireDigitalPad.h"
#include "WireInputDeviceExtension.h"
#include "WireIR.h"
#include "WireMainInputDevice.h"
#include "WireNunchuk.h"
#include "WireShake.h"
#include "WireSwing.h"
#include "WireTilt.h"

namespace Wire
{

class InputSystem
{

public:
	InputSystem();
	virtual ~InputSystem();

	virtual void Capture() = 0;
	virtual Bool DiscoverDevices() = 0;

	const InputDeviceExtension* GetDeviceExtension(UInt index) const;
	UInt GetDeviceExtensionsCount() const;
	const MainInputDevice* GetMainDevice(UInt index) const;
	UInt GetMainDevicesCount() const;
	const InputDevice* GetDevice(UInt index) const;
	UInt GetDevicesCount() const;

protected:
	TArray<InputDevicePtr> mDevices;
	TArray<MainInputDevicePtr> mMainDevices;
	TArray<InputDeviceExtensionPtr> mDeviceExtensions;

	void AddDevice(InputDevice* pInputDevice);
	void RemoveDevice(InputDevice* pInputDevice);
};

}

#endif
