// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETEXTURE2D_H
#define WIRETEXTURE2D_H

#include "WireObject.h"

namespace Wire
{

class Image2D;

class Texture2D : public Object
{
	WIRE_DECLARE_RTTI;

public:
	enum FilterType
	{
		FT_NEAREST,          // nearest neighbor
		FT_LINEAR,           // linear filtering
		FT_NEAREST_NEAREST,  // nearest within image, nearest across images
		FT_NEAREST_LINEAR,   // nearest within image, linear across images
		FT_LINEAR_NEAREST,   // linear within image, nearest across images
		FT_LINEAR_LINEAR,    // linear within image, linear across images
		FT_QUANTITY
	};

	enum WrapType
	{
		WT_CLAMP,
		WT_REPEAT,
		WT_MIRRORED_REPEAT,
		WT_QUANTITY
	};

	Texture2D(Image2D* pImage);
	virtual ~Texture2D();

	inline Image2D* GetImage();
	inline const Image2D* GetImage() const;
	void SetImage(Image2D* pImage);

	// Access to filter modes. The default is LINEAR.
	inline void SetFilterType(FilterType filterType);
	inline FilterType GetFilterType() const;

	// Access to wrap modes. The defaults are WT_CLAMP.
	inline void SetWrapType(UInt i, WrapType wrapType);
	inline WrapType GetWrapType(UInt i) const;

	// Access to the anisotropy value for image filtering.
	inline void SetAnisotropyValue(Float anisotropy);
	inline Float GetAnisotropyValue() const;

private:
	Pointer<Image2D> mspImage;
	FilterType mFilterType;	// default = FT_LINEAR_LINEAR
	WrapType mWarpType[2];	// default = WT_CLAMP
	Float mAnisotropy;		// default = 1.0F
};

typedef Pointer<Texture2D> Texture2DPtr;
#include "WireTexture2D.inl"

}

#endif
