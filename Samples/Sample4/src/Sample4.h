#pragma once
#ifndef SAMPLE4_H
#define SAMPLE4_H

#include "WireApplication.h"

namespace Wire
{

class Sample4 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	virtual Bool OnInitialize();
	virtual void OnIdle();

private:
	struct BlendMode
	{
		BlendMode(Material::BlendMode mode0, Material::BlendMode mode1)
			:
			Mode0(mode0),
			Mode1(mode1) {}

		Material::BlendMode Mode0;
		Material::BlendMode Mode1;
	};

	Texture2D* CreateTexture();
	Texture2D* CreateTexture2();

	CameraPtr mspCamera;
	RenderObjectPtr mspCube;

	Float mAngle;
	Double mLastTime;
};

WIRE_REGISTER_INITIALIZE(Sample4);

}

#endif
