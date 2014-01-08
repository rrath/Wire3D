// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline void SphereBV::SetCenter(const Vector3F& rCenter)
{
	mSphere.Center = rCenter;
}

//----------------------------------------------------------------------------
inline void SphereBV::SetRadius(Float radius)
{
	mSphere.Radius = radius;
}

//----------------------------------------------------------------------------
inline Vector3F SphereBV::GetCenter() const
{
	return mSphere.Center;
}

//----------------------------------------------------------------------------
inline Float SphereBV::GetRadius() const
{
	return mSphere.Radius;
}

//----------------------------------------------------------------------------
inline Sphere3F& SphereBV::Sphere()
{
	return mSphere;
}

//----------------------------------------------------------------------------
inline const Sphere3F& SphereBV::GetSphere() const
{
	return mSphere;
}
