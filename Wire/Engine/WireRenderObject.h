// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRERENDEROBJECT_H
#define WIRERENDEROBJECT_H

#include "WireLight.h"
#include "WireMaterial.h"
#include "WireMesh.h"
#include "WireState.h"

namespace Wire
{

class IndexBuffer;
class VertexBuffer;

class RenderObject: public Object
{
	WIRE_DECLARE_RTTI;

public:
	RenderObject(VertexBuffer* pVBuffer, IndexBuffer* pIBuffer, Material*
		pMaterial = NULL);
	RenderObject(Mesh* pMesh, Material* pMaterial = NULL);
	// shallow copy (shared Mesh and Material)
	RenderObject(const RenderObject* pRenderObject);
	virtual ~RenderObject();

	inline Mesh* GetMesh();
	inline const Mesh* GetMesh() const;
	inline void SetMesh(Mesh* pMesh);

	inline Material* GetMaterial();
	inline const Material* GetMaterial() const;
	inline void SetMaterial(Material* pMaterial);

	inline TArray<LightPtr>* GetLights();
	inline const TArray<LightPtr>* GetLights() const;
	inline Light* GetLight(UInt i = 0) const;
	void SetLights(TArray<LightPtr>* pLights);

	inline StatePtr* GetStates();
	inline const StatePtr* GetStates() const;
	inline State* GetState(State::StateType type) const;

	inline UInt GetStateSetID() const;
	inline void SetStateSetID(UInt stateSetID);

protected:
	RenderObject();
	void Init();

private:
	MeshPtr mspMesh;
	MaterialPtr mspMaterial;

	StatePtr mStates[State::MAX_STATE_TYPE];
	TArray<LightPtr>* mpLights;

	// Identical IDs of different RenderObjects mean that all their
	// States[] and Lights are identical. This is used for sorting by
	// render state.
	UInt mStateSetID;
};

typedef Pointer<RenderObject> RenderObjectPtr;
#include "WireRenderObject.inl"

}

#endif
