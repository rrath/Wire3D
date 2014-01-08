// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENUNCHUKSWING_H
#define WIRENUNCHUKSWING_H

#include "WireSwing.h"

namespace Wire
{

class NunchukSwing : public Swing
{
	WIRE_DECLARE_RTTI;

public:
	NunchukSwing(const InputDevice* pParent);
	virtual ~NunchukSwing();

	virtual Float GetBackward() const;
	virtual Float GetDown() const;
	virtual Float GetForward() const;
	virtual Float GetLeft() const;
	virtual Float GetRight() const;
	virtual Float GetUp() const;

};

}

#endif
