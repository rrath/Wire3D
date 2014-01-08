// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNodeSwitch.h"

#include "WireCuller.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NodeSwitch, Node);

//----------------------------------------------------------------------------
NodeSwitch::NodeSwitch()
{
    mActiveChild = SN_INVALID_CHILD;
}

//----------------------------------------------------------------------------
NodeSwitch::~NodeSwitch()
{
}

//----------------------------------------------------------------------------
void NodeSwitch::GetVisibleSet(Culler& rCuller, Bool noCull)
{
    if (mActiveChild == SN_INVALID_CHILD)
    {
        return;
    }

	for (UInt i = 0; i < mEffects.GetQuantity(); i++)
    {
        // This is a global effect. Place a 'begin' marker in the visible
        // set to indicate the effect is active.
        rCuller.Insert(mEffects[i], NULL);
    }

    Spatial* pChild = mChildren[mActiveChild];
    if (pChild)
    {
        pChild->OnGetVisibleSet(rCuller, noCull);
    }

	for (UInt i = 0; i < mEffects.GetQuantity(); i++)
    {
        // Place an 'end' marker in the visible set to indicate that the
        // global effect is inactive.
        rCuller.Insert(NULL, NULL);
    }
}
