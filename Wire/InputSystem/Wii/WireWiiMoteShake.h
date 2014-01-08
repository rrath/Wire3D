// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIIMOTESHAKE_H
#define WIREWIIMOTESHAKE_H

#include "WireShake.h"

namespace Wire
{

class WiiMoteShake : public Shake
{
	WIRE_DECLARE_RTTI;

public:
	WiiMoteShake(const InputDevice* pParent);
	virtual ~WiiMoteShake();

	virtual Float GetX() const;
	virtual Float GetY() const;
	virtual Float GetZ() const;

private:
	WPADData* GetDataFromParent() const;

};

}

#endif
