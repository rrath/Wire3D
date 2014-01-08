// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt RenderTarget::GetTargetQuantity() const
{
	return mImages.GetQuantity();
}

//----------------------------------------------------------------------------
inline Image2D::FormatMode RenderTarget::GetFormat() const
{
	WIRE_ASSERT(mImages.GetQuantity() > 0);
	return mImages[0]->GetFormat();
}

//----------------------------------------------------------------------------
inline UInt RenderTarget::GetWidth() const
{
	WIRE_ASSERT(mImages.GetQuantity() > 0);
	return mImages[0]->GetBound(0);
}

//----------------------------------------------------------------------------
inline UInt RenderTarget::GetHeight() const
{
	WIRE_ASSERT(mImages.GetQuantity() > 0);
	return mImages[0]->GetBound(1);
}

//----------------------------------------------------------------------------
inline Image2D* RenderTarget::GetColorTexture(UInt i) const
{
	WIRE_ASSERT(mImages.GetQuantity() > i);
	return mImages[i];
}
	
//----------------------------------------------------------------------------
inline Image2D* RenderTarget::GetDepthStencilTexture() const
{
	return mspDepthStencilImage;
}

//----------------------------------------------------------------------------
inline Bool RenderTarget::HasMipmaps() const
{
	WIRE_ASSERT(mImages.GetQuantity() > 0);
	return mImages[0]->HasMipmaps();
}

//----------------------------------------------------------------------------
inline Bool RenderTarget::HasDepthStencil() const
{
	return mspDepthStencilImage != NULL;
}
