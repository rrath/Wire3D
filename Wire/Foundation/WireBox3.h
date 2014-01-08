// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREBOX3_H
#define WIREBOX3_H

#include "WireVector3.h"

namespace Wire
{

template <class Real>
class Box3
{
public:
	// A box has center C, axis directions U[0], U[1], and U[2] (all
	// unit-length vectors), and extents e[0], e[1], and e[2] (all nonnegative
	// numbers). A point X = C+y[0]*U[0]+y[1]*U[1]+y[2]*U[2] is inside or
	// on the box whenever |y[i]| <= e[i] for all i.

	// construction
	Box3();  // uninitialized
 	Box3(const Vector3<Real>& rCenter, const Vector3<Real>* pAxis,
		const Real* pExtent);
 	Box3(const Vector3<Real>& rCenter, const Vector3<Real>& rAxis0,
 		const Vector3<Real>& rAxis1, const Vector3<Real>& rAxis2,
 		Real extent0, Real extent1, Real extent2);
 
 	void ComputeVertices(Vector3<Real> vertices[8]) const;
 
 	Vector3<Real> Center;
 	Vector3<Real> Axis[3];  // must be an orthonormal set of vectors
 	Real Extent[3];         // must be nonnegative
};

#include "WireBox3.inl"

typedef Box3<Float> Box3F;
typedef Box3<Double> Box3D;

}

#endif
