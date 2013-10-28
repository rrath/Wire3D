#pragma once
#ifndef SAMPLE3_H
#define SAMPLE3_H

#include "WireApplication.h"

namespace Wire
{

class Sample3 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	virtual Bool OnInitialize();
	virtual void OnIdle();

private:
	NodeDLod* CreateLods();
	Spatial* CreateLod(UInt shapeCount, UInt segmentCount);

	RenderObject* CreatePqTorusKnot(UInt shapeCount, Float shapeRadius,
		UInt segmentCount, UInt p, UInt q);
	Texture2D* CreateTexture();

	void DrawLodTextLabel();

	CameraPtr mspCamera;
	Culler mCuller;

	// camera for GUI/text
	CameraPtr mspOrthographic;
	RenderTextPtr mspText;

	Float mAngle;
	Double mLastTime;

	NodePtr mspRoot;
	Texture2DPtr mspTexture;
};

WIRE_REGISTER_INITIALIZE(Sample3);

}

#endif
