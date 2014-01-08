// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderTarget.h"

#include "WireRenderer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, RenderTarget, Object);

//----------------------------------------------------------------------------
RenderTarget::RenderTarget(UInt targetQuantity, Image2D::FormatMode format,
	UInt width, UInt height, Bool hasMipmaps, Bool hasDepthStencil)
{
	WIRE_ASSERT(targetQuantity > 0 /* Number of targets must be at least 1 */);
	for (UInt i = 0; i < targetQuantity; i++)
	{
		Image2D* pImage = WIRE_NEW Image2D(format, width, height, NULL, false,
			Buffer::UT_DYNAMIC, (hasMipmaps ? 0 : 1));
		mImages.Append(pImage);
	}

	if (hasDepthStencil)
	{
		mspDepthStencilImage = WIRE_NEW Image2D(Image2D::FM_D24S8, width,
			height, NULL, false, Buffer::UT_DYNAMIC, 1);
	}
}

//----------------------------------------------------------------------------
RenderTarget::~RenderTarget()
{
	Renderer::UnbindAll(this);
	mImages.RemoveAll();
}
