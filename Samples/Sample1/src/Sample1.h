#pragma once
#ifndef SAMPLE1_H
#define SAMPLE1_H

#include "WireApplication.h"

namespace Wire
{

class Sample1 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	// OnInitialize() is called by the framework after the Renderer has been
	// created. User specific initialization code goes here.
	virtual Bool OnInitialize();

	// OnIdle() is called every frame. Here we update the objects, draw them
	// and present the result on the screen.
	virtual void OnIdle();

private:
	RenderObject* CreateCube();
	Texture2D* CreateTexture();

	CameraPtr mspCamera;
	Culler mCuller;

	RenderObjectPtr mspCube;
	BoundingVolumePtr mspWorldBound;

	LightPtr mspLight;
	StateMaterialPtr mspMaterial;

	StateCullPtr mspCull;
	StateAlphaPtr mspAlpha;

	Float mAngle;
	Double mLastTime;

};

WIRE_REGISTER_INITIALIZE(Sample1);

}

#endif
