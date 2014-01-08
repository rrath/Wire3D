// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRESPHEREBV_H
#define WIRESPHEREBV_H

#include "WireBoundingVolume.h"
#include "WireSphere3.h"

namespace Wire
{

class SphereBV : public BoundingVolume
{
	WIRE_DECLARE_RTTI;

public:
	SphereBV();
	SphereBV(const Sphere3F& rSphere);
	virtual ~SphereBV();

	// all bounding volumes must define a center and radius
	inline virtual void SetCenter(const Vector3F& rCenter);
	inline virtual void SetRadius(Float radius);
	inline virtual Vector3F GetCenter() const;
	inline virtual Float GetRadius() const;

	inline Sphere3F& Sphere();
	inline const Sphere3F& GetSphere() const;

	// Compute a sphere that contains all the points.
	virtual void ComputeFrom(const VertexBuffer* pVertexBuffer);

	// Compute a sphere that contains only indexed points
	virtual void ComputeFrom(const VertexBuffer* pVertexBuffer,
		const IndexBuffer* pIndexBuffer, UInt startIndex, UInt indexCount);

	// Transform the sphere (model-to-world conversion).
	virtual void TransformBy(const Transformation& rTransform,
		BoundingVolume* pResult);

	// Determine if the bounding volume is one side of the plane, the other
	// side, or straddles the plane. If it is on the positive side (the
	// side to which the normal points), the return value is +1. If it is
	// on the negative side, the return value is -1. If it straddles the
	// plane, the return value is 0.
	virtual Int WhichSide(const Plane3F& rPlane) const;

	// Make a copy of the bounding volume.
	virtual void CopyFrom(const BoundingVolume* pInput);

	// Change the current sphere so that it is the minimum volume sphere that
	// contains the input sphere as well as its old sphere.
	virtual void GrowToContain(const BoundingVolume* pInput);

protected:
	Sphere3F mSphere;
};

typedef Pointer<SphereBV> SphereBVPtr;
#include "WireSphereBV.inl"

}

#endif
