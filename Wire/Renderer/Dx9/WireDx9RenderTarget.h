// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREDX9RENDERTARGET_H
#define WIREDX9RENDERTARGET_H

#include "WireImage2D.h"
#include "WireTArray.h"

struct IDirect3DSurface9;
struct IDirect3DTexture9;

namespace Wire
{

class PdrTexture2D;
class Renderer;
class RenderTarget;

class PdrRenderTarget
{
public:
	PdrRenderTarget(Renderer* pRenderer, const RenderTarget* pRenderTarget);
	~PdrRenderTarget();

	void Enable(Renderer* pRenderer);
	void Disable(Renderer* pRenderer);
	Image2D* ReadColor(UInt i, Renderer* pRenderer);
	inline UInt GetBufferSize() { return mBufferSize; }

private:
	Bool IsDepthFormatSupported(Renderer* pRenderer) const;

	UInt mWidth;
	UInt mHeight;
	Image2D::FormatMode mFormat;
	Bool mHasDepthStencil;

	TArray<IDirect3DTexture9*> mColorTextures;
	TArray<IDirect3DSurface9*> mColorSurfaces;
	IDirect3DTexture9* mpDepthStencilTexture;
	IDirect3DSurface9* mpDepthStencilSurface;

	UInt mBufferSize;

	// save/restore the render target 0, i.e. the backbuffer
	IDirect3DSurface9* mpSaveColorSurface;
	IDirect3DSurface9* mpSaveDepthStencilSurface;
};

}

#endif
