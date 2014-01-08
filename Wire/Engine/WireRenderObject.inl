// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Mesh* RenderObject::GetMesh()
{
	return mspMesh;
}

//----------------------------------------------------------------------------
inline const Mesh* RenderObject::GetMesh() const
{
	return mspMesh;
}

//----------------------------------------------------------------------------
inline void RenderObject::SetMesh(Mesh* pMesh)
{
	mspMesh = pMesh;
}

//----------------------------------------------------------------------------
inline Material* RenderObject::GetMaterial()
{
	return mspMaterial;
}

//----------------------------------------------------------------------------
inline const Material* RenderObject::GetMaterial() const
{
	return mspMaterial;
}

//----------------------------------------------------------------------------
inline void RenderObject::SetMaterial(Material* pMaterial)
{
	mspMaterial = pMaterial;
}

//----------------------------------------------------------------------------
inline TArray<LightPtr>* RenderObject::GetLights()
{
	return mpLights;
}

//----------------------------------------------------------------------------
inline const TArray<LightPtr>* RenderObject::GetLights() const
{
	return mpLights;
}

//----------------------------------------------------------------------------
inline Light* RenderObject::GetLight(UInt i) const
{
	if (mpLights)
	{
		if (mpLights->GetQuantity() > i)
		{
			return (*mpLights)[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
inline void RenderObject::SetLights(TArray<LightPtr>* pLights)
{
	if (mpLights)
	{
		WIRE_DELETE mpLights;
	}

	mpLights = pLights;
}

//----------------------------------------------------------------------------
inline StatePtr* RenderObject::GetStates()
{
	return mStates;
}

//----------------------------------------------------------------------------
inline const StatePtr* RenderObject::GetStates() const
{
	return mStates;
}

//----------------------------------------------------------------------------
inline State* RenderObject::GetState(State::StateType type) const
{
	return mStates[type];
}

//----------------------------------------------------------------------------
inline UInt RenderObject::GetStateSetID() const
{
	return mStateSetID;
}

//----------------------------------------------------------------------------
inline void RenderObject::SetStateSetID(UInt stateSetID)
{
	mStateSetID = stateSetID;
}
