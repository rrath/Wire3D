// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXRENDERTARGET_H
#define WIREGXRENDERTARGET_H

#include "WireImage2D.h"

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

	inline void Enable(Renderer* pRenderer) {}
	inline void Disable(Renderer* pRenderer) {}
	Image2D* ReadColor(UInt i, Renderer* pRenderer);
	inline UInt GetBufferSize() { return 0; }

private:
	Image2D::FormatMode mFormat;
};

}

#endif
