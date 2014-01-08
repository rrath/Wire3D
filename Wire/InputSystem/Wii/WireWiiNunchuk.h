// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIINUNCHUK_H
#define WIREWIINUNCHUK_H

#include "WireNunchuk.h"

namespace Wire
{

class WiiNunchuk : public Nunchuk
{
	WIRE_DECLARE_RTTI;

public:
	WiiNunchuk(MainInputDevice* pMainInputDevice);
	virtual ~WiiNunchuk();

	virtual void SetUp();
};

}

#endif
