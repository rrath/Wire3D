// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREINPUTDEVICE_H
#define WIREINPUTDEVICE_H

#include "WireInputDataBuffer.h"
#include "WireObject.h"
#include "WireTArray.h"

namespace Wire
{

class InputCapability;

class InputDevice : public Object
{
	WIRE_DECLARE_RTTI;

public:
	InputDevice();
	virtual ~InputDevice();

	const InputDataBuffer* GetDataBuffer() const;
	virtual void SetDataBuffer(const InputDataBuffer* pInputData);

	Bool HasCapability(const Rtti& rCapabilityType) const;
	const TArray<Pointer<InputCapability> >& GetCapabilities() const;
	const InputCapability* GetCapability(const Rtti& rCapabilityType) const;
	virtual void SetUp() = 0;

protected:
	void RegisterCapability(InputCapability* pInputCapability);

	TArray<Pointer<InputCapability> > mCapabilities;

private:
	const InputDataBuffer* mpDataBuffer;
};

typedef Pointer<InputDevice> InputDevicePtr;

}

#endif
