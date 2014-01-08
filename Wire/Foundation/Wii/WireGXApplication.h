// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXAPPLICATION_H
#define WIREGXAPPLICATION_H

#include "WireApplication.h"
#include "WireColorRGBA.h"

namespace Wire
{

class GXApplication : public Application
{

public:
	GXApplication(const ColorRGBA& rBackgroundColor = ColorRGBA::BLACK,
		const Char* pWindowTitle = NULL, Int xPosition = 0, Int yPosition = 0,
		UInt width = 640, UInt height = 480, Bool isFullscreen = true,
		Bool useVSync = true);

	virtual ~GXApplication();

private:
	// Entry point to be implemented by the application. The return value is
	// an exit code, if desired.
	virtual Int Main(Int argumentQuantity, Char* arguments[]);
};

}

#endif
