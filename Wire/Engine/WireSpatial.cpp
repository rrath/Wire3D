// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireSpatial.h"

#include "WireBoundingVolume.h"
#include "WireCuller.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Spatial, SceneObject);

//----------------------------------------------------------------------------
Spatial::Spatial()
	:
	WorldIsCurrent(false),
	WorldBound(BoundingVolume::Create()),
	WorldBoundIsCurrent(false),
	Culling(CULL_DYNAMIC),
	mpParent(NULL)
{
}

//----------------------------------------------------------------------------
Spatial::Spatial(const Spatial* pSpatial)
	:
	mpParent(NULL)
{
	Local = pSpatial->Local;
	World = pSpatial->World;
	WorldIsCurrent = pSpatial->WorldIsCurrent;
	WorldBound = BoundingVolume::Create();
	WorldBound->CopyFrom(pSpatial->WorldBound);
	WorldBoundIsCurrent = pSpatial->WorldBoundIsCurrent;
	Culling = pSpatial->Culling;
	mStates.SetQuantity(pSpatial->mStates.GetQuantity());
	for (UInt i = 0; i < pSpatial->mStates.GetQuantity(); i++)
	{
		mStates[i] = pSpatial->mStates[i];
	}

	mLights.SetQuantity(pSpatial->mLights.GetQuantity());
	for (UInt i = 0; i < pSpatial->mLights.GetQuantity(); i++)
	{
		mLights[i] = pSpatial->mLights[i];
	}
}

//----------------------------------------------------------------------------
Spatial::~Spatial()
{
}

//----------------------------------------------------------------------------
void Spatial::UpdateGS(Double appTime, Bool isInitiator,
	Bool updateControllers)
{
	UpdateWorldData(appTime, updateControllers);
	UpdateWorldBound();
	if (isInitiator)
	{
		PropagateBoundToRoot();
	}
}

//----------------------------------------------------------------------------
void Spatial::UpdateBS()
{
	UpdateWorldBound();
	PropagateBoundToRoot();
}

//----------------------------------------------------------------------------
void Spatial::UpdateWorldData(Double appTime, Bool updateControllers)
{
	if (updateControllers)
	{
		// update any controllers associated with this object
		UpdateControllers(appTime);
	}

	// update world transforms
	if (!WorldIsCurrent)
	{
		if (mpParent)
		{
			World.Product(mpParent->World, Local);
		}
		else
		{
			World = Local;
		}
	}
}

//----------------------------------------------------------------------------
void Spatial::PropagateBoundToRoot()
{
	if (mpParent)
	{
		mpParent->UpdateWorldBound();
		mpParent->PropagateBoundToRoot();
	}
}

//----------------------------------------------------------------------------
void Spatial::OnGetVisibleSet(Culler& rCuller, Bool noCull)
{
	if (Culling == CULL_ALWAYS)
	{
		return;
	}

	if (Culling == CULL_NEVER)
	{
		noCull = true;
	}

	UInt savePlaneState = rCuller.GetPlaneState();
	if (noCull || rCuller.IsVisible(WorldBound, true))
	{
		OnGetVisibleUpdateControllers(rCuller.GetCamera());
		GetVisibleSet(rCuller, noCull);
	}

	rCuller.SetPlaneState(savePlaneState);
}

//----------------------------------------------------------------------------
State* Spatial::GetState(State::StateType type) const
{
	// check if type of state already exists
	for (UInt i = 0; i < mStates.GetQuantity(); i++)
	{
		if (mStates[i]->GetStateType() == type)
		{
			// type of state exists, return it
			return mStates[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
void Spatial::AttachState(State* pState)
{
	WIRE_ASSERT(pState);

	// Check if this type of state is already in the list.
	for (UInt i = 0; i < mStates.GetQuantity(); i++)
	{
		if (mStates[i]->GetStateType() == pState->GetStateType())
		{
			// This type of state already exists, so replace it.
			mStates[i] = pState;
			return;
		}
	}

	// This type of state is not in the current list, so add it.
	mStates.Append(pState);
}

//----------------------------------------------------------------------------
void Spatial::DetachState(State::StateType type)
{
	for (UInt i = 0; i < mStates.GetQuantity(); i++)
	{
		State* pState = mStates[i];

		if (pState->GetStateType() == type)
		{
			mStates.RemoveAt(i);
			return;
		}
	}
}

//----------------------------------------------------------------------------
void Spatial::AttachLight(Light* pLight)
{
	WIRE_ASSERT(pLight);

	// Check if this light is already in the list.
	for (UInt i = 0; i < mLights.GetQuantity(); i++)
	{
		if (mLights[i] == pLight)
		{
			// This Light already exists.
			return;
		}
	}

	// This light is not in the current list, so add it.
	mLights.Append(pLight);
}

//----------------------------------------------------------------------------
void Spatial::UpdateRS(States* pStates, Lights*	pLights)
{
	Bool isInitiator = (pStates == NULL);

	if (isInitiator)
	{
		const UInt growBy = 16;

		// The order of preference is
		//   (1) Default global states are used.
		//   (2) Nodes can override them
		//   (3) Effects can override render states at DrawCall
		//
		// Note: If states have not been pushed to a Node's RenderObject (i.e.
		//   RenderObject's states are NULL), the current render state of the
		//   Renderer is being used to draw the RenderObject.

		pStates = WIRE_NEW TArray<State*>[State::MAX_STATE_TYPE];

		for (UInt i = 0; i < State::MAX_STATE_TYPE; i++)
		{
			pStates[i].SetGrowBy(growBy);
			pStates[i].Append(State::Default[i]);
		}

        // stack has no lights initially
        pLights = WIRE_NEW TArray<Light*>(0, growBy);

		// traverse to root and push states from root to this node
		PropagateStateFromRoot(pStates, pLights);
	}
	else
	{
		// push states at this node
		PushState(pStates, pLights);
	}

	// propagate the new state to the subtree rooted here
	UpdateState(pStates, pLights);

	if (isInitiator)
	{
		WIRE_DELETE[] pStates;
		WIRE_DELETE pLights;
	}
	else
	{
		// pop states at this node
		PopState(pStates, pLights);
	}
}

//----------------------------------------------------------------------------
void Spatial::PropagateStateFromRoot(States* pStates, Lights* pLights)
{
	// traverse to root to allow downward state propagation
	if (mpParent)
	{
		mpParent->PropagateStateFromRoot(pStates, pLights);
	}

	// push states onto current render state stack
	PushState(pStates, pLights);
}

//----------------------------------------------------------------------------
void Spatial::PushState(States* pStates, Lights* pLights)
{
	for (UInt i = 0; i < mStates.GetQuantity(); i++)
	{
		State::StateType type = mStates[i]->GetStateType();
		pStates[type].Append(mStates[i]);
	}

	for (UInt i = 0; i < mLights.GetQuantity(); i++)
	{
		pLights->Append(mLights[i]);
	}
}

//----------------------------------------------------------------------------
void Spatial::PopState(States* pStates,	Lights* pLights)
{
	for (UInt i = 0; i < mStates.GetQuantity(); i++)
	{
		State::StateType type = mStates[i]->GetStateType();
		pStates[type].RemoveLast();
	}

	for (UInt i = 0; i < mLights.GetQuantity(); i++)
	{
		pLights->RemoveLast();
	}
}
