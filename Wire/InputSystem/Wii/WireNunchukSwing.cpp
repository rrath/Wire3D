// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNunchukSwing.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NunchukSwing, Swing);

NunchukSwing::NunchukSwing(const InputDevice* pParent)
	: Swing(pParent)
{
}

NunchukSwing::~NunchukSwing()
{
}

Float NunchukSwing::GetLeft() const
{
	return 0;
}

Float NunchukSwing::GetRight() const
{
	return 0;
}

Float NunchukSwing::GetUp() const
{
	return 0;
}

Float NunchukSwing::GetDown() const
{
	return 0;
}

Float NunchukSwing::GetForward() const
{
	return 0;
}

Float NunchukSwing::GetBackward() const
{
	return 0;
}
