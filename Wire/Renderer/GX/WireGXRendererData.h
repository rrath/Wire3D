// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXRENDERERDATA_H
#define WIREGXRENDERERDATA_H

#include "WireMatrix34.h"
#include "WireGXVertexFormat.h"

namespace Wire
{

class Image2D;
class Mesh;
class PdrDisplayList;
class PdrIndexBuffer;
class RenderText;

class PdrRendererData
{
public:
	PdrRendererData();

	// convenience functions
	static UInt GetTotalImageMemory(const Image2D* pImage, UInt bpp);

	static void ConvertRGBA8888ToTiles(UChar* pSrc, UShort width,
		UShort height, UChar* pDst);
	static void ConvertRGB888ToTiles(UChar* pSrc, UShort width, UShort height,
		UChar* pDst);
	static void ConvertRGB565ToTiles(UChar* pSrc, UShort width, UShort height,
		UChar* pDst);
	static void ConvertRGBA4444ToTiles(UChar* pSrc, UShort width,
		UShort height, UChar* pDst);
	static void GetTileCount(UInt& rTilesYCount, UShort& rHeight,
		UInt& rTilesXCount, UShort &rWidth);

	// GX rendering specific functions
	void Draw(const TArray<PdrVertexFormat::VertexElement>& rElements,
		const UShort* const pIBuffer, UInt indexCount, UInt startIndex);
	void DrawWireframe(const TArray<PdrVertexFormat::VertexElement>&
		rElements, const UShort* const pIBuffer, UInt indexCount,
		UInt startIndex);

	// Platform dependent renderer statistics (e.g. display lists allocated)
	void AppendStatistics(RenderText* pText);
	void ResetStatistics();

	// internally used by System::Assert
	void* GetFramebuffer();
	void SetFramebufferIndex(UInt i);

	// Platform-dependent data
	void* FifoBuffer;
	void* FrameBuffer[2];
	GXRenderModeObj* RMode;

	PdrIndexBuffer* PdrIBuffer;
	PdrVertexFormat* PdrVFormat;

	THashTable<const Mesh*, PdrDisplayList*> DisplayListMap;

	GXColor ClearColor;

	// Transforms world space to camera space.
	Matrix34F View;

	static const UInt MAXLIGHTS = 8;
	UInt FrameBufferIndex;
	Bool IsFrameBufferDirty;
	Bool UseVSync;

	UChar LightsMask;

	static const UInt DEFAULT_FIFO_SIZE = 256*1024;
	static const UChar ALPHA_SRC_BLEND[];
	static const UChar ALPHA_DST_BLEND[];
	static const UChar CULL_TYPE[];
	static const UChar FOG_DENSITY[];
	static const UChar ZBUFFER_COMPARE[];
	static const UChar TEX_MIN_FILTER[];
	static const UChar TEX_WRAP_MODE[];
	static const UChar TEX_BLEND[];
	static const UChar IMAGE2D_FORMAT[];

	struct Statistics
	{
		Statistics() : DisplayListCount(0), DisplayListsSize(0),
			DrawCallsUsingDisplayLists(0) {}
		UInt DisplayListCount;
		UInt DisplayListsSize;
		UInt DrawCallsUsingDisplayLists;
	};

	Statistics Statistics;
};

}

#endif
