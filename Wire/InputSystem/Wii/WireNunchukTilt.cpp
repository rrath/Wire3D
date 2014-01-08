// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNunchukTilt.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NunchukTilt, Tilt);

NunchukTilt::NunchukTilt(const InputDevice* pParent)
	: Tilt(pParent)
{
}

NunchukTilt::~NunchukTilt()
{
}

Float NunchukTilt::GetBackward() const
{
	return 0;
}

Float NunchukTilt::GetForward() const
{
	return 0;
}

Float NunchukTilt::GetLeft() const
{
	return 0;
}

Float NunchukTilt::GetRight() const
{
	return 0;
}
