// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENUNCHUKANALOGPAD_H
#define WIRENUNCHUKANALOGPAD_H

#include "WireAnalogPad.h"

namespace Wire
{

class NunchukAnalogPad : public AnalogPad
{
	WIRE_DECLARE_RTTI;

public:
	NunchukAnalogPad(const InputDevice* pParent);
	virtual ~NunchukAnalogPad();

	virtual Float GetDown() const;
	virtual Float GetLeft() const;
	virtual Float GetRight() const;
	virtual Float GetUp() const;

private:

	Float GetX() const;
	Float GetY() const;
};

}

#endif
