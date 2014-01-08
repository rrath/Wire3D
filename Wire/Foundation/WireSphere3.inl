// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real>::Sphere3()
{
	// uninitialized
}

//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real>::Sphere3(const Vector3<Real>& rCenter, Real radius)
	:
	Center(rCenter),
	Radius(radius)
{
}

//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real>::Sphere3(const Sphere3& rSphere)
	:
	Center(rSphere.Center),
	Radius(rSphere.Radius)
{
}

//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real>& Sphere3<Real>::operator= (const Sphere3& rSphere)
{
	Center = rSphere.Center;
	Radius = rSphere.Radius;
	return *this;
}

//----------------------------------------------------------------------------
template <class Real>
Sphere3<Real> Sphere3<Real>::MergeSpheres(const Sphere3<Real>& rSphere0,
	const Sphere3<Real>& rSphere1)
{
	Vector3<Real> centerDiff = rSphere1.Center - rSphere0.Center;
	Real centerDiffLenSqr = centerDiff.SquaredLength();
	Real radiusDiff = rSphere1.Radius - rSphere0.Radius;
	Real radiusDiffSqr = radiusDiff * radiusDiff;

	if (radiusDiffSqr >= centerDiffLenSqr)
	{
		return (radiusDiff >= static_cast<Real>(0.0) ? rSphere1 : rSphere0);
	}

	Real length = Math<Real>::Sqrt(centerDiffLenSqr);
	Sphere3<Real> sphere;

	if (length > Math<Real>::ZERO_TOLERANCE)
	{
		Real coeff = (length + radiusDiff)/((static_cast<Real>(2.0))*length);
		sphere.Center = rSphere0.Center + coeff*centerDiff;
	}
	else
	{
		sphere.Center = rSphere0.Center;
	}

	sphere.Radius = (static_cast<Real>(0.5))*(length + rSphere0.Radius +
		rSphere1.Radius);

	return sphere;
}
