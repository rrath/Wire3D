// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderObject.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, RenderObject, Object);

//----------------------------------------------------------------------------
RenderObject::RenderObject(VertexBuffer* pVBuffer, IndexBuffer* pIBuffer,
	Material* pMaterial)
	:
	mspMaterial(pMaterial)
{
	Init();
	mspMesh = WIRE_NEW Mesh(pVBuffer, pIBuffer);
}

//----------------------------------------------------------------------------
RenderObject::RenderObject(Mesh* pMesh, Material* pMaterial)
	:
	mspMesh(pMesh),
	mspMaterial(pMaterial)
{
	Init();
	WIRE_ASSERT(pMesh);
}

//----------------------------------------------------------------------------
RenderObject::RenderObject(const RenderObject* pRenderObject)
{
	mspMesh = pRenderObject->mspMesh;
	mspMaterial = pRenderObject->mspMaterial;
	
	for (UInt i = 0; i < State::MAX_STATE_TYPE; i++)
	{
		mStates[i] = pRenderObject->mStates[i];
	}

	mStateSetID = pRenderObject->mStateSetID;

	mpLights = NULL;
	if (pRenderObject->mpLights)
	{
		mpLights = WIRE_NEW TArray<LightPtr>(*(pRenderObject->mpLights));
	}
}

//----------------------------------------------------------------------------
RenderObject::RenderObject()
{
	Init();
}

//----------------------------------------------------------------------------
RenderObject::~RenderObject()
{
	if (mpLights)
	{
		WIRE_DELETE mpLights;
	}
}

//----------------------------------------------------------------------------
void RenderObject::Init()
{
	mpLights = NULL;
	mStateSetID = System::MAX_UINT;
}
