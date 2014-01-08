// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXINDEXBUFFER_H
#define WIREGXINDEXBUFFER_H

#include "WireBuffer.h"
#include "WireGXRendererData.h"
#include "WireRenderer.h"
#include "WireTMap.h"
#include "WireTypes.h"

namespace Wire
{

class Renderer;
class IndexBuffer;
class PdrDisplayList;

class PdrIndexBuffer
{
public:
	PdrIndexBuffer(Renderer* pRenderer, const IndexBuffer* pIndexBuffer);
	PdrIndexBuffer(Renderer* pRenderer, UInt size, Buffer::UsageType usage);
	~PdrIndexBuffer();

	inline void Enable(Renderer* pRenderer);
	inline void Disable(Renderer* pRenderer);

	inline void* Lock(Buffer::LockingMode mode);
	inline void Unlock();

	void Update(const IndexBuffer* pIndexBuffer);
	void Update(const IndexBuffer* pIndexBuffer, UInt count, UInt offset = 0);

	inline UInt GetBufferSize() const;

	inline UShort* GetBuffer();
	inline const UShort* GetBuffer() const;

private:
	void CreateBuffer(Renderer* pRenderer, UInt size, Buffer::UsageType
		usage);

	UShort* mpBuffer;
	UInt mBufferSize;
};

#include "WireGXIndexBuffer.inl"

}

#endif
