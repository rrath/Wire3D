// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIIMOTESWING_H
#define WIREWIIMOTESWING_H

#include "WireSwing.h"

namespace Wire
{

class WiiMoteSwing : public Swing
{
	WIRE_DECLARE_RTTI;

public:
	WiiMoteSwing(const InputDevice* pParent);
	virtual ~WiiMoteSwing();

	virtual Float GetBackward() const;
	virtual Float GetDown() const;
	virtual Float GetForward() const;
	virtual Float GetLeft() const;
	virtual Float GetRight() const;
	virtual Float GetUp() const;

};

}

#endif
