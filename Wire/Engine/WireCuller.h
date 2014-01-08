// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRECULLER_H
#define WIRECULLER_H

#include "WireBoundingVolume.h"
#include "WireCamera.h"
#include "WireTArray.h"
#include "WireVisibleSet.h"

namespace Wire
{

class RenderObject;
class Spatial;

class Culler
{
public:
	// Construction and destruction. If the camera is not passed to the
	// constructor, you should set it using SetCamera before calling
	// ComputeVisibleSet. The other two input parameters are used to define the
	// behavior of the container of the set of potentially visible objects.
	Culler(const Camera* pCamera = NULL, UInt maxQuantity = VisibleSet::
		VS_DEFAULT_MAX_QUANTITY, UInt growBy = VisibleSet::VS_DEFAULT_GROWBY);
	virtual ~Culler();

	// Access to the camera, frustum copy, and potentially visible set.
	inline void SetCamera(const Camera* pCamera);
	inline const Camera* GetCamera() const;
	void SetFrustum(const Float* pFrustum);
	inline VisibleSet* GetVisibleSet(UInt i = 0);
	inline TArray<VisibleSet*>& GetVisibleSets();

	// This is the main function you should use for culling within a scene
	// graph. Traverse the scene and construct the potentially visible set
	// relative to the world planes.
	virtual void ComputeVisibleSet(Spatial* pScene);

	// Compare the object's world bounding volume against all culling planes.
	Bool IsVisible(const Spatial* pSpatial) const;

	// Compare the bounding volume against the active culling planes.
	// Only Spatial calls this function (during traversal using plane state)
	Bool IsVisible(const BoundingVolume* pBV, Bool usePlaneMask = false);

	// The base class behavior appends the RenderObject/Effect and its
	// Transformation/NULL to the end of the VisibleSet. Derived classes
	// may override this behavior; for example, the array might be maintained
	// as a sorted array for minimizing render state changes or it might be
	// maintained as a unique list of objects for a portal system.
	inline virtual void Insert(Object* pObject, Transformation* pTrafo,
		const Vector3F& rPosition = Vector3F::ZERO);

	void Clear();
	void SetMaxQuantity(UInt maxQuantity);

	enum { VS_MAX_PLANE_QUANTITY = 32 };
	inline void SetPlaneState(UInt planeState);
	inline UInt GetPlaneState() const;

protected:
	// The input camera has information that might be needed during the
	// culling pass over the scene.
	const Camera* mpCamera;

	// A copy of the view frustum for the input camera. This allows various
	// subsystems to change the frustum parameters during culling (for
	// example, the portal system) without affecting the camera, whose initial
	// state is needed by the renderer.
	Float mFrustum[Camera::VF_QUANTITY];

	// The world culling planes corresponding to the view frustum plus any
	// additional user-defined culling planes. The member mPlaneState
	// represents bit flags to store whether or not a plane is active in the
	// culling system. A bit of 1 means the plane is active, otherwise the
	// plane is inactive. An active plane is compared to bounding volumes,
	// whereas an inactive plane is not. This supports an efficient culling
	// of a hierarchy. For example, if a node's bounding volume is inside
	// the left plane of the view frustum, then the left plane is set to
	// inactive because the children of the node are automatically all inside
	// the left plane.
	Int mPlaneQuantity;
	Plane3F mPlanes[VS_MAX_PLANE_QUANTITY];
	UInt mPlaneState;

	// The potentially visible sets for a call to GetVisibleSet.
	TArray<VisibleSet*> mVisibleSets;
};

#include "WireCuller.inl"

}

#endif
