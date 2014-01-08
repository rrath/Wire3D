// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRERENDERTARGET_H
#define WIRERENDERTARGET_H

#include "WireImage2D.h"

namespace Wire
{

class RenderTarget : public Object
{
	WIRE_DECLARE_RTTI;

public:
	// The number of supported targets depends on the graphics hardware
	// and drivers. Use the Renderer's GetMaxSimultaneousRenderTargets() to
	// determine the number. 'targetQuantity' must be at least 1.
	RenderTarget(UInt targetQuantity, Image2D::FormatMode format, UInt width,
		UInt height, Bool hasMipmaps, Bool hasDepthStencil);
	virtual ~RenderTarget();

	inline UInt GetTargetQuantity() const;
	inline Image2D::FormatMode GetFormat() const;
	inline UInt GetWidth() const;
	inline UInt GetHeight() const;
	inline Image2D* GetColorTexture(UInt i = 0) const;
	inline Image2D* GetDepthStencilTexture() const;
	inline Bool HasMipmaps() const;
	inline Bool HasDepthStencil() const;

private:
	TArray<Image2DPtr> mImages;
	Image2DPtr mspDepthStencilImage;
	Image2D::FormatMode mFormat;
};

typedef Pointer<RenderTarget> RenderTargetPtr;
#include "WireRenderTarget.inl"

}

#endif
