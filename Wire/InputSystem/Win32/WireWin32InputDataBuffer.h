// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIN32INPUTDATABUFFER_H
#define WIREWIN32INPUTDATABUFFER_H

#include "WireInputDataBuffer.h"
#include "WireTypes.h"

namespace Wire
{

class Win32InputDataBuffer : public InputDataBuffer
{

public:
	Win32InputDataBuffer();
	virtual ~Win32InputDataBuffer();

	Bool GetKey(UInt key) const;
	Float GetMouseX() const;
	Float GetMouseY() const;
	Float GetMouseWheel() const;
	void SetKeyDown(UInt key);
	void SetKeyUp(UInt key);
	void SetMouseX(Float mouseX);
	void SetMouseY(Float mouseY);
	void CopyFrom(const Win32InputDataBuffer* pOther);
	void IncrementMouseWheel(Float delta);
private:
	Float mMouseX;
	Float mMouseY;
	Float mMouseWheel;
	Bool* mpKeys;

};

}

#endif
