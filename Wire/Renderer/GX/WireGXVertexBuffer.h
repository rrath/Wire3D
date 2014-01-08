// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXVERTEXBUFFER_H
#define WIREGXVERTEXBUFFER_H

#include "WireBuffer.h"
#include "WireTArray.h"

namespace Wire
{

class Renderer;
class VertexBuffer;

class PdrVertexBuffer
{
public:
	PdrVertexBuffer(Renderer* pRenderer, const VertexBuffer* pVertexBuffer);
	PdrVertexBuffer(Renderer* pRenderer, UInt size, Buffer::UsageType usage);
	~PdrVertexBuffer();

	void Enable(Renderer* pRenderer, UInt vertexSize, UInt streamIndex);
	inline void Disable(Renderer* pRenderer, UInt streamIndex);

	inline void* Lock(Buffer::LockingMode mode);
	inline void Unlock();

	void Update(const VertexBuffer* pVertexBuffer);
	void Update(const VertexBuffer* pVertexBuffer, UInt count,
		UInt offset = 0);

	inline UInt GetBufferSize() const;

private:
	void CreateBuffer(Renderer* pRenderer, UInt size, Buffer::UsageType
		usage);

	void* mpBuffer;
	UInt mBufferSize;
};

#include "WireGXVertexBuffer.inl"

}

#endif
