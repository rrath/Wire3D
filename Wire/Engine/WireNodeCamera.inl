// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Camera* NodeCamera::Get()
{
	return mspCamera;
}

//----------------------------------------------------------------------------
inline const Camera* NodeCamera::Get() const
{
	return mspCamera;
}

//----------------------------------------------------------------------------
inline void NodeCamera::SetEnabled(Bool enabled)
{
	mEnabled = enabled;
}

//----------------------------------------------------------------------------
inline Bool NodeCamera::IsEnabled() const
{
	return mEnabled;
}
