#pragma once
#ifndef SAMPLE2_H
#define SAMPLE2_H

#include "WireApplication.h"

namespace Wire
{

class Sample2 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	Sample2();

	virtual Bool OnInitialize();
	virtual void OnIdle();

private:
	Node* CreateHelicopter();
	RenderObject* CreateCube(ColorRGBA top, ColorRGBA bottom);

	CameraPtr mspCamera;
	Culler mCuller;

	Float mAngle;
	Double mLastTime;

	NodePtr mspRoot;
	NodePtr mspTopRotor;
	NodePtr mspRearRotor;
};

WIRE_REGISTER_INITIALIZE(Sample2);

}

#endif
