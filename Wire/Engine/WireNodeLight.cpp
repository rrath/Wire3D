// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNodeLight.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NodeLight, Node);

//----------------------------------------------------------------------------
NodeLight::NodeLight(Light* pLight)
	:
	mspLight(pLight)
{
	LightToLocalTransform();
}

//----------------------------------------------------------------------------
NodeLight::~NodeLight()
{
}

//----------------------------------------------------------------------------
void NodeLight::Set(Light* pLight)
{
	mspLight = pLight;

	LightToLocalTransform();
}

//----------------------------------------------------------------------------
void NodeLight::UpdateWorldData(Double appTime, Bool updateControllers)
{
	Node::UpdateWorldData(appTime, updateControllers);

	if (mspLight && mspLight->Enabled)
	{
		mspLight->Position = World.GetTranslate();
		mspLight->Direction = World.GetMatrix().GetColumn(0);
	}
}

//----------------------------------------------------------------------------
void NodeLight::LightToLocalTransform()
{
	if (mspLight)
	{
		Vector3F u;
		Vector3F v;
		Vector3F::GenerateOrthonormalBasis(u, v, mspLight->Direction);
		Matrix34F matrix(mspLight->Direction, u, v, mspLight->Position);
		Local.SetMatrix(matrix, false);
	}
}
