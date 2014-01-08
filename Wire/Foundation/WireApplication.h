// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREAPPLICATION_H
#define WIREAPPLICATION_H

#include "WireEngine.h"
#include "WireInputSystem.h"
#include "WireFoundation.h"
#include "WireRenderer.h"

namespace Wire
{

class Application
{
public:
	// abstract base class
	virtual ~Application();

	virtual Int Main(Int argumentQuantity, Char* arguments[]) = 0;

 	// Called before the creation and initialization of the renderer.
 	// Returning 'false' quits the application.
 	virtual Bool OnPrecreate();

	// Called before main loop. Returning 'false' quits the application.
	virtual Bool OnInitialize();

	// Called by the main loop
	virtual void OnIdle();

	// Called before terminating the application.
	virtual void OnTerminate();

	// Called when the window is resized.
	virtual void OnResize(UInt width, UInt height);

	// Called by the main loop just after input capturing.
	virtual void OnInput();

	// Called when input devices are connected/disconnected.
	virtual void OnInputDevicesChange();

	virtual void Close();

	static void SetApplication(Application* pApplication);
	static Application* GetApplication(); 

	inline Renderer* GetRenderer() const;
	inline InputSystem* GetInputSystem() const;

	inline UInt GetWidth() const;
	inline UInt GetHeight() const;
	inline Float GetWidthF() const;
	inline Float GetHeightF() const;

protected:
	Application(const ColorRGBA& rBackgroundColor, const Char* pWindowTitle,
		Int xPosition, Int yPosition, UInt width, UInt height,
		Bool isFullscreen, Bool useVSync);

	// the unique application object
	static Application* s_pApplication;

	Renderer* mpRenderer;
	InputSystem* mpInputSystem;

	ColorRGBA mBackgroundColor;
	const Char* mpWindowTitle;
	Int mXPosition;
	Int mYPosition;
	UInt mWidth;
	UInt mHeight;
	Bool mIsFullscreen;
	Bool mUseVSync;
	Bool mIsRunning;
};

#include "WireApplication.inl"

}

//----------------------------------------------------------------------------
#ifdef WIRE_WIN
#include "Dx9/WireDx9Application.h"
#define WIREAPPLICATION Wire::Dx9Application
#else
#include "Wii/WireGXApplication.h"
#define WIREAPPLICATION Wire::GXApplication
#endif

#endif
