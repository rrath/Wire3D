// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIN32KEYMAPPEDTILT_H
#define WIREWIN32KEYMAPPEDTILT_H

#include "WireTilt.h"

namespace Wire
{

class Win32KeyMappedTilt : public Tilt
{
	WIRE_DECLARE_RTTI;

public:
	Win32KeyMappedTilt(const InputDevice* pParent);
	virtual ~Win32KeyMappedTilt();

	virtual Float GetBackward() const;
	virtual Float GetForward() const;
	virtual Float GetLeft() const;
	virtual Float GetRight() const;

};

}

#endif
