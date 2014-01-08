// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREPLANE3_H
#define WIREPLANE3_H

#include "WireVector3.h"

namespace Wire
{

template <class Real>
class Plane3
{
public:
    // The plane is represented as Dot(N,X) = c where N is a unit-length
    // normal vector, c is the plane constant, and X is any point on the
    // plane. The user must ensure that the normal vector satisfies this
    // condition.

    Plane3();  // uninitialized
    Plane3(const Plane3& rPlane);

	// Compute d = Dot(N,Q)-c where N is the plane normal and c is the plane
	// constant. This is a signed distance. The sign of the return value is
	// positive if the point is on the positive side of the plane, negative if
	// the point is on the negative side, and zero if the point is on the
	// plane.
	Real DistanceTo(const Vector3<Real>& rQ) const;

	// The "positive side" of the plane is the half space to which the plane
	// normal points. The "negative side" is the other half space. The
	// function returns +1 for the positive side, -1 for the negative side,
	// and 0 for the point being on the plane.
	Int WhichSide(const Vector3<Real>& rP) const;

	Vector3<Real> Normal;
	Real Constant;
};

#include "WirePlane3.inl"

typedef Plane3<Float> Plane3F;
typedef Plane3<Double> Plane3D;

}

#endif
