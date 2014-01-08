// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireGXApplication.h"
#include "WireGXRendererInput.h"
#include "WireRenderer.h"
#include "WireGXRendererData.h"
#include "WireWiiInputSystem.h"
#include <fat.h>

using namespace Wire;

//----------------------------------------------------------------------------
GXApplication::GXApplication(const ColorRGBA& rBackgroundColor, const Char*
	pWindowTitle, Int xPosition, Int yPosition, UInt width, UInt height,
	Bool, Bool useVSync)
	:
	Application(rBackgroundColor, pWindowTitle, xPosition, yPosition, width,
		height, true, useVSync)
{
}

//----------------------------------------------------------------------------
GXApplication::~GXApplication()
{
	WIRE_DELETE mpInputSystem;
	mpInputSystem = NULL;

	WIRE_DELETE mpRenderer;
	mpRenderer = NULL;
}

//----------------------------------------------------------------------------
Int GXApplication::Main(Int argumentQuantity, Char* arguments[])
{
	fatInitDefault();

	// allow work to be done before the renderer is created
	if (!s_pApplication->OnPrecreate())
	{
		return -1;
	}

	PdrRendererInput input;
	input.BackgroundColor = mBackgroundColor;
	mpRenderer = WIRE_NEW Renderer(input, 0, 0, mIsFullscreen, mUseVSync);

	mpInputSystem = WIRE_NEW WiiInputSystem;

	if (s_pApplication->OnInitialize())
	{
		// first attempt to discover input devices
		if (mpInputSystem->DiscoverDevices())
		{
			s_pApplication->OnInputDevicesChange();
		}

		mIsRunning = true;
		while (mIsRunning) 
		{
			mpInputSystem->Capture();
			s_pApplication->OnInput();

			s_pApplication->OnIdle();

			// new attempt to discover input devices
			if (mpInputSystem->DiscoverDevices())
			{
				s_pApplication->OnInputDevicesChange();
			}
		}
	}

	s_pApplication->OnTerminate();

	return 0;
}
