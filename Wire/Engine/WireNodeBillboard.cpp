// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNodeBillboard.h"

#include "WireCamera.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NodeBillboard, Node);

//----------------------------------------------------------------------------
NodeBillboard::NodeBillboard(Camera* pCamera)
    :
    mspCamera(pCamera)
{
}

//----------------------------------------------------------------------------
NodeBillboard::~NodeBillboard()
{
}

//----------------------------------------------------------------------------
void NodeBillboard::UpdateWorldData(Double appTime, Bool updateControllers)
{
    // Compute billboard's world transforms based on its parent's world
    // transform and its local transforms. Notice that you should not call
    // Node::UpdateWorldData since that function updates its children. The
    // children of a NodeBillboard cannot be updated until the billboard is
    // aligned with the camera.
    Spatial::UpdateWorldData(appTime, updateControllers);

    if (mspCamera)
    {
        // Inverse-transform the camera to the model space of the billboard.
        Vector3F camLocation = World.ApplyInverse(mspCamera->GetLocation());

        // To align the billboard, the projection of the camera to the
        // xz-plane of the billboard's model space determines the angle of
        // rotation about the billboard's model y-axis. If the projected
        // camera is on the model axis (x = 0 and z = 0), ATan2 returns zero
        // (rather than NaN), so there is no need to trap this degenerate
        // case and handle it separately.
        Float angle = MathF::ATan2(camLocation.X(), camLocation.Z());
        Matrix34F orientation(Vector3F::UNIT_Y, angle);
        World.SetRotate(World.GetMatrix() * orientation);
    }

    // update the children now that the billboard orientation is known
    for (UInt i = 0; i < mChildren.GetQuantity(); i++)
    {
        Spatial* pChild = mChildren[i];
        if (pChild)
        {
            pChild->UpdateGS(appTime, false, updateControllers);
        }
    }
}
