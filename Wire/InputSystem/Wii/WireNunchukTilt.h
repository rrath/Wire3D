// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENUNCHUKTILT_H
#define WIRENUNCHUKTILT_H

#include "WireTilt.h"

namespace Wire
{

class NunchukTilt : public Tilt
{
	WIRE_DECLARE_RTTI;

public:
	NunchukTilt(const InputDevice* pParent);
	virtual ~NunchukTilt();

	virtual Float GetBackward() const;
	virtual Float GetForward() const;
	virtual Float GetLeft() const;
	virtual Float GetRight() const;

};

}

#endif
