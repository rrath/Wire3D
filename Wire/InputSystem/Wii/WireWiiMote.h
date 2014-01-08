// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIIMOTE_H
#define WIREWIIMOTE_H

#include "WireMainInputDevice.h"

namespace Wire
{

class WiiMote : public MainInputDevice
{
	WIRE_DECLARE_RTTI;

public:
	WiiMote(Int channel);
	virtual ~WiiMote();

	UInt GetChannel() const;
	virtual void SetUp();

private:
	UInt mChannel;

};

}

#endif
