// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class Real>
Plane3<Real>::Plane3()
{
	// uninitialized
}

//----------------------------------------------------------------------------
template <class Real>
Plane3<Real>::Plane3(const Plane3& rPlane)
	:
	Normal(rPlane.Normal)
{
	Constant = rPlane.Constant;
}

//----------------------------------------------------------------------------
template <class Real>
Real Plane3<Real>::DistanceTo(const Vector3<Real>& rP) const
{
    return Normal.Dot(rP) - Constant;
}

//----------------------------------------------------------------------------
template <class Real>
Int Plane3<Real>::WhichSide(const Vector3<Real>& rQ) const
{
	Real distance = DistanceTo(rQ);

	if (distance < static_cast<Real>(0.0))
	{
		return -1;
	}

	if (distance > static_cast<Real>(0.0))
	{
		return +1;
	}

	return 0;
}
