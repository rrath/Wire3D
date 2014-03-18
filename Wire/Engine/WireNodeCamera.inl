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

//----------------------------------------------------------------------------
inline void NodeCamera::SetDepth(Int depth)
{
	mDepth = depth;
}

//----------------------------------------------------------------------------
inline Int NodeCamera::GetDepth() const
{
	return mDepth;
}

//----------------------------------------------------------------------------
inline void NodeCamera::SetClearColor(const ColorRGBA& rClearColor)
{
	mClearColor = rClearColor;
}

//----------------------------------------------------------------------------
inline ColorRGBA NodeCamera::GetClearColor() const
{
	return mClearColor;
}

//----------------------------------------------------------------------------
inline void NodeCamera::SetClearFlag(ClearFlag clearFlag)
{
	mClearFlag = clearFlag;
}

//----------------------------------------------------------------------------
inline NodeCamera::ClearFlag NodeCamera::GetClearFlag() const
{
	return mClearFlag;
}

//----------------------------------------------------------------------------
inline void NodeCamera::SetSkybox(Node* pSkybox)
{
	mspSkybox = pSkybox;
}

//----------------------------------------------------------------------------
inline Node* NodeCamera::GetSkybox() const
{
	return mspSkybox;
}
