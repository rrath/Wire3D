// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENODEBILLBOARD_H
#define WIRENODEBILLBOARD_H

#include "WireNode.h"

namespace Wire
{

class Camera;

class NodeBillboard : public Node
{
	WIRE_DECLARE_RTTI;

public:
    // The model space of the billboard has an up vector of (0,1,0) that is
    // chosen to be the billboard's axis of rotation.

    // Construction and destruction.
    NodeBillboard(Camera* pCamera = NULL);
    virtual ~NodeBillboard();

    // the camera to which the billboard is aligned
    inline void AlignTo(Camera* pCamera);

protected:
    // geometric updates
    virtual void UpdateWorldData(Double appTime, Bool updateControllers);

    Pointer<Camera> mspCamera;
};

typedef Pointer<NodeBillboard> NodeBillboardPtr;

#include "WireNodeBillboard.inl"

}

#endif
