// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UChar* Image2D::GetData() const
{
	return mpData;
}

//----------------------------------------------------------------------------
inline Image2D::FormatMode Image2D::GetFormat() const
{
	return mFormat;
}

//----------------------------------------------------------------------------
inline UInt Image2D::GetQuantity(UInt level) const
{
	if (level == 0)
	{
		return mBound[0] * mBound[1];
	}
	else
	{
		return GetBound(0, level) * GetBound(1, level);
	}
}

//----------------------------------------------------------------------------
inline Bool Image2D::HasAlpha() const
{
	return mFormat == FM_RGBA8888 || mFormat == FM_RGBA4444;
}

//----------------------------------------------------------------------------
inline Bool Image2D::HasMipmaps() const
{
	return mMipmapCount > 1;
}

//----------------------------------------------------------------------------
inline UInt Image2D::GetMipmapCount() const
{
	return mMipmapCount;
}

//----------------------------------------------------------------------------
inline Bool Image2D::IsPowerOfTwo(UInt value) const
{
	return ((value & (value-1)) == 0);
}

//----------------------------------------------------------------------------
inline UInt Image2D::GetBytesPerPixel() const
{
	return GetBytesPerPixel(mFormat);
}

//----------------------------------------------------------------------------
inline UInt Image2D::GetBytesPerPixel(FormatMode format)
{
	return s_ImageBpp[format];
}

//----------------------------------------------------------------------------
inline Bool Image2D::HasAlpha(FormatMode format)
{
	return s_FormatHasAlpha[format];
}

//----------------------------------------------------------------------------
inline void Image2D::RGB888ToRGB565(UChar* pSrc888, UChar* pDst565)
{
	UShort rgb565 = static_cast<UShort>(*pSrc888++);		// red
	rgb565 &= 0xF8;
	rgb565 = rgb565 << 8;

	UShort component = static_cast<UShort>(*pSrc888++);		// green
	component &= 0xFC;
	rgb565 |= component << 3;

	component = static_cast<UShort>(*pSrc888);				// blue
	component &= 0xF8;
	rgb565 |= component >> 3;

	*pDst565++ = static_cast<UChar>(rgb565 >> 8);
	*pDst565 = static_cast<UChar>(rgb565);
}

//----------------------------------------------------------------------------
inline void Image2D::RGBA8888ToRGBA4444(UChar* pSrc8888, UChar* pDst4444)
{
	UShort rgba4444 = static_cast<UShort>(*pSrc8888++);		// red
	rgba4444 &= 0xF0;
	rgba4444 = rgba4444 << 8;

	UShort component = static_cast<UShort>(*pSrc8888++);	// green
	component &= 0xF0;
	rgba4444 |= component << 4;

	component = static_cast<UShort>(*pSrc8888++);			// blue
	component &= 0xF0;
	rgba4444 |= component;

	component = static_cast<UShort>(*pSrc8888);				// alpha
	rgba4444 |= component >> 4;

	*pDst4444++ = static_cast<UChar>(rgba4444 >> 8);
	*pDst4444 = static_cast<UChar>(rgba4444);
}

//----------------------------------------------------------------------------
inline void Image2D::RGB565ToRGB888(UChar* pSrc565, UChar* pDst888)
{
	*pDst888++ = *pSrc565 & 0xF8;
	UChar gHigh = (*pSrc565++ & 0x07) << 5;
	UChar gLow = (*pSrc565 & 0xE0) >> 3;
	*pDst888++ = gHigh | gLow;
	*pDst888++ = (*pSrc565 & 0x1F) << 3;
}

//----------------------------------------------------------------------------
inline void Image2D::RGBA4444ToRGBA8888(UChar* pSrc4444, UChar* pDst8888)
{
	*pDst8888++ = *pSrc4444 & 0xF0;
	*pDst8888++ = (*pSrc4444++ & 0x0F) << 4;
	*pDst8888++ = *pSrc4444 & 0xF0;
	*pDst8888++ = (*pSrc4444 & 0x0F) << 4;
}

//----------------------------------------------------------------------------
inline Image2D* Image2D::GetDefault()
{
	return s_spDefault;
}

//----------------------------------------------------------------------------
inline Image2D* Image2D::GetDefaultWithAlpha()
{
	return s_spDefaultWithAlpha;
}
