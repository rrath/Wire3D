// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREINDEXBUFFER_H
#define WIREINDEXBUFFER_H

#include "WireBuffer.h"

namespace Wire
{

class IndexBuffer : public Buffer
{
	WIRE_DECLARE_RTTI;

public:
	IndexBuffer(UInt quantity, UsageType usage = UT_STATIC);
	IndexBuffer(UShort* pIndices, UInt quantity, UsageType usage = UT_STATIC);
	IndexBuffer(const IndexBuffer* pIndexBuffer);
	virtual ~IndexBuffer();

	// Access to indices.
	inline UShort operator[] (UInt i) const;
	inline UShort& operator[] (UInt i);

	// Direct access to the index buffer data. The quantity is the number of
	// UInt elements. The number of bytes for the entire index buffer is
	// GetQuantity()*sizeof(UShort).
	inline UInt GetQuantity() const;
	inline UShort* GetData();
	inline const UShort* GetData() const;

	void Copy(UShort* pDst, UShort offset, UInt activeIndexCount,
		UInt startIndex);

private:
	UInt mQuantity;
	UShort* mpIndices;
};

typedef Pointer<IndexBuffer> IndexBufferPtr;

#include "WireIndexBuffer.inl"

}

#endif
