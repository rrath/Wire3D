// Sample11 - Input
// This sample demonstrates how to handle input and input devices.

#include "Sample11.h"

using namespace Wire;

WIRE_APPLICATION(Sample11);

//----------------------------------------------------------------------------
Sample11::Sample11()
	:
	mpCursors(NULL)
{
}

//----------------------------------------------------------------------------
Bool Sample11::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	mpCursors = WIRE_NEW Cursors;
	mspGuiRoot = WIRE_NEW Node;
	mspGuiRoot->AttachChild(mpCursors->GetRoot());
	mspTextNode = WIRE_NEW Node(StandardMesh::CreateText());
	mspGuiRoot->AttachChild(mspTextNode);

	mspGuiCamera = WIRE_NEW Camera(/* isPerspective */false);
	mCuller.SetCamera(mspGuiCamera);

	UpdateInputDevicesInformationText();
	return true;
}

//----------------------------------------------------------------------------
void Sample11::OnTerminate()
{
	if (mpCursors)
	{
		WIRE_DELETE mpCursors;
		mpCursors = NULL;
	}
}

//----------------------------------------------------------------------------
void Sample11::OnIdle()
{
	Double time = System::GetTime();

	// set the frustum for the text camera (screenWidth and screenHeight
	// could have been changed by the user resizing the window)
	mspGuiCamera->SetFrustum(0, GetWidthF(), 0, GetHeightF(), 0, 1);

	mspGuiRoot->UpdateGS(time);
	mCuller.ComputeVisibleSet(mspGuiRoot);

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspGuiCamera);
	GetRenderer()->Draw(mCuller.GetVisibleSets());
	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
void Sample11::OnInput()
{
	// Override base class to change default behavior.
	// InputSystem can be accessed anywhere.

	UInt playerCount = GetInputSystem()->GetMainDevicesCount();
	playerCount = playerCount > 4 ? 4 : playerCount; // maximum of 4 players

	// if there's no controller connected yet, return
	if (playerCount == 0) 
	{
		return;
	}

	// check if the controller device has buttons
	const Buttons* pButtons = DynamicCast<Buttons>(GetInputSystem()->
		GetMainDevice(0)->GetCapability(Buttons::TYPE, false));
	if (!pButtons)
	{
		return;
	}

	// close the application if the home button is pressed
	if (pButtons->GetButton(Buttons::BUTTON_HOME))
	{
		Parent::Close();
		return;
	}

	for (UInt playerIndex = 0; playerIndex < playerCount; playerIndex++) 
	{
		const MainInputDevice* pInputDevice = GetInputSystem()->GetMainDevice(
			playerIndex);

		// if there's no IR, position the pointer at the center of the screen
		Float x = GetWidthF() * 0.5F;
		Float y = GetHeightF() * 0.5F;

		const IR* pIR = DynamicCast<const IR>(pInputDevice->GetCapability(
			IR::TYPE, false));
		if (pIR)
		{
			// Wire3D uses the OpenGL convention of (0,0) being at the bottom
			// left corner of the screen, but we want the upper left corner
			// so make a vertical correction here
			x = pIR->GetRight();
			y = GetHeightF() - pIR->GetUp();
		}
		
		Cursors::CursorMode mode = Cursors::CM_POINTING;

		// if there are buttons in the main device, get them
		const Buttons* pButtons = DynamicCast<const Buttons>(pInputDevice->
			GetCapability(Buttons::TYPE, false));

		// if there are buttons and the 'A' (win32: left mouse button) button
		// is pressed, change the cursor mode
		if (pButtons && pButtons->GetButton(Buttons::BUTTON_A))
		{
			mode = Cursors::CM_PRIMARY_BUTTON_PRESSED;
		}

		// same for 'B' button (win32: right mouse button)
		if (pButtons && pButtons->GetButton(Buttons::BUTTON_B))
		{
			mode = Cursors::CM_SECONDARY_BUTTON_PRESSED;
		}

		Float tilt = 0;
		// get the main device tilt (win32: mouse wheel) (in degrees)
		const Tilt* pTilt = DynamicCast<const Tilt>(pInputDevice->
			GetCapability(Tilt::TYPE, false));
		if (pTilt)
		{
			tilt = MathF::DEG_TO_RAD * (pTilt->GetRight());
		}

		mpCursors->SetCursor(x, y, mode, playerIndex+1, tilt);
	}
}

//----------------------------------------------------------------------------
void Sample11::OnInputDevicesChange()
{
	// new input devices found or existing ones disconnected
	UpdateInputDevicesInformationText();
}

//----------------------------------------------------------------------------
void Sample11::OnResize(UInt width, UInt height)
{
	Parent::OnResize(width, height);

	// window size changed, re-layout the text accordingly
	UpdateInputDevicesInformationText();
}

//----------------------------------------------------------------------------
void Sample11::UpdateInputDevicesInformationText()
{
	WIRE_ASSERT(DynamicCast<RenderText>(mspTextNode->GetRenderObject()));
	RenderText* pText = StaticCast<RenderText>(mspTextNode->GetRenderObject());
	if (!pText)
	{
		return;
	}

	// set to screen width (might change any time in window mode)
	pText->SetLineWidth(GetWidthF());
	pText->Clear();
	// Text uses OpenGL convention of (0,0) being left bottom of window
	pText->SetPen(0, GetHeightF()-pText->GetFontHeight()-32.0F);

	const UInt TextArraySize = 4000;
	Char text[TextArraySize];
	System::Sprintf(text, TextArraySize, "Number of Input Devices: %d\n",
		GetInputSystem()->GetDevicesCount());
	pText->Append(text);

	for (UInt i = 0; i < GetInputSystem()->GetDevicesCount(); i++)
	{
		const InputDevice* pInputDevice = GetInputSystem()->GetDevice(i);
		const Rtti& rType = pInputDevice->GetType();
		
		System::Sprintf(text, TextArraySize, "\nInput Device Number: %d - "
			"%s (%s)\n- Capabilities:\n", i, rType.GetBaseType()->GetName(),
			rType.GetName());
		pText->Append(text);

		const TArray<InputCapabilityPtr>& inputCapabilities = pInputDevice->
			GetCapabilities();
		for (UInt j = 0; j < inputCapabilities.GetQuantity(); j++)
		{
			const Rtti& rType = inputCapabilities[j]->GetType();

			System::Sprintf(text, TextArraySize, "%s (%s)\n",
				rType.GetBaseType()->GetName(), rType.GetName());
			pText->Append(text);
		}
	}

	pText->Update(GetRenderer());
}
