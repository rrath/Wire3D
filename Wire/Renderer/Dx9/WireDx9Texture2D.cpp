// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireDx9Texture2D.h"

#include "WireDx9RendererData.h"
#include "WireRenderer.h"
#include "WireTexture2D.h"

using namespace Wire;

const D3DFORMAT PdrRendererData::IMAGE2D_FORMAT[Image2D::FM_QUANTITY] =
{
	D3DFMT_A8R8G8B8,	// Image2D::FM_RGB888
	D3DFMT_A8R8G8B8,	// Image2D::FM_RGBA8888
	D3DFMT_R5G6B5,		// Image2D::FM_RGB565
	D3DFMT_A4R4G4B4,	// Image2D::FM_RGBA4444
	D3DFMT_D24S8        // Image2D::FM_D24S8
};

const DWORD PdrRendererData::StateSampler::TEX_MIN_FILTER[Texture2D::
	FT_QUANTITY] =
{
	D3DTEXF_POINT,  // Texture2D::FT_NEAREST
	D3DTEXF_LINEAR, // Texture2D::FT_LINEAR
	D3DTEXF_POINT,  // Texture2D::FT_NEAREST_NEAREST
	D3DTEXF_POINT,  // Texture2D::FT_NEAREST_LINEAR
	D3DTEXF_LINEAR, // Texture2D::FT_LINEAR_NEAREST
	D3DTEXF_LINEAR, // Texture2D::FT_LINEAR_LINEAR
};

const DWORD PdrRendererData::StateSampler::TEX_MIP_FILTER[Texture2D::
	FT_QUANTITY] =
{
	D3DTEXF_NONE,   // Texture2D::FT_NEAREST
	D3DTEXF_NONE,   // Texture2D::FT_LINEAR
	D3DTEXF_POINT,  // Texture2D::FT_NEAREST_NEAREST
	D3DTEXF_LINEAR, // Texture2D::FT_NEAREST_LINEAR
	D3DTEXF_POINT,  // Texture2D::FT_LINEAR_NEAREST
	D3DTEXF_LINEAR, // Texture2D::FT_LINEAR_LINEAR
};

const DWORD PdrRendererData::StateSampler::TEX_WRAP_MODE[Texture2D::
	WT_QUANTITY] =
{
	D3DTADDRESS_CLAMP,      // Texture2D::WT_CLAMP
	D3DTADDRESS_WRAP,       // Texture2D::WT_REPEAT
	D3DTADDRESS_MIRROR,     // Texture2D::WT_MIRRORED_REPEAT
};

//----------------------------------------------------------------------------
PdrTexture2D::PdrTexture2D(Renderer* pRenderer, const Image2D* pImage)
{
	// Copy the image data from system memory to video memory.
	WIRE_ASSERT(pImage);

	if (!pImage->GetData())
	{
		CreateRenderTarget(pRenderer, pImage);
		return;
	}

	// Windows stores BGR (lowest byte to highest byte), but Wire stores RGB.
	// The byte ordering must be reversed.
	UChar* const pSrc = pImage->GetData();
	UChar* pDst = NULL;
	UInt quantity = pImage->GetTotalQuantity();
	UInt bpp = pImage->GetBytesPerPixel();
	bpp = (bpp == 3) ? 4 : bpp;
	UInt byteQuantity = quantity * bpp;
	Image2D::FormatMode format = pImage->GetFormat();

	if (pSrc)
	{
		UInt srcOffset = 0;
		UInt dstOffset = 0;

		switch (format)
		{
		case Image2D::FM_RGB888:
			// Swap R and B and pad to an RGBA image.
			pDst = WIRE_NEW UChar[byteQuantity];
			for (UInt i = 0; i < quantity; i++)
			{
				pDst[dstOffset    ] = pSrc[srcOffset + 2];
				pDst[dstOffset + 1] = pSrc[srcOffset + 1];
				pDst[dstOffset + 2] = pSrc[srcOffset    ];
				pDst[dstOffset + 3] = 0xFF;
				srcOffset += 3;
				dstOffset += 4;
			}

			break;

		case Image2D::FM_RGBA8888:
			pDst = WIRE_NEW UChar[byteQuantity];
			for (UInt i = 0; i < quantity; i++)
			{
				pDst[srcOffset    ] = pSrc[srcOffset + 2];
				pDst[srcOffset + 1] = pSrc[srcOffset + 1];
				pDst[srcOffset + 2] = pSrc[srcOffset    ];
				pDst[srcOffset + 3] = pSrc[srcOffset + 3];
				srcOffset += 4;
			}

			break;

		case Image2D::FM_RGB565:
			pDst = WIRE_NEW UChar[byteQuantity];
			for (UInt i = 0; i < quantity; i++)
			{
				pDst[srcOffset] = pSrc[srcOffset+1];
				pDst[srcOffset+1] = pSrc[srcOffset];
				srcOffset += 2;
			}

			break;

		case Image2D::FM_RGBA4444:
			pDst = WIRE_NEW UChar[byteQuantity];
			for (UInt i = 0; i < quantity; i++)
			{
				UShort texel = static_cast<UShort>(pSrc[srcOffset]);
				texel = texel << 8;
				texel |= static_cast<UShort>(pSrc[srcOffset+1]);
				UShort texelRGB = (texel & 0xFFF0) >> 4;
				UShort texelA = (texel & 0x0F) << 12;
				texel = texelRGB | texelA;
				pDst[srcOffset] = static_cast<UChar>(texel);
				pDst[srcOffset+1] = static_cast<UChar>(texel >> 8);
				srcOffset += 2;
			}

			break;

		default:
			// There is no need to preprocess depth or intensity images. The
			// floating-point formats and the 16/32-bit integer formats are
			// already RGB/RGBA.
			pDst = pSrc;
			break;
		}
	}

	const Bool isStatic = (pImage->GetUsage() == Buffer::UT_STATIC) ||
		(pImage->GetUsage() == Buffer::UT_STATIC_DISCARD_ON_BIND);
	const D3DPOOL pool = isStatic ?	D3DPOOL_MANAGED : D3DPOOL_DEFAULT;
	const UInt mipmapCount = pImage->GetMipmapCount();
	const DWORD usage = 0;

	const UInt width = pImage->GetBound(0);
	WIRE_ASSERT(width <= pRenderer->GetMaxTextureWidth());
	const UInt height = pImage->GetBound(1);
	WIRE_ASSERT(height <= pRenderer->GetMaxTextureHeight());
	HRESULT hr;
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	WIRE_ASSERT(IsTextureFormatSupported(pRenderer, format));
	hr = rDevice->CreateTexture(width, height, mipmapCount, usage,
		PdrRendererData::IMAGE2D_FORMAT[format], pool, &mpBuffer, NULL);
	WIRE_ASSERT(SUCCEEDED(hr));

	mBufferSize = 0;

	if (pDst)
	{
		Buffer::LockingMode lockingMode = isStatic ? Buffer::LM_READ_WRITE :
			Buffer::LM_WRITE_ONLY;

		for (UInt level = 0; level < mipmapCount; ++level)
		{
			void* pData = Lock(lockingMode, level);
			WIRE_ASSERT(pData);
			UInt size = pImage->GetQuantity(level) * bpp;
			UInt offset = pImage->GetMipmapOffset(level) / pImage->
				GetBytesPerPixel();
			System::Memcpy(pData, size, pDst + offset * bpp, size);
			Unlock(level);
			mBufferSize += size;
		}
	}

	if (pSrc != pDst)
	{
		WIRE_DELETE[] pDst;
	}
}

//----------------------------------------------------------------------------
PdrTexture2D::~PdrTexture2D()
{
	mpBuffer->Release();
}

//----------------------------------------------------------------------------
void PdrTexture2D::Enable(Renderer* pRenderer, const Texture2D* pTexture,
	UInt unit)
{
	WIRE_ASSERT(unit < pRenderer->GetMaxTextureStages());

	PdrRendererData* pData = pRenderer->GetRendererData();
	IDirect3DDevice9*& rDevice = pData->D3DDevice;
	HRESULT hr;

	// Anisotropic filtering value.
	Float anisotropy = pTexture->GetAnisotropyValue();
	if (anisotropy > pRenderer->GetMaxAnisotropy())
	{
		anisotropy = pRenderer->GetMaxAnisotropy();
	}
	else if (anisotropy < 1.0F)
	{
		anisotropy = 1.0F;
	}

	PdrRendererData::StateSampler& rState = pRenderer->GetRendererData()->
		SamplerStates[unit];

	if (!rState.IsValid)
	{
		hr = rDevice->SetSamplerState(unit, D3DSAMP_MAXANISOTROPY, rState.
			MAXANISOTROPY);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetSamplerState(unit, D3DSAMP_MAGFILTER, rState.
			MAGFILTER);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetSamplerState(unit, D3DSAMP_MINFILTER, rState.
			MINFILTER);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetSamplerState(unit, D3DSAMP_MIPFILTER, rState.
			MIPFILTER);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetSamplerState(unit, D3DSAMP_BORDERCOLOR, rState.
			BORDERCOLOR);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetSamplerState(unit, D3DSAMP_ADDRESSU, rState.
			ADDRESSU);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetSamplerState(unit, D3DSAMP_ADDRESSV, rState.
			ADDRESSV);
		WIRE_ASSERT(SUCCEEDED(hr));

		rState.IsValid = true;
	}

	DWORD dAnisotropy = static_cast<DWORD>(anisotropy);
	if (rState.MAXANISOTROPY != dAnisotropy)
	{
		hr = rDevice->SetSamplerState(unit, D3DSAMP_MAXANISOTROPY,
			dAnisotropy);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.MAXANISOTROPY = dAnisotropy;
	}

	// Set the filter mode.
	Texture2D::FilterType filterType = pTexture->GetFilterType();
	if (filterType == Texture2D::FT_NEAREST)
	{
		if (rState.MAGFILTER != D3DTEXF_POINT)
		{
			hr = rDevice->SetSamplerState(unit, D3DSAMP_MAGFILTER,
				D3DTEXF_POINT);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.MAGFILTER = D3DTEXF_POINT;
		}
	}
	else
	{
		if (pData->SupportsMagFAniso && (1.0F < anisotropy &&
			anisotropy <= pRenderer->GetMaxAnisotropy()))
		{
			if (rState.MAGFILTER != D3DTEXF_ANISOTROPIC)
			{
				hr = rDevice->SetSamplerState(unit, D3DSAMP_MAGFILTER,
					D3DTEXF_ANISOTROPIC);
				WIRE_ASSERT(SUCCEEDED(hr));
				rState.MAGFILTER = D3DTEXF_ANISOTROPIC;
			}
		}
		else
		{
			if (rState.MAGFILTER != D3DTEXF_LINEAR)
			{
				hr = rDevice->SetSamplerState(unit, D3DSAMP_MAGFILTER,
					D3DTEXF_LINEAR);
				WIRE_ASSERT(SUCCEEDED(hr));
				rState.MAGFILTER = D3DTEXF_LINEAR;
			}
		}
	}

	// Set the mipmap mode.
	if (pData->SupportsMinFAniso && (1.0F < anisotropy &&
		anisotropy <= pRenderer->GetMaxAnisotropy()))
	{
		if (rState.MINFILTER != D3DTEXF_ANISOTROPIC)
		{
			hr = rDevice->SetSamplerState(unit, D3DSAMP_MINFILTER,
				D3DTEXF_ANISOTROPIC);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.MINFILTER = D3DTEXF_ANISOTROPIC;
		}
	}
	else
	{
		DWORD minFilter = PdrRendererData::StateSampler::TEX_MIN_FILTER[
			filterType];
		if (rState.MINFILTER != minFilter)
		{
			hr = rDevice->SetSamplerState(unit, D3DSAMP_MINFILTER, minFilter);
			WIRE_ASSERT(SUCCEEDED(hr));
			rState.MINFILTER = minFilter;
		}
	}

	DWORD mipFilter = PdrRendererData::StateSampler::TEX_MIP_FILTER[
		filterType];
	if (rState.MIPFILTER != mipFilter)
	{
		hr = rDevice->SetSamplerState(unit, D3DSAMP_MIPFILTER, mipFilter);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.MIPFILTER = mipFilter;
	}

	// Set the border color (for clamp to border).
	const ColorRGBA borderColor(ColorRGBA::BLACK); //pTexture->GetBorderColor();
	DWORD dColor = D3DCOLOR_COLORVALUE(borderColor.R(), borderColor.G(),
		borderColor.B(), borderColor.A());
	if (rState.BORDERCOLOR != dColor)
	{
		hr = rDevice->SetSamplerState(unit, D3DSAMP_BORDERCOLOR, dColor);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.BORDERCOLOR = dColor;
	}

	DWORD uWrap = PdrRendererData::StateSampler::TEX_WRAP_MODE[pTexture->
		GetWrapType(0)];
	if (rState.ADDRESSU != uWrap)
	{
		hr = rDevice->SetSamplerState(unit, D3DSAMP_ADDRESSU, uWrap);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.ADDRESSU = uWrap;
	}

	DWORD vWrap = PdrRendererData::StateSampler::TEX_WRAP_MODE[pTexture->
		GetWrapType(1)];
	if (rState.ADDRESSV != vWrap)
	{
		hr = rDevice->SetSamplerState(unit, D3DSAMP_ADDRESSV, vWrap);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.ADDRESSV = vWrap;
	}

	hr = rDevice->SetTexture(unit, mpBuffer);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void PdrTexture2D::Disable(Renderer* pRenderer, UInt unit)
{
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	HRESULT hr;
	hr = rDevice->SetTexture(unit, 0);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void* PdrTexture2D::Lock(Buffer::LockingMode mode, UInt level)
{
	D3DLOCKED_RECT rect;
	HRESULT hr;
	hr = mpBuffer->LockRect(level, &rect, NULL,
		PdrRendererData::BUFFER_LOCKING[mode]);
	WIRE_ASSERT(SUCCEEDED(hr));
	return rect.pBits;
}

//----------------------------------------------------------------------------
void PdrTexture2D::Unlock(UInt level)
{
	HRESULT hr;
	hr = mpBuffer->UnlockRect(level);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
Bool PdrTexture2D::IsTextureFormatSupported(Renderer* pRenderer, Image2D::
	FormatMode format)
{
	PdrRendererData* pData = pRenderer->GetRendererData();
	D3DFORMAT d3dFormat = PdrRendererData::IMAGE2D_FORMAT[format];
	HRESULT hr;
	hr = pData->D3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		pData->Present.BackBufferFormat, 0, D3DRTYPE_TEXTURE,
		d3dFormat);
	return SUCCEEDED(hr);
}

//----------------------------------------------------------------------------
void PdrTexture2D::CreateRenderTarget(Renderer* pRenderer, const Image2D*
	pImage)
{
	PdrRendererData* pData = pRenderer->GetRendererData();
	IDirect3DDevice9*& rDevice = pData->D3DDevice;
	HRESULT hr;

	const UInt width = pImage->GetBound(0);
	const UInt height = pImage->GetBound(1);
	WIRE_ASSERT(width <= pRenderer->GetMaxTextureWidth());
	WIRE_ASSERT(height <= pRenderer->GetMaxTextureHeight());

	const Image2D::FormatMode format = pImage->GetFormat();
	mBufferSize = width*height*Image2D::GetBytesPerPixel(format);
	if (format == Image2D::FM_D24S8)
	{
		hr = rDevice->CreateTexture(width, height, 1, D3DUSAGE_DEPTHSTENCIL,
			PdrRendererData::IMAGE2D_FORMAT[format], D3DPOOL_DEFAULT,
			&mpBuffer, NULL);
		WIRE_ASSERT(SUCCEEDED(hr));
	}
	else
	{
		UINT levels = 1;
		DWORD usage = D3DUSAGE_RENDERTARGET;
		if (pImage->HasMipmaps())
		{
			levels = 0;
			usage |= D3DUSAGE_AUTOGENMIPMAP;
		}

		WIRE_ASSERT(IsTextureFormatSupported(pRenderer, format));
		hr = rDevice->CreateTexture(width, height, levels, usage,
			PdrRendererData::IMAGE2D_FORMAT[format], D3DPOOL_DEFAULT,
			&mpBuffer, NULL);
		WIRE_ASSERT(SUCCEEDED(hr));
		WIRE_ASSERT(pImage->GetMipmapCount() == mpBuffer->GetLevelCount());

		if (levels == 0)
		{
			mBufferSize = Image2D::GetTotalQuantity(width, height) *
				Image2D::GetBytesPerPixel(format);
		}
	}
}
