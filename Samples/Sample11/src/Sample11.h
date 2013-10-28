#pragma once
#ifndef SAMPLE11_H
#define SAMPLE11_H

#include "WireApplication.h"
#include "Cursors.h"

namespace Wire
{

class Sample11 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	Sample11();

	virtual Bool OnInitialize();
	virtual void OnTerminate();
	virtual void OnIdle();
	virtual void OnInput();
	virtual void OnInputDevicesChange();
	virtual void OnResize(UInt width, UInt height);

private:
	void UpdateInputDevicesInformationText();

	Culler mCuller;
	CameraPtr mspGuiCamera;
	NodePtr mspGuiRoot;
	NodePtr mspTextNode;

	Cursors* mpCursors;
};

WIRE_REGISTER_INITIALIZE(Sample11);

}

#endif
