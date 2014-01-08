// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREVERTEXBUFFER_H
#define WIREVERTEXBUFFER_H

#include "WireBuffer.h"
#include "WireColor32.h"
#include "WireTransformation.h"
#include "WireVector2.h"
#include "WireVector3.h"
#include "WireVertexAttributes.h"

namespace Wire
{

class VertexBuffer : public Buffer
{
	WIRE_DECLARE_RTTI;

public:
	VertexBuffer(const VertexAttributes& rAttributes, UInt vertexQuantity,
		UsageType usage = UT_STATIC);
	// Deep copy of pVertexBuffer
	VertexBuffer(const VertexBuffer* pVertexBuffer);
	// takes ownership of pData
	VertexBuffer(Float* pData, const VertexAttributes& rAttributes,
		UInt vertexQuantity, UsageType usage = UT_STATIC);
	virtual ~VertexBuffer();

	inline UInt GetQuantity() const;
	inline Float* GetData();
	inline const Float* GetData() const;
	inline const VertexAttributes& GetAttributes() const;

	Vector3F& Position3(UInt i);
	Vector3F Position3(UInt i) const;

	Color32& Color3(UInt i, UInt unit = 0);
	Color32 Color3(UInt i, UInt unit = 0) const;

	Color32& Color4(UInt i, UInt unit = 0);
	Color32 Color4(UInt i, UInt unit = 0) const;

	Vector3F& Normal3(UInt i);
	Vector3F Normal3(UInt i) const;

	Vector2F& TCoord2(UInt i, UInt unit = 0);
	Vector2F TCoord2(UInt i, UInt unit = 0) const;

	inline Float* GetPosition(UInt i = 0);
	inline const Float* GetPosition(UInt i = 0) const;

 	inline Float* GetColor(UInt i = 0, UInt unit = 0);
 	inline const Float* GetColor(UInt i = 0, UInt unit = 0) const;

	inline Float* GetNormal(UInt i = 0);
	inline const Float* GetNormal(UInt i = 0) const;

	inline Float* GetTCoord(UInt i = 0, UInt unit = 0);
	inline const Float* GetTCoord(UInt i = 0, UInt unit = 0) const;

	// Compute Y = M*X+T
	// User has to take care that pDst is big enough to fit this VertexBuffer
	void ApplyForward(const Transformation& rTransformation, Float* pDst,
		UInt activeCount = 0, UShort minIndex = 0);

	void GeneratePlatonicNormals();

private:
	VertexAttributes mAttributes;

	// The number of vertices in the buffer.
	UInt mVertexQuantity;

	// The vertex buffer data.
	Float* mpChannel;
};

typedef Pointer<VertexBuffer> VertexBufferPtr;

#include "WireVertexBuffer.inl"

}

#endif
