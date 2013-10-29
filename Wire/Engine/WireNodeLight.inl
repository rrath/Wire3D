// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2013. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Light* NodeLight::Get()
{
	return mspLight;
}

//----------------------------------------------------------------------------
inline const Light* NodeLight::Get() const
{
	return mspLight;
}