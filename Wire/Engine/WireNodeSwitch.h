// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENODESWITCH_H
#define WIRENODESWITCH_H

#include "WireNode.h"

namespace Wire
{

class NodeSwitch: public Node
{
	WIRE_DECLARE_RTTI;

public:
    NodeSwitch();
    virtual ~NodeSwitch();

    enum { SN_INVALID_CHILD = -1 };

    inline void SetActiveChild(Int activeChild);
    inline Int GetActiveChild() const;
    inline void DisableAllChildren();

protected:
    // culling
    virtual void GetVisibleSet(Culler& rCuller, Bool noCull);

    Int mActiveChild;
};

typedef Pointer<NodeSwitch> NodeSwitchPtr;
#include "WireNodeSwitch.inl"

}

#endif
