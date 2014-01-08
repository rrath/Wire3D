// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNodeSkybox.h"

#include "WireCuller.h"
#include "WireIndexBuffer.h"
#include "WireStateCull.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NodeSkybox, Node);

//----------------------------------------------------------------------------
NodeSkybox::NodeSkybox(Texture2D* pPosZ, Texture2D* pNegZ, Texture2D* pPosX,
	Texture2D* pNegX, Texture2D* pPosY, Texture2D* pNegY)
{
	Init(pPosZ, pNegZ, pPosX, pNegX, pPosY, pNegY);
}

//----------------------------------------------------------------------------
NodeSkybox::~NodeSkybox()
{
}

//----------------------------------------------------------------------------
void NodeSkybox::UpdateWorldData(Double appTime, Bool)
{
	mAppTime = appTime;
}

//----------------------------------------------------------------------------
void NodeSkybox::GetVisibleSet(Culler& rCuller, Bool noCull)
{
	const Float sqrtOfThree = 1.7320508F;
	const Camera* const pCamera = rCuller.GetCamera();
	Float scale = pCamera->GetDMax() * 0.25F * sqrtOfThree;
	World.SetTranslate(pCamera->GetLocation());
	World.SetUniformScale(scale);

	Node::UpdateWorldData(mAppTime, true);
	Node::GetVisibleSet(rCuller, noCull);
}

//----------------------------------------------------------------------------
void NodeSkybox::Init(Texture2D* pPosZ, Texture2D* pNegZ, Texture2D* pPosX,
	Texture2D* pNegX, Texture2D* pPosY, Texture2D* pNegY)
{
	mAppTime = 0;
	Culling = CULL_NEVER;
	WorldIsCurrent = true;
	StateCull* pCull = WIRE_NEW StateCull;
	pCull->CullFace = StateCull::CM_BACK;
	AttachState(pCull);

	// +Z (front)
	AddQuad(Vector3F(1.0F, 1.0F, 1.0F), Vector3F(-1.0F, 1.0F, 1.0F),
			Vector3F(-1.0F, -1.0F, 1.0F), Vector3F(1.0F, -1.0F, 1.0F),
			pPosZ);

	// -Z (back)
	AddQuad(Vector3F(-1.0F, 1.0F, -1.0F), Vector3F(1.0F, 1.0F,-1.0F),
			Vector3F(1.0F, -1.0F, -1.0F), Vector3F(-1.0F, -1.0F, -1.0F),
			pNegZ);

	// +X (left)
	AddQuad(Vector3F(1.0F, 1.0F, -1.0F), Vector3F(1.0F, 1.0F, 1.0F),
			Vector3F(1.0F, -1.0F, 1.0F), Vector3F(1.0F, -1.0F, -1.0F),
			pPosX);

	// -X (right)
	AddQuad(Vector3F(-1.0F, 1.0F, 1.0F), Vector3F(-1.0F, 1.0F, -1.0F),
			Vector3F(-1.0F, -1.0F, -1.0F), Vector3F(-1.0F, -1.0F, 1.0F),
			pNegX);

	// +Y (up)
	AddQuad(Vector3F(1.0F, 1.0F, -1.0F), Vector3F(-1.0F, 1.0F, -1.0F),
			Vector3F(-1.0F, 1.0F, 1.0F), Vector3F(1.0F, 1.0F, 1.0F),
			pPosY);

	// -Y (down)
	AddQuad(Vector3F(1.0F, -1.0F, 1.0F), Vector3F(-1.0F, -1.0F, 1.0F),
			Vector3F(-1.0F, -1.0F, -1.0F), Vector3F(1.0F, -1.0F, -1.0F),
			pNegY);
}

//----------------------------------------------------------------------------
void NodeSkybox::AddQuad(const Vector3F& v0, const Vector3F& v1, const
	Vector3F& v2, const Vector3F& v3, Texture2D* pTexture)
{
	VertexAttributes attributes;
	attributes.SetPositionChannels(3);  // channels: X, Y, Z
	attributes.SetTCoordChannels(2);	// channels: U, V

	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes, 4);
	pVBuffer->Position3(0) = v0;
	pVBuffer->TCoord2(0) = Vector2F(0, 0);
	pVBuffer->Position3(1) = v1;
	pVBuffer->TCoord2(1) = Vector2F(1, 0);
	pVBuffer->Position3(2) = v2;
	pVBuffer->TCoord2(2) = Vector2F(1, 1);
	pVBuffer->Position3(3) = v3;
	pVBuffer->TCoord2(3) = Vector2F(0, 1);

	const UShort indices[6] = { 0, 1, 2, 0, 2, 3 };
	UInt indexQuantity = sizeof(indices) / sizeof(UShort);
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(indexQuantity);
	for	(UInt i = 0; i < indexQuantity; i++)
	{
		(*pIBuffer)[i] = indices[i];
	}

	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(pTexture, Material::BM_REPLACE);

	Node* pQuad = WIRE_NEW Node(pVBuffer, pIBuffer, pMaterial);
	AttachChild(pQuad);
}
