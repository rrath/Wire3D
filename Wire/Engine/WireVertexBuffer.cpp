// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireVertexBuffer.h"

#include "WireRenderer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, VertexBuffer, Buffer);

//----------------------------------------------------------------------------
VertexBuffer::VertexBuffer(const VertexAttributes& rAttributes,
	UInt vertexQuantity, UsageType usage)
	:
	Buffer(usage),
	mAttributes(rAttributes),
	mVertexQuantity(vertexQuantity)
{
	WIRE_ASSERT(vertexQuantity < 65536 /* only 16bit indices on Wii */);
	mpChannel = WIRE_NEW Float[mVertexQuantity * mAttributes.
		GetChannelQuantity()];
}

//----------------------------------------------------------------------------
VertexBuffer::VertexBuffer(const VertexBuffer* pVertexBuffer)
	:
	Buffer(pVertexBuffer->GetUsage())
{
	mAttributes = pVertexBuffer->mAttributes;
	mVertexQuantity = pVertexBuffer->mVertexQuantity;
	UInt floatQuantity = mVertexQuantity * mAttributes.GetChannelQuantity();
	mpChannel = WIRE_NEW Float[floatQuantity];
	size_t size = floatQuantity*sizeof(Float);
	System::Memcpy(mpChannel, size, pVertexBuffer->mpChannel, size);
}

//----------------------------------------------------------------------------
VertexBuffer::VertexBuffer(Float* pData, const VertexAttributes& rAttributes,
	UInt vertexQuantity, UsageType usage)
	:
	Buffer(usage),
	mAttributes(rAttributes),
	mVertexQuantity(vertexQuantity)
{
	WIRE_ASSERT(vertexQuantity < 65536 /* only 16bit indices on Wii */);
	mpChannel = pData;
}

//----------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	// Inform all renderers using this vertex buffer that it is being
	// destroyed. This allows the renderer to free up any associated
	// resources.
	Renderer::UnbindAll(this);

	WIRE_DELETE[] mpChannel;
}

//----------------------------------------------------------------------------
Vector3F& VertexBuffer::Position3(UInt i)
{
	WIRE_ASSERT(mAttributes.GetPositionChannels() == 3);
	Float* pChannel = GetPosition(i);
	return *(reinterpret_cast<Vector3F*>(pChannel));
}

//----------------------------------------------------------------------------
Vector3F VertexBuffer::Position3(UInt i) const
{
	WIRE_ASSERT(mAttributes.GetPositionChannels() == 3);
	const Float* pChannel = GetPosition(i);
	return *(reinterpret_cast<Vector3F*>(const_cast<Float*>(pChannel)));
}

//----------------------------------------------------------------------------
Color32& VertexBuffer::Color3(UInt i, UInt unit)
{
	WIRE_ASSERT(mAttributes.GetColorChannels(unit) == 3);
	Float* pChannel = GetColor(i, unit);
	return *(reinterpret_cast<Color32*>(pChannel));
}

//----------------------------------------------------------------------------
Color32 VertexBuffer::Color3(UInt i, UInt unit) const
{
	WIRE_ASSERT(mAttributes.GetColorChannels(unit) == 3);
	const Float* pChannel = GetColor(i, unit);
	return *(reinterpret_cast<Color32*>(const_cast<Float*>(pChannel)));
}

//----------------------------------------------------------------------------
Color32& VertexBuffer::Color4(UInt i, UInt unit)
{
	WIRE_ASSERT(mAttributes.GetColorChannels(unit) == 4);
	Float* pChannel = GetColor(i, unit);
	return *(reinterpret_cast<Color32*>(pChannel));
}

//----------------------------------------------------------------------------
Color32 VertexBuffer::Color4(UInt i, UInt unit) const
{
	WIRE_ASSERT(mAttributes.GetColorChannels(unit) == 4);
	const Float* pChannel = GetColor(i, unit);
	return *(reinterpret_cast<Color32*>(const_cast<Float*>(pChannel)));
}

//----------------------------------------------------------------------------
Vector3F& VertexBuffer::Normal3(UInt i)
{
	WIRE_ASSERT(mAttributes.GetNormalChannels() == 3);
	Float* pChannel = GetNormal(i);
	return *(reinterpret_cast<Vector3F*>(pChannel));
}

//----------------------------------------------------------------------------
Vector3F VertexBuffer::Normal3(UInt i) const
{
	WIRE_ASSERT(mAttributes.GetNormalChannels() == 3);
	const Float* pChannel = GetNormal(i);
	return *(reinterpret_cast<Vector3F*>(const_cast<Float*>(pChannel)));
}

//----------------------------------------------------------------------------
Vector2F& VertexBuffer::TCoord2(UInt i, UInt unit)
{
	WIRE_ASSERT(mAttributes.GetTCoordChannels(unit) == 2);
	Float* pChannel = GetTCoord(i, unit);
	return *(reinterpret_cast<Vector2F*>(pChannel));
}

//----------------------------------------------------------------------------
Vector2F VertexBuffer::TCoord2(UInt i, UInt unit) const
{
	WIRE_ASSERT(mAttributes.GetTCoordChannels(unit) == 2);
	const Float* pChannel = GetTCoord(i, unit);
	return *(reinterpret_cast<Vector2F*>(const_cast<Float*>(pChannel)));
}

//----------------------------------------------------------------------------
void VertexBuffer::ApplyForward(const Transformation& rTransformation,
	Float* pDst, UInt activeCount, UShort minIndex)
{
	WIRE_ASSERT((mAttributes.GetVertexSize() % sizeof(Float)) == 0);
	Float* pSrc = GetData() + minIndex * mAttributes.GetVertexSize() / sizeof(Float);
	const UInt quantity = activeCount == 0 ? GetQuantity() : activeCount;
	const UInt channelQuantity = mAttributes.GetChannelQuantity();
	Bool needsTrafo = mAttributes.HasPosition() || mAttributes.HasNormal();
	if (rTransformation.IsIdentity() || !needsTrafo)
	{
		if (pSrc != pDst)
		{
			size_t size = quantity * channelQuantity * sizeof(Float);
			System::Memcpy(pDst, size, pSrc, size);
			return;
		}
	}

	const VertexAttributes& rAttr = mAttributes;
	const Matrix34F& transform = rTransformation.GetMatrix();
	const Vector3F& scale = rTransformation.GetScale();

	// positions only vertex buffer
	if (rAttr.GetPositionChannels() == rAttr.GetChannelQuantity())
	{
		WIRE_ASSERT(rAttr.GetPositionChannels() == 3);
		const Float* pPosition = pSrc;
		for (UInt i = 0; i < quantity; i++)
		{
			Vector3F v;
			v.X() = *pPosition++;
			v.Y() = *pPosition++;
			v.Z() = *pPosition++;
			if (rTransformation.IsRSMatrix())
			{
				v.X() *= scale.X();
				v.Y() *= scale.Y();
				v.Z() *= scale.Z();
			}

			v = transform * v;

			*pDst++ = v.X();
			*pDst++ = v.Y();
			*pDst++ = v.Z();
		}

		return;
	}

	// normals only vertex buffer
	if (rAttr.GetNormalChannels() == rAttr.GetChannelQuantity())
	{
		WIRE_ASSERT(rAttr.GetNormalChannels() == 3);
		const Float* pNormal = pSrc;
		for (UInt i = 0; i < quantity; i++)
		{
			Vector3F n;
			n.X() = *pNormal++;
			n.Y() = *pNormal++;
			n.Z() = *pNormal++;

			if (rTransformation.IsRSMatrix())
			{
				n.X() *= scale.X();
				n.Y() *= scale.Y();
				n.Z() *= scale.Z();
			}

			n = transform.Times3(n);

			*pDst++ = n.X();
			*pDst++ = n.Y();
			*pDst++ = n.Z();
		}

		return;
	}

	// interleaved vertex buffer
	for (UInt i = minIndex; i < minIndex+quantity; i++)
	{
		if (rAttr.GetPositionChannels() == 3)
		{
			const Float* const pPosition = GetPosition(i);
			Vector3F v(pPosition[0], pPosition[1], pPosition[2]);

			if (rTransformation.IsRSMatrix())
			{
				v.X() *= scale.X();
				v.Y() *= scale.Y();
				v.Z() *= scale.Z();
			}

			v = transform * v;

			*pDst++ = v.X();
			*pDst++ = v.Y();
			*pDst++ = v.Z();
		}
		else
		{
			/* implement transform for non 3d? */
			WIRE_ASSERT(rAttr.GetPositionChannels() == 0);
		}

		if (rAttr.GetNormalChannels() == 3)
		{
			const Float* const pNormal = GetNormal(i);
			Vector3F n(pNormal[0], pNormal[1], pNormal[2]);

			if (rTransformation.IsRSMatrix())
			{
				n.X() *= scale.X();
				n.Y() *= scale.Y();
				n.Z() *= scale.Z();
			}

			n = transform.Times3(n);

			*pDst++ = n.X();
			*pDst++ = n.Y();
			*pDst++ = n.Z();
		}
		else
		{
			/* implement transform for non 3d? */
			WIRE_ASSERT(rAttr.GetNormalChannels() == 0);
		}

		UInt colorChannelQuantity = rAttr.GetColorChannelQuantity();
		for (UInt unit = 0; unit < colorChannelQuantity; unit++)
		{
			if (rAttr.GetColorChannels(unit) > 0)
			{
				UInt* pColor = reinterpret_cast<UInt*>(GetColor(i, unit));
				*(reinterpret_cast<UInt*>(pDst++)) = *pColor;
			}
		}

		UInt tCoordChannelQuantity = rAttr.GetTCoordChannelQuantity();
		for (UInt unit = 0; unit < tCoordChannelQuantity; unit++)
		{
			if (rAttr.GetTCoordChannels(unit) == 2)
			{
				const Float* pTCoords = GetTCoord(i, unit);
				*pDst++ = *pTCoords++;
				*pDst++ = *pTCoords++;
			}
			else
			{
				/* implement transform for non 2d UVs? */
				WIRE_ASSERT(false);
			}
		}
	}
}

//----------------------------------------------------------------------------
void VertexBuffer::GeneratePlatonicNormals()
{
	const VertexAttributes& rAttr = mAttributes;

	if (rAttr.GetNormalChannels() == rAttr.GetPositionChannels() &&
		rAttr.GetNormalChannels() == 3)
	{
		for (UInt i = 0; i < mVertexQuantity; i++)
		{	
			Normal3(i) = Position3(i);
			Normal3(i).Normalize();
		}
	}
	else
	{
		WIRE_ASSERT(false);
	}
}
