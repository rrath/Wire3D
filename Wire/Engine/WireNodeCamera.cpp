// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNodeCamera.h"

#include "WireCamera.h"
#include "WireCuller.h"
#include "WireRenderer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NodeCamera, Node);

//----------------------------------------------------------------------------
NodeCamera::NodeCamera(Camera* pCamera)
	:
	mspCamera(pCamera),
	mDepth(0),
	mClearColor(ColorRGBA::BLACK),
	mClearFlag(CF_ALL),
	mEnabled(true)
{
	CameraToLocalTransform();
}

//----------------------------------------------------------------------------
NodeCamera::~NodeCamera()
{
}

//----------------------------------------------------------------------------
void NodeCamera::Set(Camera* pCamera)
{
	mspCamera = pCamera;

	CameraToLocalTransform();
}

//----------------------------------------------------------------------------
void NodeCamera::UpdateWorldData(Double appTime, Bool updateControllers)
{
	Node::UpdateWorldData(appTime, updateControllers);

	if (mspCamera && mEnabled)
	{
		mspCamera->SetFrame(World.GetTranslate(),
			World.GetRotate().GetColumn(0),
			World.GetRotate().GetColumn(1),
			World.GetRotate().GetColumn(2));
	}
}

//----------------------------------------------------------------------------
void NodeCamera::CameraToLocalTransform()
{
	if (mspCamera)
	{
		Matrix34F matrix(mspCamera->GetDVector(), mspCamera->GetUVector(),
			mspCamera->GetRVector(), mspCamera->GetLocation());
		Local.SetMatrix(matrix, false);
	}
}

//----------------------------------------------------------------------------
void NodeCamera::Draw(TArray<NodeCamera*>& rNodeCameras, Spatial* pRoot,
	Culler& rCuller, Renderer* pRenderer)
{
	WIRE_ASSERT(pRenderer && pRoot);
	if (!pRenderer || !pRoot)
	{
		return;
	}

	const UInt maxCameraCount = 64;
	if (rNodeCameras.GetQuantity() >= 64)
	{
		WIRE_ASSERT(false);
		return;
	}

	// cull all skyboxes attached to cameras in the scene
	Spatial::CullingMode tempCullingModes[maxCameraCount];
	for (UInt i = 0; i < rNodeCameras.GetQuantity(); i++)
	{
		NodeCamera* pNodeCamera = rNodeCameras[i];
		WIRE_ASSERT(pNodeCamera);
		Node* pSkybox = pNodeCamera->mspSkybox;
		if (pSkybox && pNodeCamera->IsEnabled())
		{
			tempCullingModes[i] = pSkybox->Culling;
			pSkybox->Culling = Spatial::CULL_ALWAYS;
		}
	}

	for (UInt i = 0; i < rNodeCameras.GetQuantity(); i++)
	{
		NodeCamera* pNodeCamera = rNodeCameras[i];
		WIRE_ASSERT(pNodeCamera && pNodeCamera->Get());
		if (!pNodeCamera->IsEnabled())
		{
			continue;
		}

		Camera* pCamera = pNodeCamera->Get();

		rCuller.SetCamera(pCamera);
		rCuller.ComputeVisibleSet(pRoot);

		Float left;
		Float right;
		Float top;
		Float bottom;
		pCamera->GetViewport(left, right, top, bottom);
		UInt width = pRenderer->GetWidth();
		UInt height = pRenderer->GetHeight();
		Vector4F rect;
		rect.X() = MathF::Round(left*width);
		rect.Y() = MathF::Round((1.0F-top)*height);
		rect.Z() = MathF::Round((right-left)*width);
		rect.W() = MathF::Round((top-bottom)*height);

		ColorRGBA clearColor = pRenderer->GetClearColor();
		switch (rNodeCameras[i]->mClearFlag)
		{
		case CF_ALL:
			pRenderer->SetClearColor(pNodeCamera->GetClearColor());
			pRenderer->ClearBuffers(true, true, rect);
			pRenderer->SetClearColor(clearColor);
			break;

		case CF_Z_ONLY:
			pRenderer->ClearBuffers(false, true, rect);
			break;

		case CF_NONE:		
			break;

		default:
			WIRE_ASSERT(false);
		}

		pRenderer->SetCamera(pCamera);
		pRenderer->Draw(rCuller.GetVisibleSets());

		Node* pSkybox = pNodeCamera->mspSkybox;
		if (pSkybox)
		{
			pSkybox->Culling = Spatial::CULL_NEVER;
			rCuller.ComputeVisibleSet(pSkybox);
			pRenderer->Draw(rCuller.GetVisibleSets());
			pSkybox->Culling = Spatial::CULL_ALWAYS;
		}
	}

	// restore culling mode of all skyboxes attached to cameras in the scene
	for (UInt i = 0; i < rNodeCameras.GetQuantity(); i++)
	{
		NodeCamera* pNodeCamera = rNodeCameras[i];
		WIRE_ASSERT(pNodeCamera);
		Node* pSkybox = pNodeCamera->mspSkybox;
		if (pSkybox && pNodeCamera->IsEnabled())
		{
			pSkybox->Culling = tempCullingModes[i];
		}
	}
}

//----------------------------------------------------------------------------
void NodeCamera::SortByDepth(TArray<NodeCamera*>& rCameras)
{
	for (UInt i = 0; i < rCameras.GetQuantity()-1; ++i)
	{
		for (UInt j = 0; j < rCameras.GetQuantity()-i-1; ++j)
		{
			if (rCameras[j]->GetDepth() > rCameras[j+1]->GetDepth()) 
			{
				NodeCamera* pTemp = rCameras[j];
				rCameras[j] = rCameras[j+1];
				rCameras[j+1] = pTemp;
			}
		}
	}
}
