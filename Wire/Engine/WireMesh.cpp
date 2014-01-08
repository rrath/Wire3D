// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireMesh.h"

#include "WireBoundingVolume.h"
#include "WireIndexBuffer.h"
#include "WireRenderer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Mesh, Object);

//----------------------------------------------------------------------------
Mesh::Mesh(VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer)
	:
	mVertexBuffers(1, 1)
{
	SetVertexBuffer(pVertexBuffer);
	Init(pIndexBuffer, 0, pIndexBuffer->GetQuantity());
}

//----------------------------------------------------------------------------
Mesh::Mesh(VertexBuffer* pVertexBuffer, IndexBuffer* pIndexBuffer,
	UInt startIndex, UInt indexCount)
	:
	mVertexBuffers(1, 1)
{
	SetVertexBuffer(pVertexBuffer);
	Init(pIndexBuffer, startIndex, indexCount);
}

//----------------------------------------------------------------------------
Mesh::Mesh(VertexBuffers& rVertexBuffers, IndexBuffer* pIndexBuffer)
	:
	mVertexBuffers(rVertexBuffers.GetQuantity(), 1)
{
	SetVertexBuffers(rVertexBuffers);
	Init(pIndexBuffer, 0, pIndexBuffer->GetQuantity());
}

//----------------------------------------------------------------------------
Mesh::Mesh(VertexBuffers& rVertexBuffers, IndexBuffer* pIndexBuffer,
	UInt startIndex, UInt indexCount)
	:
	mVertexBuffers(rVertexBuffers.GetQuantity(), 1)
{
	SetVertexBuffers(rVertexBuffers);
	Init(pIndexBuffer, startIndex, indexCount);
}

//----------------------------------------------------------------------------
Mesh::Mesh(const Mesh* pMesh)
{
	WIRE_ASSERT(pMesh);
	SetVertexBuffers(pMesh->GetVertexBuffers());
	Init(pMesh->mspIndexBuffer, pMesh->GetStartIndex(), pMesh->
		GetIndexCount());
}

//----------------------------------------------------------------------------
Mesh::Mesh(const Mesh* pMesh, UInt startIndex, UInt indexCount)
{
	WIRE_ASSERT(pMesh);
	SetVertexBuffers(pMesh->GetVertexBuffers());
	Init(pMesh->mspIndexBuffer, startIndex, indexCount);
}

//----------------------------------------------------------------------------
Mesh::~Mesh()
{
	// Inform all renderers using this mesh that it is being destroyed.
	// This allows the renderer to free up any associated resources.
	// (i.e. display lists on the Wii)
	Renderer::UnbindAll(this);
}

//----------------------------------------------------------------------------
VertexBuffer* Mesh::GetPositionBuffer()
{
	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i]->GetAttributes().HasPosition())
		{
			return mVertexBuffers[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
const VertexBuffer* Mesh::GetPositionBuffer() const
{
	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i]->GetAttributes().HasPosition())
		{
			return mVertexBuffers[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
VertexBuffer* Mesh::GetNormalBuffer()
{
	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i]->GetAttributes().HasNormal())
		{
			return mVertexBuffers[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
const VertexBuffer* Mesh::GetNormalBuffer() const
{
	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i]->GetAttributes().HasNormal())
		{
			return mVertexBuffers[i];
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
void Mesh::SetStartIndex(UInt startIndex, Bool updateModelBound)
{
	if (mStartIndex != startIndex)
	{
		mStartIndex = startIndex;
		mIsDirty = true;

		if (updateModelBound)
		{
			UpdateModelBound();
		}
	}
}

//----------------------------------------------------------------------------
void Mesh::SetIndexCount(UInt indexCount, Bool updateModelBound)
{
	if (mIndexCount != indexCount)
	{
		mIndexCount = indexCount;
		mIsDirty = true;

		if (updateModelBound)
		{
			UpdateModelBound();
		}
	}
}

//----------------------------------------------------------------------------
void Mesh::UpdateModelBound()
{
	VertexBuffer* pPositions = GetPositionBuffer();
	WIRE_ASSERT(pPositions);

	if ((mStartIndex == 0) && (mIndexCount == mspIndexBuffer->GetQuantity()))
	{
		mspModelBound->ComputeFrom(pPositions);
	}
	else
	{
		mspModelBound->ComputeFrom(pPositions, mspIndexBuffer, mStartIndex,
			mIndexCount);
	}
}

//----------------------------------------------------------------------------
void Mesh::GenerateNormals(Bool ignoreHardEdges)
{
	VertexBuffer* pPositions = GetPositionBuffer();
	if (!pPositions)
	{
		return;
	}

	VertexBuffer* pNormals = GetNormalBuffer();
	if (!pNormals)
	{
		return;
	}

	UShort* const pIndices = mspIndexBuffer->GetData();

	// collect the triangles each vertex is part of
	TArray<TArray<UInt> > buckets(pPositions->GetQuantity());
	buckets.SetQuantity(pPositions->GetQuantity());

	UInt triIndex = 0;
	for (UInt i = 0; i < mspIndexBuffer->GetQuantity(); i += 3)
	{
		buckets[pIndices[i]].Append(triIndex);
		buckets[pIndices[i+1]].Append(triIndex);
		buckets[pIndices[i+2]].Append(triIndex);
		triIndex++;
	}

	if (ignoreHardEdges)
	{
		for (UInt j = 0; j < pPositions->GetQuantity(); j++)
		{
			const Vector3F& vertex = pPositions->Position3(j);
			for (UInt i = j+1; i < pPositions->GetQuantity(); i++)
			{
				if (vertex == pPositions->Position3(i))
				{
					UInt origCount = buckets[j].GetQuantity();
					for (UInt k = 0; k < buckets[i].GetQuantity(); k++)
					{
						buckets[j].Append(buckets[i][k]);
					}

					for (UInt k = 0; k < origCount; k++)
					{
						buckets[i].Append(buckets[j][k]);
					}
				}
			}
		}
	}

	// calculate the normals of the individual triangles
	TArray<Vector3F> faceNormals(mspIndexBuffer->GetQuantity()/3);
	for (UInt i = 0; i < mspIndexBuffer->GetQuantity(); i +=3)
	{
		Vector3F v1 = pPositions->Position3(pIndices[i+1]) - pPositions->
			Position3(pIndices[i]);
		Vector3F v2 = pPositions->Position3(pIndices[i+2]) - pPositions->
			Position3(pIndices[i+1]);

		Vector3F normal = v2.Cross(v1);
		normal.Normalize();
		faceNormals.Append(normal);
	}

	// calculate the normal of the vertex from the normals of its faces
	for (UInt i = 0; i < buckets.GetQuantity(); i++)
	{
		Vector3F normal(Vector3F::ZERO);
		for (UInt j = 0; j < buckets[i].GetQuantity(); j++)
		{
			normal += faceNormals[buckets[i][j]];
		}

		if (buckets[i].GetQuantity() > 0)
		{
			normal /= static_cast<Float>(buckets[i].GetQuantity());
			normal.Normalize();
		}
		else
		{
			// vertex not used in mesh, use a default normal
			normal = Vector3F::UNIT_X;
		}

		pNormals->Normal3(i) = normal;
	}
}

//----------------------------------------------------------------------------
Bool Mesh::HasNormal() const
{
	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i]->GetAttributes().HasNormal())
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
Bool Mesh::HasColor() const
{
	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i]->GetAttributes().HasColor())
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
void Mesh::Init(IndexBuffer* pIndexBuffer, UInt startIndex, UInt indexCount)
{
	WIRE_ASSERT(pIndexBuffer);
	mspIndexBuffer = pIndexBuffer;
	mStartIndex = startIndex;
	mIndexCount = indexCount;
	mIsDirty = false;
	FindMinMaxIndex(startIndex, indexCount);
	mspModelBound = BoundingVolume::Create();
	UpdateModelBound();
}

//----------------------------------------------------------------------------
void Mesh::FindMinMaxIndex(UInt startIndex, UInt indexCount)
{
	mStartVertex = System::MAX_USHORT;
	UShort maxIndex = 0;
	for (UInt i = startIndex; i < startIndex + indexCount; i++)
	{
		UShort index = (*mspIndexBuffer)[i];
		mStartVertex = mStartVertex > index ? index : mStartVertex;
		maxIndex = maxIndex < index ? index : maxIndex;
	}

	mVertexCount = maxIndex - mStartVertex + 1;
}

//----------------------------------------------------------------------------
void Mesh::SetVertexBuffer(VertexBuffer* pVertexBuffer)
{
	WIRE_ASSERT(pVertexBuffer);
	mVertexBuffers.SetQuantity(0, false);
	mVertexBuffers.SetMaxQuantity(1);
	mVertexBuffers.Append(pVertexBuffer);
}

//----------------------------------------------------------------------------
void Mesh::SetVertexBuffers(const VertexBuffers& rVertexBuffers)
{
	WIRE_ASSERT(rVertexBuffers.GetQuantity() > 0);
	WIRE_ASSERT(rVertexBuffers[0]);

	mVertexBuffers.SetQuantity(0, false);
	mVertexBuffers.SetMaxQuantity(rVertexBuffers.GetMaxQuantity());

	for (UInt i = 0; i < rVertexBuffers.GetQuantity(); i++)
	{
		WIRE_ASSERT(rVertexBuffers[i]);
		WIRE_ASSERT(rVertexBuffers[i]->GetQuantity() == rVertexBuffers[0]->
			GetQuantity()); // check if all streams have the same vertex count
		mVertexBuffers.Append(rVertexBuffers[i]);
	}
}
