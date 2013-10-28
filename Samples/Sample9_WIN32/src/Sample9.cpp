#include "WireFoundation.h"
#include "WireEngine.h"
#include "WireDx9RendererInput.h"

#include <Windows.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

using namespace Wire;

Int gWindowID = 0;
Renderer* gpRenderer = NULL;
RenderObjectPtr gspCube;
CameraPtr gspCamera;

//----------------------------------------------------------------------------
void OnIdle()
{
	// the render loop
	Transformation transformation;
	Matrix3F rotate(Vector3F(0.2F, 0.7F, 0.1F), MathF::FMod(
		static_cast<Float>(System::GetTime()), MathF::TWO_PI));
	transformation.SetRotate(rotate);

	gpRenderer->ClearBuffers();
	gpRenderer->PreDraw(gspCamera);
	gpRenderer->Draw(gspCube, transformation);
	gpRenderer->PostDraw();
	gpRenderer->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
LRESULT CALLBACK MsWindowEventHandler(HWND hWnd, UINT msg, WPARAM wParam,
									   LPARAM lParam)
{
	if (!gWindowID)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch (msg) 
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			if (gpRenderer)
			{
				OnIdle();
			}

			EndPaint(hWnd, &ps);
			return 0;
		}

	case WM_CHAR:
		{
			UChar key = static_cast<UChar>(wParam);

			// quit application if the ESC key is pressed
			if (key == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}
		}

	case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
			{
				UInt width = static_cast<UInt>(LOWORD(lParam));
				UInt height = static_cast<UInt>(HIWORD(lParam));
				if (gpRenderer)
				{
					gpRenderer->Resize(width, height);
				}
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
Int main(Int argc, Char** argv)
{
	// this initializes all classes that were statically registered using
	// the WIRE_REGISTER_INITIALIZE macros.
	Main::Initialize();

	// Windows specific setup code...
	const UInt width = 640;
	const UInt height = 480;
	const Bool isFullscreen = false;
	const Bool useVSync = false;

	// register the window class
	static Char s_WindowClass[] = "Wire Application";
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = MsWindowEventHandler;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = 0;
	wc.hIcon         = LoadIcon(0,IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0,IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = s_WindowClass;
	wc.lpszMenuName  = 0;
	RegisterClass(&wc);

	// require the window to have the specified client area
	RECT rect = { 0, 0, width-1, height-1 };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	// create the application window
	HWND hWnd = CreateWindow(s_WindowClass, "Sample - Custom Main",
		WS_OVERLAPPEDWINDOW, 0, 0, rect.right-rect.left+1,
		rect.bottom-rect.top+1, 0, 0, 0, 0);

	gWindowID = PtrToInt(hWnd);

	// create the Wire renderer (pass the window handle)
	PdrRendererInput input;
	input.WindowHandle = hWnd;
	gpRenderer = WIRE_NEW Renderer(input, width, height, isFullscreen,
		useVSync);
	
	// create a cube to be rendered
	gspCube = StandardMesh::CreateCube8(/* RGB(A) channels */ 4);
	Vector3F cameraLocation(0.0F, 0.0F, 10.0F);
	Vector3F viewDirection(0.0F, 0.0F, -1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	gspCamera = WIRE_NEW Camera(cameraLocation, viewDirection, up);

	// display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// start the message pump
	Bool isApplicationRunning = true;
	while (isApplicationRunning)
	{
		MSG msg;
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isApplicationRunning = false;
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
			// render the cube
			OnIdle();
		}
	}

	// dereference (i.e. destroy) before destroying the renderer
	gspCamera = NULL;
	gspCube = NULL;

	// delete the renderer
	WIRE_DELETE gpRenderer;
	gpRenderer = NULL;
	Main::Terminate();

	return 0;
}
