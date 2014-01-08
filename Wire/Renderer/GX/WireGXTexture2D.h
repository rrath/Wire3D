// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXTEXTURE2D_H
#define WIREGXTEXTURE2D_H

#include "WireTypes.h"
#include "Wii/WireOgcDefines.h"

namespace Wire
{

class Image2D;
class Renderer;
class Texture2D;

class PdrTexture2D
{
public:
	PdrTexture2D(Renderer* pRenderer, const Image2D* pImage);
 	~PdrTexture2D();
 
 	void Enable(Renderer* pRenderer, const Texture2D* pTexture, UInt unit);
 	void Disable(Renderer* pRenderer, UInt unit);

	inline UInt GetBufferSize() const { return mBufferSize; }

private:
	void* mpBuffer;
	GXTexObj mTexObj;
	UInt mBufferSize;
};

}

#endif
