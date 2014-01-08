// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireGXRenderTarget.h"

#include "WireRenderTarget.h"

using namespace Wire;

//----------------------------------------------------------------------------
PdrRenderTarget::PdrRenderTarget(Renderer*, const RenderTarget* pRenderTarget)
{
	mFormat = pRenderTarget->GetFormat();
}

//----------------------------------------------------------------------------
PdrRenderTarget::~PdrRenderTarget()
{
}

//----------------------------------------------------------------------------
Image2D* PdrRenderTarget::ReadColor(UInt, Renderer*)
{
	if (Image2D::HasAlpha(mFormat))
	{
		return Image2D::GetDefaultWithAlpha();
	}

	return Image2D::GetDefault();
}
