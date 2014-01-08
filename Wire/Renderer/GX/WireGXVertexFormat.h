// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXVERTEXFORMAT_H
#define WIREGXVERTEXFORMAT_H

#include "WireRenderer.h"

namespace Wire
{

class VertexBuffer;

class PdrVertexFormat
{
public:
	struct VertexElement
	{
		UInt Offset;
		UChar Attr;
		UChar CompCnt;
		UChar CompType;
		UChar StreamIndex;
	};

	PdrVertexFormat(Renderer* pRenderer, const TArray<Pointer<VertexBuffer> >&
		rVertexBuffers);

	void Enable(Renderer* pRenderer);
	void Disable(Renderer* pRenderer);

	inline const TArray<VertexElement>& GetDeclaration() const;

private:
	void AddAttributes(UInt streamIndex, const VertexAttributes& rAttributes,
		UInt& rColorsCount, UInt& rUvSetCount);

	TArray<VertexElement> mDeclaration;
};

#include "WireGXVertexFormat.inl"

}

#endif
