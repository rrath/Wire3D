// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireSphereBV.h"

#include "WireIndexBuffer.h"
#include "WireVertexBuffer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, SphereBV, BoundingVolume);

//----------------------------------------------------------------------------
// The factory function of BoundingVolume creates SphereBV objects. You may
// delete this and allow another bounding volume class to be the default.
//----------------------------------------------------------------------------
BoundingVolume* BoundingVolume::Create()
{
	return WIRE_NEW SphereBV;
}

//----------------------------------------------------------------------------
SphereBV::SphereBV()
	:
	mSphere(Vector3F::ZERO, 0.0F)
{
}

//----------------------------------------------------------------------------
SphereBV::SphereBV(const Sphere3F& rSphere)
	:
	mSphere(rSphere)
{
}

//----------------------------------------------------------------------------
SphereBV::~SphereBV()
{
}

//----------------------------------------------------------------------------
void SphereBV::ComputeFrom(const VertexBuffer* pVertexBuffer)
{
	if (pVertexBuffer)
	{
		UInt quantity = pVertexBuffer->GetQuantity();

		mSphere.Center = Vector3F::ZERO;
		mSphere.Radius = 0.0F;

		for (UInt i = 0; i < quantity; i++)
		{
			mSphere.Center += pVertexBuffer->Position3(i);
		}

		mSphere.Center /= static_cast<Float>(quantity);

		for (UInt i = 0; i < quantity; i++)
		{
			Vector3F diff = pVertexBuffer->Position3(i) - mSphere.Center;
			Float radiusSqr = diff.SquaredLength();
			if (radiusSqr > mSphere.Radius)
			{
				mSphere.Radius = radiusSqr;
			}
		}

		mSphere.Radius = MathF::Sqrt(mSphere.Radius);
	}
}

//----------------------------------------------------------------------------
void SphereBV::ComputeFrom(const VertexBuffer* pVertexBuffer,
	const IndexBuffer* pIndexBuffer, UInt startIndex, UInt indexCount)
{
	WIRE_ASSERT((startIndex + indexCount) <= pIndexBuffer->GetQuantity());

	if (pVertexBuffer && pIndexBuffer && indexCount > 0)
	{
		UInt quantity = pVertexBuffer->GetQuantity();

		TArray<Bool> vertexUsed(quantity);
		vertexUsed.SetQuantity(quantity);
		for (UInt i = 0; i < quantity; i++)
		{
			vertexUsed[i] = false;
		}

		const IndexBuffer& rIndexBuffer = *pIndexBuffer;
		const UInt lastIndex = startIndex+indexCount;
		for (UInt i = startIndex; i < lastIndex; i++)
		{
			UShort j = rIndexBuffer[i];
			vertexUsed[j] = true;
		}


		mSphere.Center = Vector3F::ZERO;
		mSphere.Radius = 0.0F;

		UInt usedQuantity = 0;
		for (UInt i = 0; i < quantity; i++)
		{
			if (vertexUsed[i])
			{
				usedQuantity++;
				mSphere.Center += pVertexBuffer->Position3(i);
			}
		}

		mSphere.Center /= static_cast<Float>(usedQuantity);

		for (UInt i = 0; i < quantity; i++)
		{
			if (vertexUsed[i])
			{
				Vector3F diff = pVertexBuffer->Position3(i) - mSphere.Center;
				Float radiusSqr = diff.SquaredLength();
				if (radiusSqr > mSphere.Radius)
				{
					mSphere.Radius = radiusSqr;
				}
			}
		}

		mSphere.Radius = MathF::Sqrt(mSphere.Radius);
	}
}

//----------------------------------------------------------------------------
void SphereBV::TransformBy(const Transformation& rTransform,
	BoundingVolume* pResult)
{
	WIRE_ASSERT(pResult->IsDerived(SphereBV::TYPE));
	Sphere3F& rTarget = StaticCast<SphereBV>(pResult)->mSphere;
	rTarget.Center = rTransform.ApplyForward(mSphere.Center);
	rTarget.Radius = rTransform.GetNorm() * mSphere.Radius;
}

//----------------------------------------------------------------------------
Int SphereBV::WhichSide(const Plane3F& rPlane) const
{
	Float distance = rPlane.DistanceTo(mSphere.Center);

	if (distance <= -mSphere.Radius)
	{
		return -1;
	}

	if (distance >= mSphere.Radius)
	{
		return +1;
	}

	return 0;
}

//----------------------------------------------------------------------------
void SphereBV::CopyFrom(const BoundingVolume* pInput)
{
	WIRE_ASSERT(pInput->IsDerived(SphereBV::TYPE));
	mSphere = StaticCast<SphereBV>(pInput)->mSphere;
}

//----------------------------------------------------------------------------
void SphereBV::GrowToContain(const BoundingVolume* pInput)
{
	mSphere = Sphere3F::MergeSpheres(mSphere, StaticCast<SphereBV>(pInput)->
		mSphere);
}
