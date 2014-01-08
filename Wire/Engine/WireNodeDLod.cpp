// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNodeDLod.h"

#include "WireCuller.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NodeDLod, NodeSwitch);

//----------------------------------------------------------------------------
NodeDLod::NodeDLod()
	:
	mModelLodCenter(Vector3F::ZERO)
{
}

//----------------------------------------------------------------------------
NodeDLod::~NodeDLod()
{
}

//----------------------------------------------------------------------------
void NodeDLod::SetLod(UInt i, Spatial* pLod, Float minDist, Float maxDist)
{
	SetChild(i, pLod);
	SetModelDistance(i, minDist, maxDist);
}

//----------------------------------------------------------------------------
void NodeDLod::SetModelDistance(UInt i, Float minDist, Float maxDist)
{
    mModelMinDist.SetElement(i, minDist);
    mModelMaxDist.SetElement(i, maxDist);
    mWorldMinDist.SetElement(i, minDist);
    mWorldMaxDist.SetElement(i, maxDist);
}

//----------------------------------------------------------------------------
void NodeDLod::SelectLevelOfDetail(const Camera* pCamera)
{
    // ASSERT: The child array of an NodeDLod is compacted. There are no
    // empty slots in the array and the number of children is GetQuantity().
    // Moreover, it is assumed that all model distance values were set for
    // these children.
	WIRE_ASSERT(mModelMinDist.GetQuantity() == GetQuantity());

    // compute world LOD center
    mWorldLodCenter = World.ApplyForward(mModelLodCenter);

    // compute world squared distance intervals
    for (UInt i = 0; i < mChildren.GetQuantity(); i++)
    {
        Float value = mModelMinDist[i];
        mWorldMinDist[i] = World.GetUniformScale() * value;

        value = mModelMaxDist[i];
        mWorldMaxDist[i] = World.GetUniformScale() * value;
    }

    // select the LOD child
    SetActiveChild(SN_INVALID_CHILD);
    if (mChildren.GetQuantity() > 0)
    {
        Vector3F diff = mWorldLodCenter - pCamera->GetLocation();
        Float dist = diff.Length();

        for (UInt i = 0; i < mChildren.GetQuantity(); i++) 
        {
            if (mWorldMinDist[i] <= dist && dist < mWorldMaxDist[i])
            {
                SetActiveChild(i);
                break;
            }
        }
    }
}

//----------------------------------------------------------------------------
void NodeDLod::GetVisibleSet(Culler& rCuller, Bool noCull)
{
    SelectLevelOfDetail(rCuller.GetCamera());
    NodeSwitch::GetVisibleSet(rCuller, noCull);
}
