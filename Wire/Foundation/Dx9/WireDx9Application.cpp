// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireDx9Application.h"
#include "WireRenderer.h"
#include "WireDx9RendererInput.h"
#include "WireWin32InputSystem.h"
#include <Windows.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")  // only used for D3DXCompileShader

using namespace Wire;

// These are in the VC7.1 basetsd.h, but not the VC6 basetsd.h.
#ifndef PtrToInt
#define PtrToInt(p)  ((INT)(INT_PTR)(p))
#endif

//----------------------------------------------------------------------------
Dx9Application::Dx9Application(const ColorRGBA& rBackgroundColor,
	const Char* pWindowTitle, Int xPosition, Int yPosition, UInt width,
	UInt height, Bool isFullscreen, Bool useVSync)
	:
	Application(rBackgroundColor, pWindowTitle, xPosition, yPosition, width,
		height, isFullscreen, useVSync),
	mWindowID(0)
{
	_set_error_mode(_OUT_TO_MSGBOX);

	DEVMODE mode;
	mode.dmSize = sizeof(DEVMODE);
	mode.dmDriverExtra = 0;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);
	if (width == 0 && height == 0)
	{
		mWidth = mode.dmPelsWidth;
		mHeight = mode.dmPelsHeight;
	}
}

//----------------------------------------------------------------------------
Dx9Application::~Dx9Application()
{
	WIRE_DELETE mpInputSystem;
	mpInputSystem = NULL;

	WIRE_DELETE mpRenderer;
	mpRenderer = NULL;
}

//----------------------------------------------------------------------------
LRESULT CALLBACK WireMsWindowEventHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Dx9Application* pApp = static_cast<Dx9Application*>(Application::GetApplication());

	if (!pApp || !pApp->GetWindowID())
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	Win32InputSystem* pInputSystem = static_cast<Win32InputSystem*>(pApp->GetInputSystem());

	// It this call returns true it means that the given system message was handled by the broker.
	if (pInputSystem->OnSystemMessage(msg, static_cast<UInt>(wParam), static_cast<Long>(lParam), hWnd))
	{
		return 0;
	}

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		if (pApp->GetRenderer())
		{
			pApp->OnIdle();
		}

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_SIZE:
	{
		if (wParam != SIZE_MINIMIZED)
		{
			UInt width = static_cast<UInt>(LOWORD(lParam));
			UInt height = static_cast<UInt>(HIWORD(lParam));
			pApp->OnResize(width, height);
		}

		return 0;
	}

	case WM_ERASEBKGND:
	{
		// This tells Windows not to erase the background (and that the
		// application is doing so).
		return 1;
	}

	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//----------------------------------------------------------------------------
Int Dx9Application::GetWindowID() const
{
	return mWindowID;
}

//----------------------------------------------------------------------------
Int Dx9Application::Main(Int, Char*[])
{
	// allow work to be done before the window and renderer is created
	if (!s_pApplication->OnPrecreate())
	{
		return -1;
	}

	// register the window class
	static Char s_WindowClass[] = "Wire Application";
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WireMsWindowEventHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = 0;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = s_WindowClass;
	wc.lpszMenuName = 0;
	RegisterClass(&wc);

	// require the window to have the specified client area
	RECT rect =
	{ 0, 0, mWidth - 1, mHeight - 1 };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	// create the application window
	HWND hWnd = CreateWindow(s_WindowClass, mpWindowTitle,
			WS_OVERLAPPEDWINDOW, mXPosition, mYPosition,
			rect.right-rect.left+1, rect.bottom-rect.top+1, 0, 0, 0, 0);

	mWindowID = PtrToInt(hWnd);

	// hide mouse pointer
	UChar and[1] = { 0xFF };
	UChar xor[1] = { 0x00 };
	HCURSOR hCursor = CreateCursor(0, 0, 0, 1, 1, &and, &xor);
	SetClassLongPtr(hWnd, GCLP_HCURSOR, LONG_PTR(hCursor));
	SetCursor(hCursor);

	PdrRendererInput input;
	input.WindowHandle = hWnd;

	mpRenderer = WIRE_NEW Renderer(input, mWidth, mHeight, mIsFullscreen, mUseVSync);
	mpRenderer->SetClearColor(mBackgroundColor);

	mpInputSystem = WIRE_NEW Win32InputSystem;

	if (s_pApplication->OnInitialize())
	{
		// display the window
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);

		// first attempt to discover input devices
		if (mpInputSystem->DiscoverDevices())
		{
			s_pApplication->OnInputDevicesChange();
		}

		mIsRunning = true;
		// start the message pump
		while (mIsRunning)
		{
			MSG msg;
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					mIsRunning = false;
					continue;
				}

				HACCEL hAccel = 0;
				if (!TranslateAccelerator(hWnd, hAccel, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
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
	}

	s_pApplication->OnTerminate();

	return 0;
}
