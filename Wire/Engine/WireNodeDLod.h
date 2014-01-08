// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENODEDLOD_H
#define WIRENODEDLOD_H

#include "WireNodeSwitch.h"

namespace Wire
{

class Camera;

class NodeDLod : public NodeSwitch
{
	WIRE_DECLARE_RTTI;

public:
	NodeDLod();
	virtual ~NodeDLod();

	void SetLod(UInt i, Spatial* pLod, Float minDist, Float maxDist);

    // center for level of detail
    inline Vector3F& ModelCenter();
    inline const Vector3F& GetModelCenter() const;
    inline const Vector3F& GetWorldCenter() const;

    // distance intervals for children
    void SetModelDistance(UInt i, Float minDist, Float maxDist);
    inline Float GetModelMinDistance(UInt i) const;
    inline Float GetModelMaxDistance(UInt i) const;
    inline Float GetWorldMinDistance(UInt i) const;
    inline Float GetWorldMaxDistance(UInt i) const;

protected:
    // set the NodeSwitch based on distance from world LOD center to camera
    void SelectLevelOfDetail(const Camera* pCamera);

    // culling
    virtual void GetVisibleSet(Culler& rCuller, Bool noCull);

	// point whose distance to camera determines correct child
    Vector3F mModelLodCenter;
    Vector3F mWorldLodCenter;

    // squared distances for each LOD interval
    TArray<Float> mModelMinDist;
    TArray<Float> mModelMaxDist;
    TArray<Float> mWorldMinDist;
    TArray<Float> mWorldMaxDist;
};

typedef Pointer<NodeDLod> NodeDLodPtr;

#include "WireNodeDLod.inl"

}

#endif
