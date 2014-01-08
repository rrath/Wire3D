// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireImage2D.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Image2D, Buffer);
WIRE_IMPLEMENT_INITIALIZE(Image2D);
WIRE_IMPLEMENT_TERMINATE(Image2D);

const UChar Image2D::s_ImageBpp[] =
{
	3,	// FM_RGB888,
	4,	// FM_RGBA8888,
	2,	// FM_RGB565,
	2,	// FM_RGBA4444,
	4	// FM_D24S8
};

const Bool Image2D::s_FormatHasAlpha[] =
{
	false,	// FM_RGB888,
	true,	// FM_RGBA8888,
	false,	// FM_RGB565,
	true,	// FM_RGBA4444,
	false	// FM_D24S8
};

Image2DPtr Image2D::s_spDefault;
Image2DPtr Image2D::s_spDefaultWithAlpha;

const UChar Image2D::s_Default[] =
{
	0xFF, 0xFF, 0xFF,  0xFF, 0x00, 0x00,
	0xFF, 0x00, 0x00,  0xFF, 0xFF, 0xFF
};

const UChar Image2D::s_DefaultWithAlpha[] =
{
	0xFF, 0xFF, 0xFF, 0x7F,  0xFF, 0x00, 0x00, 0x7F,
	0xFF, 0x00, 0x00, 0x7F,  0xFF, 0xFF, 0xFF, 0x7F
};

//----------------------------------------------------------------------------
void Image2D::Initialize()
{
	const UInt width = 2;
	const UInt height = 2;

	FormatMode format = FM_RGB888;
	s_spDefault = WIRE_NEW Image2D(format, width, height,
		const_cast<UChar*>(s_Default), false, Buffer::UT_STATIC, 1);

	format = FM_RGBA8888;
	s_spDefaultWithAlpha = WIRE_NEW Image2D(format, width, height,
		const_cast<UChar*>(s_DefaultWithAlpha), false, Buffer::UT_STATIC, 1);
}

//----------------------------------------------------------------------------
void Image2D::Terminate()
{
	s_spDefault = NULL;
	s_spDefaultWithAlpha = NULL;
}

//----------------------------------------------------------------------------
Image2D::Image2D(FormatMode format, UInt width, UInt height, UChar* pData,
	Bool filterMipmaps, UsageType usage, UInt mipmapCount)
	:
	Buffer(usage),
	mFormat(format),
	mpData(pData),
	mMipmapCount(mipmapCount)
{
	mBound[0] = width;
	mBound[1] = height;

	if (mipmapCount == 0)
	{
		mMipmapCount = GetMipmapCount(width, height);
	}

	if (filterMipmaps)
	{
		FilterMipmaps();
	}
}

//----------------------------------------------------------------------------
Image2D::~Image2D()
{
	Discard();
}

//----------------------------------------------------------------------------
UInt Image2D::GetBound(UInt i, UInt level) const
{
	WIRE_ASSERT(i < 2);
	if (GetMipmapCount() <= level)
	{
		WIRE_ASSERT(false);
		return 0;
	}

	UInt width = mBound[0];
	UInt height = mBound[1];
	UInt currentLevel = 0;

	while ((width > 1 || height > 1) && (currentLevel < level))
	{
		width = width > 1 ? width >> 1 : 1;
		height = height > 1 ? height >> 1 : 1;
		currentLevel++;
	}

	switch (i)
	{
	case 0: return width;
	case 1: return height;
	default: WIRE_ASSERT(false);
		return 0;
	}
}

//----------------------------------------------------------------------------
UInt Image2D::GetMipmapQuantity(UInt level) const
{
	WIRE_ASSERT(level < GetMipmapCount());
	return GetMipmapQuantity(level, mBound[0], mBound[1]);
}

//----------------------------------------------------------------------------
UInt Image2D::GetMipmapQuantity(UInt level, UInt width, UInt height)
{
	UInt currentLevel = 0;

	while ((width > 1 || height > 1) && currentLevel < level)
	{
		currentLevel++;
		width = width > 1 ? width >> 1 : width;
		height = height > 1 ? height >> 1 : height;
	}

	return width * height;
}

//----------------------------------------------------------------------------
UInt Image2D::GetTotalQuantity() const
{
	UInt totalQuantity = 0;
	for (UInt i = 0; i < GetMipmapCount(); i++)
	{
		totalQuantity += GetMipmapQuantity(i);
	}

	return totalQuantity;
}

//----------------------------------------------------------------------------
UInt Image2D::GetTotalQuantity(UInt width, UInt height)
{
	UInt totalQuantity = 0;
	UInt mipmapCount = GetMipmapCount(width, height);
	for (UInt i = 0; i < mipmapCount; i++)
	{
		totalQuantity += GetMipmapQuantity(i, width, height);
	}

	return totalQuantity;
}

//----------------------------------------------------------------------------
UInt Image2D::GetMipmapOffset(UInt level) const
{
	if ((!HasMipmaps() && level > 0) || (GetMipmapCount() <= level))
	{
		return 0;
	}

	UInt width = mBound[0];
	UInt height = mBound[1];
	UInt bpp = GetBytesPerPixel();
	UInt offset = 0;
	UInt currentLevel = 0;

	while ((width > 1 || height > 1) && currentLevel < level)
	{
		offset += width * height * bpp;
		currentLevel++;
		width = width > 1 ? width >> 1 : width;
		height = height > 1 ? height >> 1 : height;
	}

	return offset;
}

//----------------------------------------------------------------------------
UChar* Image2D::GetMipmap(UInt level) const
{
	if ((!HasMipmaps() && level > 0) || (GetMipmapCount() <= level))
	{
		return NULL;
	}

	return mpData + GetMipmapOffset(level);
}

//----------------------------------------------------------------------------
UInt Image2D::GetMipmapCount(UInt width, UInt height)
{
	UInt mipmapCount = 1;
	while (width > 1 || height > 1)
	{
		mipmapCount++;
		width = width >> 1;
		height = height >> 1;
	}

	return mipmapCount;
}

//----------------------------------------------------------------------------
void Image2D::FilterMipmaps()
{
	UInt width = mBound[0];
	UInt height = mBound[1];

	if (!mpData || (mFormat == FM_D24S8) ||
		!(IsPowerOfTwo(width) && width > 0) ||
		!(IsPowerOfTwo(height) && height > 0))
	{
		return;
	}

	UInt bpp = GetBytesPerPixel();
	UInt totalQuantity = 0;
	UInt currentLevel = 0;

	while ((width > 1 || height > 1) && (currentLevel < GetMipmapCount()))
	{
		totalQuantity += width * height * bpp;
		currentLevel++;
		width = width > 1 ? width >> 1 : width;
		height = height > 1 ? height >> 1 : height;
	}

	if (width == 1 && height == 1)
	{
		totalQuantity += bpp;
	}

	UChar* pData = WIRE_NEW UChar[totalQuantity];
	UInt quantity = mBound[0]*mBound[1] * bpp;
	System::Memcpy(pData, quantity, mpData, quantity);
	WIRE_DELETE[] mpData;
	mpData = pData;

	width = mBound[0];
	height = mBound[1];
	currentLevel = 1;

	while ((width > 1 || height > 1) && (currentLevel < GetMipmapCount()))
	{
		UInt mipmapSize = width * height * bpp;
		FilterMipmap(pData, pData + mipmapSize, width, height);
		pData += mipmapSize;
		width = width > 1 ? width >> 1 : width;
		height = height > 1 ? height >> 1 : height;
		currentLevel++;
	}
}

//----------------------------------------------------------------------------
void Image2D::FilterMipmap(UChar* pSrc, UChar* pDst, UInt width, UInt height)
{
	if (width == 1 && height == 1)
	{
		return;
	}

	if (width == 1 || height == 1)
	{
		FilterMipmap1(pSrc, pDst, width, height);
		return;
	}

	UInt bpp = GetBytesPerPixel();

	for (UInt y = 0; y < height; y+=2)
	{
		for (UInt x = 0; x < width; x+=2)
		{
			UChar box[4][4];
			UInt offset = (y*width+x)*bpp;

			switch (mFormat)
			{
			case FM_RGBA8888:
				box[0][0] = pSrc[offset];
				box[0][1] = pSrc[offset+1];
				box[0][2] = pSrc[offset+2];
				box[0][3] = pSrc[offset+3];

				box[1][0] = pSrc[offset+4];
				box[1][1] = pSrc[offset+5];
				box[1][2] = pSrc[offset+6];
				box[1][3] = pSrc[offset+7];

				offset += width*bpp;
				box[2][0] = pSrc[offset];
				box[2][1] = pSrc[offset+1];
				box[2][2] = pSrc[offset+2];
				box[2][3] = pSrc[offset+3];

				box[3][0] = pSrc[offset+4];
				box[3][1] = pSrc[offset+5];
				box[3][2] = pSrc[offset+6];
				box[3][3] = pSrc[offset+7];
				break;

			case FM_RGB888:
				box[0][0] = pSrc[offset];
				box[0][1] = pSrc[offset+1];
				box[0][2] = pSrc[offset+2];

				box[1][0] = pSrc[offset+3];
				box[1][1] = pSrc[offset+4];
				box[1][2] = pSrc[offset+5];

				offset += width*bpp;
				box[2][0] = pSrc[offset];
				box[2][1] = pSrc[offset+1];
				box[2][2] = pSrc[offset+2];

				box[3][0] = pSrc[offset+3];
				box[3][1] = pSrc[offset+4];
				box[3][2] = pSrc[offset+5];
				break;

			case FM_RGBA4444:
				RGBA4444ToRGBA8888(pSrc+offset, box[0]);
				RGBA4444ToRGBA8888(pSrc+offset+bpp, box[1]);
				offset += width*bpp;
				RGBA4444ToRGBA8888(pSrc+offset, box[2]);
				RGBA4444ToRGBA8888(pSrc+offset+bpp, box[3]);
				break;

			case FM_RGB565:
				RGB565ToRGB888(pSrc+offset, box[0]);
				RGB565ToRGB888(pSrc+offset+bpp, box[1]);
				offset += width*bpp;
				RGB565ToRGB888(pSrc+offset, box[2]);
				RGB565ToRGB888(pSrc+offset+bpp, box[3]);
				break;

			default:
				WIRE_ASSERT(false);
			}

			for (UInt j = 0; j < 4; j++)
			{
				UShort component = 0;
				for (UInt i = 0; i < 4; i++)
				{
					component = component + static_cast<UShort>(box[i][j]);
				}

				component = component >> 2;
				box[0][j] = static_cast<UChar>(component);
			}

			switch (mFormat)
			{
			case FM_RGB888:
				*pDst++ = box[0][0];
				*pDst++ = box[0][1];
				*pDst++ = box[0][2];
				break;

			case FM_RGBA8888:
				*pDst++ = box[0][0];
				*pDst++ = box[0][1];
				*pDst++ = box[0][2];
				*pDst++ = box[0][3];
				break;

			case FM_RGBA4444:
				RGBA8888ToRGBA4444(box[0], pDst);
				pDst += bpp;
				break;

			case FM_RGB565:
				RGB888ToRGB565(box[0], pDst);
				pDst += bpp;
				break;

			default:
				WIRE_ASSERT(false);
			}
		}
	}
}

//----------------------------------------------------------------------------
void Image2D::FilterMipmap1(UChar* pSrc, UChar* pDst, UInt width, UInt height)
{
	UInt bpp = GetBytesPerPixel();

	for (UInt y = 0; y < height; y+=2)
	{
		for (UInt x = 0; x < width; x+=2)
		{
			UChar box[4][4];
			UInt offset = (y*width+x)*bpp;
			UShort c = 0;

			switch (mFormat)
			{
			case FM_RGBA8888:
				box[c][0] = pSrc[offset];
				box[c][1] = pSrc[offset+1];
				box[c][2] = pSrc[offset+2];
				box[c++][3] = pSrc[offset+3];

				if (width > 1)
				{
					box[c][0] = pSrc[offset+4];
					box[c][1] = pSrc[offset+5];
					box[c][2] = pSrc[offset+6];
					box[c++][3] = pSrc[offset+7];
				}
				else if (height > 1)
				{
					offset += width*bpp;
					box[c][0] = pSrc[offset];
					box[c][1] = pSrc[offset+1];
					box[c][2] = pSrc[offset+2];
					box[c++][3] = pSrc[offset+3];
				}
				else
				{
					WIRE_ASSERT(false);
				}

				break;

			case FM_RGB888:
				box[c][0] = pSrc[offset];
				box[c][1] = pSrc[offset+1];
				box[c++][2] = pSrc[offset+2];

				if (width > 1)
				{
					box[c][0] = pSrc[offset+3];
					box[c][1] = pSrc[offset+4];
					box[c++][2] = pSrc[offset+5];
				}
				else if (height > 1)
				{
					offset += width*bpp;
					box[c][0] = pSrc[offset];
					box[c][1] = pSrc[offset+1];
					box[c++][2] = pSrc[offset+2];
				}
				else
				{
					WIRE_ASSERT(false);
				}

				break;

			case FM_RGBA4444:
				RGBA4444ToRGBA8888(pSrc+offset, box[c++]);
				if (width > 1)
				{
					RGBA4444ToRGBA8888(pSrc+offset+bpp, box[c++]);
				}
				else if (height > 1)
				{
					offset += width*bpp;
					RGBA4444ToRGBA8888(pSrc+offset, box[c++]);
				}
				else
				{
					WIRE_ASSERT(false);
				}

				break;

			case FM_RGB565:
				RGB565ToRGB888(pSrc+offset, box[c++]);
				if (width > 1)
				{
					RGB565ToRGB888(pSrc+offset+bpp, box[c++]);
				}
				else if (height > 1)
				{
					offset += width*bpp;
					RGB565ToRGB888(pSrc+offset, box[c++]);
				}
				else
				{
					WIRE_ASSERT(false);
				}

				break;

			default:
				WIRE_ASSERT(false);
			}

			for (UInt j = 0; j < 4; j++)
			{
				UShort component = 0;
				for (UInt i = 0; i < c; i++)
				{
					component = component + static_cast<UShort>(box[i][j]);
				}

				component = component / c;
				box[0][j] = static_cast<UChar>(component);
			}

			switch (mFormat)
			{
			case FM_RGB888:
				*pDst++ = box[0][0];
				*pDst++ = box[0][1];
				*pDst++ = box[0][2];
				break;

			case FM_RGBA8888:
				*pDst++ = box[0][0];
				*pDst++ = box[0][1];
				*pDst++ = box[0][2];
				*pDst++ = box[0][3];
				break;

			case FM_RGBA4444:
				RGBA8888ToRGBA4444(box[0], pDst);
				pDst += bpp;
				break;

			case FM_RGB565:
				RGB888ToRGB565(box[0], pDst);
				pDst += bpp;
				break;

			default:
				WIRE_ASSERT(false);
			}
		}
	}
}

//----------------------------------------------------------------------------
void Image2D::Discard()
{
	if (mpData != s_Default && mpData != s_DefaultWithAlpha)
	{
		WIRE_DELETE[] mpData;

		mBound[0] = 2;
		mBound[1] = 2;
		mMipmapCount = 1;

		if (HasAlpha())
		{
			mpData = const_cast<UChar*>(s_DefaultWithAlpha);
			mFormat = FM_RGBA8888;
		}
		else
		{
			mpData = const_cast<UChar*>(s_Default);
			mFormat = FM_RGB888;
		}
	}
}
