// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Image2D* Texture2D::GetImage()
{
	return mspImage;
}

//----------------------------------------------------------------------------
inline const Image2D* Texture2D::GetImage() const
{
	return mspImage;
}

//----------------------------------------------------------------------------
inline void Texture2D::SetFilterType(FilterType filterType)
{
	mFilterType = filterType;
}

//----------------------------------------------------------------------------
inline Texture2D::FilterType Texture2D::GetFilterType() const
{
	return mFilterType;
}

//----------------------------------------------------------------------------
inline void Texture2D::SetWrapType(UInt i, WrapType wrapType)
{
	WIRE_ASSERT(i < 2);
	mWarpType[i] = wrapType;
}

//----------------------------------------------------------------------------
inline Texture2D::WrapType Texture2D::GetWrapType(UInt i) const
{
	WIRE_ASSERT(i < 2);
	return mWarpType[i];
}

//----------------------------------------------------------------------------
inline void Texture2D::SetAnisotropyValue(Float anisotropy)
{
	mAnisotropy = anisotropy;
}

//----------------------------------------------------------------------------
inline Float Texture2D::GetAnisotropyValue() const
{
	return mAnisotropy;
}
