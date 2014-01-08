// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Box3<Real>::Box3()
{
	// uninitialized
}

//----------------------------------------------------------------------------
template <class Real>
Box3<Real>::Box3(const Vector3<Real>& rCenter, const Vector3<Real>* pAxis,
	const Real* pExtent)
	:
	Center(rCenter)
{
	for (UInt i = 0; i < 3; i++)
	{
		Axis[i] = pAxis[i];
		Extent[i] = pExtent[i];
		WIRE_ASSERT(Extent[i] >= static_cast<Real>(0.0));
	}
}

//----------------------------------------------------------------------------
template <class Real>
Box3<Real>::Box3(const Vector3<Real>& rCenter, const Vector3<Real>& rAxis0,
	const Vector3<Real>& rAxis1, const Vector3<Real>& rAxis2, Real extent0,
	Real extent1, Real extent2)
	:
	Center(rCenter)
{
	Axis[0] = rAxis0;
	Axis[1] = rAxis1;
	Axis[2] = rAxis2;
	Extent[0] = extent0;
	Extent[1] = extent1;
	Extent[2] = extent2;

	WIRE_ASSERT(Extent[0] >= static_cast<Real>(0.0));
	WIRE_ASSERT(Extent[1] >= static_cast<Real>(0.0));
	WIRE_ASSERT(Extent[2] >= static_cast<Real>(0.0));
}

//----------------------------------------------------------------------------
template <class Real>
void Box3<Real>::ComputeVertices(Vector3<Real> vertices[8]) const
{
	Vector3<Real> axes[3] =
	{
		Extent[0]*Axis[0],
		Extent[1]*Axis[1],
		Extent[2]*Axis[2]
	};

	vertices[0] = Center - axes[0] - axes[1] - axes[2];
	vertices[1] = Center + axes[0] - axes[1] - axes[2];
	vertices[2] = Center + axes[0] + axes[1] - axes[2];
	vertices[3] = Center - axes[0] + axes[1] - axes[2];
	vertices[4] = Center - axes[0] - axes[1] + axes[2];
	vertices[5] = Center + axes[0] - axes[1] + axes[2];
	vertices[6] = Center + axes[0] + axes[1] + axes[2];
	vertices[7] = Center - axes[0] + axes[1] + axes[2];
}
