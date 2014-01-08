// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREBUFFER_H
#define WIREBUFFER_H

#include "WireObject.h"

namespace Wire
{

class Buffer : public Object
{
	WIRE_DECLARE_RTTI;

public:

    // Usage flags for vertex buffers, index buffers and textures.
	enum UsageType
	{
		UT_STATIC,
		UT_DYNAMIC,
		UT_DYNAMIC_WRITE_ONLY,
		UT_STATIC_DISCARD_ON_BIND,
		UT_QUANTITY
	};

	// Locking modes for accessing video memory buffers.
	enum LockingMode
	{
		LM_READ_ONLY,
		LM_WRITE_ONLY,
		LM_READ_WRITE,
		LM_QUANTITY
	};

	inline UsageType GetUsage() const;

protected:
	Buffer(UsageType mUsage);
	virtual ~Buffer();

protected:
	UsageType mUsage;
};

typedef Pointer<Buffer> BufferPtr;

#include "WireBuffer.inl"

}

#endif
