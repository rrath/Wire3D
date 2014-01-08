// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREDX9APPLICATION_H
#define WIREDX9APPLICATION_H

#include "WireApplication.h"
#include "WireColorRGBA.h"

namespace Wire
{

class Dx9Application : public Application
{

public:
	Dx9Application(const ColorRGBA& rBackgroundColor = ColorRGBA::BLACK,
		const Char* windowTitle = NULL, Int xPosition = 0, Int yPosition = 0,
		UInt width = 640, UInt height = 480, Bool isFullscreen = false,
		Bool useVSync = true);
	virtual ~Dx9Application();

	Int GetWindowID() const;

private:
	// Entry point to be implemented by the application. The return value is
	// an exit code, if desired.
	virtual Int Main(Int argumentQuantity, Char* arguments[]);

	// An identifier for the window (representation is platform-specific).
	Int mWindowID;
};

}

#endif
