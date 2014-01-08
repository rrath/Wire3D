// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Vector3F Camera::GetLocation() const
{
	return mLocation;
}

//----------------------------------------------------------------------------
inline void Camera::SetLocation(const Vector3F& rLocation)
{
	mLocation = rLocation;
}

//----------------------------------------------------------------------------
inline Vector3F Camera::GetDVector() const
{
	return mDVector;
}

//----------------------------------------------------------------------------
inline Vector3F Camera::GetUVector() const
{
	return mUVector;
}

//----------------------------------------------------------------------------
inline Vector3F Camera::GetRVector() const
{
	return mRVector;
}

//----------------------------------------------------------------------------
inline const Float* Camera::GetFrustum() const
{
	return mFrustum;
}

//----------------------------------------------------------------------------
inline Float Camera::GetDMin() const
{
	return mFrustum[VF_DMIN];
}

//----------------------------------------------------------------------------
inline Float Camera::GetDMax() const
{
	return mFrustum[VF_DMAX];
}

//----------------------------------------------------------------------------
inline Float Camera::GetUMin() const
{
	return mFrustum[VF_UMIN];
}

//----------------------------------------------------------------------------
inline Float Camera::GetUMax() const
{
	return mFrustum[VF_UMAX];
}

//----------------------------------------------------------------------------
inline Float Camera::GetRMin() const
{
	return mFrustum[VF_RMIN];
}

//----------------------------------------------------------------------------
inline Float Camera::GetRMax() const
{
	return mFrustum[VF_RMAX];
}

//----------------------------------------------------------------------------
inline Bool Camera::IsPerspective() const
{
	return mIsPerspective;
}

//----------------------------------------------------------------------------
inline void Camera::SetLayerMask(UInt64 layerMask)
{
	mLayerMask = layerMask;
}

//----------------------------------------------------------------------------
inline UInt64 Camera::GetLayerMask() const
{
	return mLayerMask;
}
