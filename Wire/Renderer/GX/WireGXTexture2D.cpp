// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireGXTexture2D.h"

#include "WireGXRendererData.h"
#include "WireImage2D.h"
#include "WireRenderer.h"
#include "WireTexture2D.h"
#include <malloc.h>

using namespace Wire;

const UChar PdrRendererData::IMAGE2D_FORMAT[Image2D::FM_QUANTITY] =
{
	GX_TF_RGBA8,	// Image2D::FM_RGB888
	GX_TF_RGBA8,	// Image2D::FM_RGBA8888
	GX_TF_RGB565,	// Image2D::FM_RGB565
	GX_TF_RGB5A3,	// Image2D::FM_RGBA4444
	GX_TF_Z24X8		// Image2D::FM_D24S8
};

const UChar PdrRendererData::TEX_MIN_FILTER[Texture2D::FT_QUANTITY] =
{
	GX_NEAR,			// Texture2D::FT_NEAREST
	GX_LINEAR,			// Texture2D::FT_LINEAR
	GX_NEAR_MIP_NEAR,	// Texture2D::FT_NEAREST_NEAREST
	GX_NEAR_MIP_LIN,	// Texture2D::FT_NEAREST_LINEAR
	GX_LIN_MIP_NEAR,	// Texture2D::FT_LINEAR_NEAREST
	GX_LIN_MIP_LIN,		// Texture2D::FT_LINEAR_LINEAR
};

const UChar PdrRendererData::TEX_WRAP_MODE[Texture2D::WT_QUANTITY] =
{
	GX_CLAMP,	// Texture2D::WT_CLAMP
	GX_REPEAT,	// Texture2D::WT_REPEAT
	GX_MIRROR,	// Texture2D::WT_MIRRORED_REPEAT
};

//----------------------------------------------------------------------------
PdrTexture2D::PdrTexture2D(Renderer* pRenderer, const Image2D* pImage)
{
	WIRE_ASSERT(pImage);

	UInt bpp = pImage->GetBytesPerPixel();
	bpp = (bpp == 3) ? 4 : bpp;
	mBufferSize = PdrRendererData::GetTotalImageMemory(pImage, bpp);

	mpBuffer = memalign(32, mBufferSize);
	UChar* pDst = static_cast<UChar*>(mpBuffer);
	WIRE_ASSERT(pDst);
	Image2D::FormatMode format = pImage->GetFormat();

	for (UInt mipLevel = 0; mipLevel < pImage->GetMipmapCount(); mipLevel++)
	{
		UChar* pSrc = pImage->GetMipmap(mipLevel);
		UShort width = pImage->GetBound(0, mipLevel);
		UShort height = pImage->GetBound(1, mipLevel);

		if (pSrc)
		{
			switch (format)
			{
			case Image2D::FM_RGBA8888:
				PdrRendererData::ConvertRGBA8888ToTiles(pSrc, width, height,
					pDst);
				break;

			case Image2D::FM_RGB888:
				PdrRendererData::ConvertRGB888ToTiles(pSrc, width, height,
					pDst);
				break;

			case Image2D::FM_RGB565:
				PdrRendererData::ConvertRGB565ToTiles(pSrc, width, height,
					pDst);
				break;

			case Image2D::FM_RGBA4444:
				PdrRendererData::ConvertRGBA4444ToTiles(pSrc, width, height,
					pDst);
				break;

			default:
				break;
			}
		}

		UInt offset = width * height * bpp;
		if (bpp == 4)
		{
			pDst += (offset < 64) ? 64 : offset;
		}
		else
		{
			pDst += (offset < 32) ? 32 : offset;
		}
	}

	DCStoreRange(mpBuffer, mBufferSize);
	GXInvalidateTexAll();

	const UShort width = pImage->GetBound(0);
	WIRE_ASSERT(width <= pRenderer->GetMaxTextureWidth());
	const UShort height = pImage->GetBound(1);
	WIRE_ASSERT(height <= pRenderer->GetMaxTextureHeight());
	const UChar usesMipmaps = pImage->HasMipmaps() ? GX_TRUE : GX_FALSE;
	GXInitTexObj(&mTexObj, mpBuffer, width, height,
		PdrRendererData::IMAGE2D_FORMAT[format], GX_REPEAT, GX_REPEAT,
		usesMipmaps);
}

//----------------------------------------------------------------------------
PdrTexture2D::~PdrTexture2D()
{
	free(mpBuffer);	// allocated using memalign, not using new
}

//----------------------------------------------------------------------------
void PdrTexture2D::Enable(Renderer* pRenderer, const Texture2D* pTexture,
	UInt unit)
{
	WIRE_ASSERT(unit < pRenderer->GetMaxTextureStages());

	GXSetTexCoordGen(GX_TEXCOORD0 + unit, GX_TG_MTX2x4, GX_TG_TEX0 + unit,
		GX_IDENTITY);

	UChar color = unit > 0 ? GX_COLOR_NULL : GX_COLOR0A0;
	GXSetTevOrder(GX_TEVSTAGE0 + unit, GX_TEXCOORD0 + unit, GX_TEXMAP0 + unit,
		color);

	GXInitTexObjWrapMode(&mTexObj,
		PdrRendererData::TEX_WRAP_MODE[pTexture->GetWrapType(0)],
		PdrRendererData::TEX_WRAP_MODE[pTexture->GetWrapType(1)]);

	Float anisotropy = static_cast<UInt>(pTexture->GetAnisotropyValue());
	UChar anisoEnum = GX_ANISO_1;
	UChar doEdgeLod = GX_FALSE;
	if (1.0F < anisotropy && anisotropy <= pRenderer->GetMaxAnisotropy())
	{
		anisoEnum = (anisotropy - 3.0F) > 0.0F ? GX_ANISO_4 : GX_ANISO_2;
		doEdgeLod = GX_TRUE;
	}

	UChar magFilter = pTexture->GetFilterType() == Texture2D::FT_NEAREST ?
		GX_NEAR : GX_LINEAR;
	GXInitTexObjLOD(&mTexObj, PdrRendererData::TEX_MIN_FILTER[pTexture->
		GetFilterType()], magFilter, 0, 10.0F, 0, GX_FALSE, doEdgeLod,
		anisoEnum);

	// set texture
	GXLoadTexObj(&mTexObj, GX_TEXMAP0 + unit);
}

//----------------------------------------------------------------------------
void PdrTexture2D::Disable(Renderer* pRenderer, UInt unit)
{
}
