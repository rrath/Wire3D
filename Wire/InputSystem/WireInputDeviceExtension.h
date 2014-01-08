// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREINPUTDEVICEEXTENSION_H
#define WIREINPUTDEVICEEXTENSION_H

#include "WireInputDevice.h"
#include "WireMainInputDevice.h"

namespace Wire
{

class InputDeviceExtension : public InputDevice
{
	WIRE_DECLARE_RTTI;

public:
	InputDeviceExtension(MainInputDevice* pMainInputDevice);
	virtual ~InputDeviceExtension();

protected:
	const MainInputDevice* GetMainInputDevice() const;

private:
	MainInputDevice* mpMainInputDevice;
};

typedef Pointer<InputDeviceExtension> InputDeviceExtensionPtr;

}

#endif
