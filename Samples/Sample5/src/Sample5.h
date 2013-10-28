#pragma once
#ifndef SAMPLE5_H
#define SAMPLE5_H

#include "WireApplication.h"

namespace Wire
{

class Sample5 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	virtual Bool OnInitialize();
	virtual void OnIdle();

private:
	Node* CreateCube(Bool useTexture = true, Bool useNormals = true,
		Bool useVertexColor = false, ColorRGBA vertexColor = ColorRGBA::WHITE);
	RenderObject* CreatePlane();

	Texture2D* CreateTexture();

	CameraPtr mspCamera;
	Culler mCuller;

	NodePtr mspRoot;
	RenderObjectPtr mspPlane;
	RenderObjectPtr mspWhiteCube;
	Texture2DPtr mspTexture;
	LightPtr mspSpotLight;

	Float mAngle;
	Double mLastTime;
};

WIRE_REGISTER_INITIALIZE(Sample5);

}

#endif
