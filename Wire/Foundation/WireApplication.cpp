// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireApplication.h"
#include "WireMain.h"

using namespace Wire;

Application* Application::s_pApplication = NULL;

//----------------------------------------------------------------------------
Application::Application(const ColorRGBA& rBackgroundColor, const Char*
	pWindowTitle, Int xPosition, Int yPosition, UInt width, UInt height,
	Bool isFullscreen, Bool useVSync)
	:
	mpRenderer(NULL),
	mpInputSystem(NULL),
	mBackgroundColor(rBackgroundColor),
	mpWindowTitle(pWindowTitle),
	mXPosition(xPosition),
	mYPosition(yPosition),
	mWidth(width),
	mHeight(height),
	mIsFullscreen(isFullscreen),
	mUseVSync(useVSync),
	mIsRunning(false)
{
}

//----------------------------------------------------------------------------
Application::~Application()
{
}

//----------------------------------------------------------------------------
void Application::SetApplication(Application* pApplication)
{
	s_pApplication = pApplication;
}

//----------------------------------------------------------------------------
Application* Application::GetApplication()
{
	return s_pApplication;
}

//----------------------------------------------------------------------------
Bool Application::OnPrecreate()
{
	return true;
}

//----------------------------------------------------------------------------
Bool Application::OnInitialize()
{
	return true;
}

//----------------------------------------------------------------------------
void Application::OnIdle()
{
}

//----------------------------------------------------------------------------
void Application::OnTerminate()
{
}

//----------------------------------------------------------------------------
void Application::OnInput()
{
	// default handling exits Application when HOME button/ESC key is pressed
	if (!mpInputSystem)
	{
		return;
	}

	if (mpInputSystem->GetMainDevicesCount() == 0)
	{
		return;
	}

	const MainInputDevice* pInputDevice = mpInputSystem->GetMainDevice(0);

	// checking for minimum capabilities
	if (!pInputDevice->HasCapability(Buttons::TYPE, true))
	{
		return;
	}

	const Buttons* pButtons = DynamicCast<Buttons>(pInputDevice->
		GetCapability(Buttons::TYPE, false));
	WIRE_ASSERT(pButtons);
	if (pButtons->GetButton(Buttons::BUTTON_HOME))
	{
		Close();
		return;
	}
}

//----------------------------------------------------------------------------
void Application::OnInputDevicesChange()
{
}

//----------------------------------------------------------------------------
void Application::Close()
{
	mIsRunning = false;
}

//----------------------------------------------------------------------------
void Application::OnResize(UInt width, UInt height)
{
	if (mpRenderer)
	{
		height = (height == 0) ? 1 : height;
		mpRenderer->Resize(width, height);
		mWidth = width;
		mHeight = height;
	}
}
