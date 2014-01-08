// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireStandardMesh.h"

#include "WireBoundingVolume.h"
#include "WireColorRGB.h"
#include "WireColorRGBA.h"
#include "WireImage2D.h"
#include "WireIndexBuffer.h"
#include "WireNode.h"
#include "WireRandom.h"
#include "WireRenderText.h"

using namespace Wire;

Texture2DPtr StandardMesh::s_spFontTexture;

WIRE_IMPLEMENT_TERMINATE(StandardMesh);

//----------------------------------------------------------------------------
void StandardMesh::Terminate()
{
	s_spFontTexture = NULL;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateCube8(const UInt vertexColorChannels,
	const Bool useNormals, const Float extent)
{
	Vector3F vertices[] = {
		Vector3F(-extent, -extent, -extent),
		Vector3F(+extent, -extent, -extent),
		Vector3F(+extent, +extent, -extent),
		Vector3F(-extent, +extent, -extent),
		Vector3F(-extent, -extent, +extent),
		Vector3F(+extent, -extent, +extent),
		Vector3F(+extent, +extent, +extent),
		Vector3F(-extent, +extent, +extent)
	};

	const ColorRGB colors[] = {
		ColorRGB(1.0F, 0.0F, 0.0F), ColorRGB(0.0F, 1.0F, 0.0F),
		ColorRGB(0.0F, 0.0F, 1.0F), ColorRGB(1.0F, 1.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 1.0F), ColorRGB(0.0F, 1.0F, 1.0F),
		ColorRGB(1.0F, 1.0F, 1.0F), ColorRGB(0.0F, 0.0F, 0.0F)
	};

	VertexAttributes attributes;
	attributes.SetPositionChannels(3);	// XYZ

	if (vertexColorChannels > 0)
	{
		WIRE_ASSERT(vertexColorChannels == 3 || vertexColorChannels == 4);
		attributes.SetColorChannels(vertexColorChannels);	// RGB(A)
	}

	if (useNormals)
	{
		attributes.SetNormalChannels(3);	// channels: X, Y, Z
	}

	UInt vertexQuantity = sizeof(vertices) / sizeof(Vector3F);
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes,
		vertexQuantity);

	for (UInt i = 0; i < pVBuffer->GetQuantity(); i++)
	{
		pVBuffer->Position3(i) = vertices[i];
		if (vertexColorChannels == 3)
		{
			pVBuffer->Color3(i) = colors[i];
		}
		else if (vertexColorChannels == 4)
		{
			const ColorRGB& rCol = colors[i];
			ColorRGBA col = ColorRGBA(rCol.R(), rCol.G(), rCol.B(), 1.0F);
			pVBuffer->Color4(i) = col;
		}

		if (useNormals)
		{
			// use platonic normal as default
			Vector3F normal = vertices[i];
			normal.Normalize();
			pVBuffer->Normal3(i) =  normal;
		}
	}

	const UShort indices[] = { 0, 1, 2,	0, 2, 3, 0, 5, 1, 0, 4, 5, 0, 7, 4, 0,
		3, 7, 6, 5, 4, 6, 4, 7, 6, 1, 5, 6, 2, 1, 6, 3, 2, 6, 7, 3 };

	UInt indexQuantity = sizeof(indices) / sizeof(UShort);
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(indexQuantity);
	for (UInt i = 0; i < indexQuantity; i++)
	{
		(*pIBuffer)[i] = indices[i];
	}

	RenderObject* pCube = WIRE_NEW RenderObject(pVBuffer, pIBuffer, NULL);
	return pCube;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateCube14(const UInt vertexColorChannels,
	const UInt uvQuantity, const Bool useNormals, const Float extent)
{
	const Vector3F vertices[] = {
		Vector3F(-extent, -extent, -extent),
		Vector3F(-extent,  extent, -extent),
		Vector3F( extent,  extent, -extent),
		Vector3F( extent, -extent, -extent),
		Vector3F( extent, -extent, extent),
		Vector3F( extent,  extent, extent),
		Vector3F(-extent,  extent, extent),
		Vector3F(-extent, -extent, extent),
		Vector3F( extent,  extent, extent),
		Vector3F(-extent,  extent, extent),
		Vector3F( extent,  extent, extent),
		Vector3F( extent, -extent, extent),
		Vector3F(-extent, -extent, extent),
		Vector3F( extent, -extent, extent)
	};

	const ColorRGB colors[] = {
		ColorRGB(1.0F, 0.0F, 0.0F), ColorRGB(0.0F, 1.0F, 0.0F),
		ColorRGB(0.0F, 0.0F, 1.0F), ColorRGB(1.0F, 1.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 1.0F), ColorRGB(0.0F, 1.0F, 1.0F),
		ColorRGB(1.0F, 1.0F, 1.0F), ColorRGB(0.0F, 0.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 0.0F), ColorRGB(0.0F, 1.0F, 0.0F),
		ColorRGB(0.0F, 0.0F, 1.0F), ColorRGB(1.0F, 1.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 1.0F), ColorRGB(0.0F, 1.0F, 1.0F)
	};

	const Vector2F uvs[] = {
		Vector2F(0.50F, 0.50F), Vector2F(0.50F, 0.25F),
		Vector2F(0.25F, 0.25F), Vector2F(0.25F, 0.50F),
		Vector2F(0.00F, 0.50F), Vector2F(0.00F, 0.25F),
		Vector2F(0.75F, 0.25F), Vector2F(0.75F, 0.50F),
		Vector2F(0.25F, 0.00F), Vector2F(0.50F, 0.00F),
		Vector2F(1.00F, 0.25F), Vector2F(1.00F, 0.50F),
		Vector2F(0.50F, 0.75F), Vector2F(0.25F, 0.75F)
	};

	const UShort indices[] = {
		0, 1, 2, 3,	11, 10, 6, 7, 7, 6, 1, 0, 3, 2, 5, 4, 1, 9, 8, 2, 12, 0,
		3, 13
	};

	VertexAttributes attributes;
	attributes.SetPositionChannels(3);  // channels: X, Y, Z

	for (UInt unit = 0; unit < uvQuantity; unit++)
	{
		attributes.SetTCoordChannels(2, unit);	// channels: U, V
	}

	if (vertexColorChannels > 0)
	{
		WIRE_ASSERT(vertexColorChannels == 3 || vertexColorChannels == 4);
		attributes.SetColorChannels(vertexColorChannels);	// RGB(A)
	}

	if (useNormals)
	{
		attributes.SetNormalChannels(3);	// channels: X, Y, Z
	}

	UInt vertexQuantity = sizeof(vertices) / sizeof(Vector3F);
	WIRE_ASSERT(vertexQuantity == (sizeof(colors) / sizeof(ColorRGB)));	
	WIRE_ASSERT(vertexQuantity == (sizeof(uvs) / sizeof(Vector2F)));
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes,
		vertexQuantity);

	for (UInt i = 0; i < pVBuffer->GetQuantity(); i++)
	{
		pVBuffer->Position3(i) = vertices[i];

		for (UInt unit = 0; unit < uvQuantity; unit++)
		{
			pVBuffer->TCoord2(i, unit) = uvs[i];
		}

		if (vertexColorChannels == 3)
		{
			pVBuffer->Color3(i) = colors[i];
		}
		else if (vertexColorChannels == 4)
		{
			const ColorRGB& rCol = colors[i];
			ColorRGBA col = ColorRGBA(rCol.R(), rCol.G(), rCol.B(), 1.0F);
			pVBuffer->Color4(i) = col;
		}

		if (useNormals)
		{
			// use platonic normal as default
			Vector3F normal = vertices[i];
			normal.Normalize();
			pVBuffer->Normal3(i) =  normal;
		}
	}

	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(6*6);
	for	(UInt i = 0; i < 6; i++)
	{
		(*pIBuffer)[0+i*6] = indices[0+i*4];
		(*pIBuffer)[1+i*6] = indices[3+i*4];
		(*pIBuffer)[2+i*6] = indices[1+i*4];

		(*pIBuffer)[3+i*6] = indices[3+i*4];
		(*pIBuffer)[4+i*6] = indices[2+i*4];
		(*pIBuffer)[5+i*6] = indices[1+i*4];
	}

	RenderObject* pCube = WIRE_NEW RenderObject(pVBuffer, pIBuffer, NULL);
	return pCube;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateCube24(const UInt vertexColorChannels,
	const UInt uvQuantity, const Bool useNormals, const Float extent)
{
	const Vector3F vertices[] =
	{
		Vector3F(-extent,  extent,  extent),
		Vector3F( extent,  extent,  extent),
		Vector3F( extent, -extent,  extent),
		Vector3F(-extent, -extent,  extent),
		Vector3F( extent,  extent, -extent),
		Vector3F( extent,  extent,  extent),
		Vector3F(-extent,  extent,  extent),
		Vector3F(-extent,  extent, -extent),
		Vector3F( extent, -extent,  extent),
		Vector3F( extent,  extent,  extent),
		Vector3F( extent,  extent, -extent),
		Vector3F( extent, -extent, -extent),
		Vector3F(-extent,  extent, -extent),
		Vector3F( extent,  extent, -extent),
		Vector3F( extent, -extent, -extent),
		Vector3F(-extent, -extent, -extent),
		Vector3F( extent, -extent, -extent),
		Vector3F( extent, -extent,  extent),
		Vector3F(-extent, -extent,  extent),
		Vector3F(-extent, -extent, -extent),
		Vector3F(-extent, -extent,  extent),
		Vector3F(-extent,  extent,  extent),
		Vector3F(-extent,  extent, -extent),
		Vector3F(-extent, -extent, -extent)
	};

	const ColorRGB colors[] = {
		ColorRGB(1.0F, 0.0F, 0.0F), ColorRGB(0.0F, 1.0F, 0.0F),
		ColorRGB(0.0F, 0.0F, 1.0F), ColorRGB(1.0F, 1.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 1.0F), ColorRGB(0.0F, 1.0F, 1.0F),
		ColorRGB(1.0F, 1.0F, 1.0F), ColorRGB(0.0F, 0.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 0.0F), ColorRGB(0.0F, 1.0F, 0.0F),
		ColorRGB(0.0F, 0.0F, 1.0F), ColorRGB(1.0F, 1.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 1.0F), ColorRGB(0.0F, 1.0F, 1.0F),
		ColorRGB(1.0F, 1.0F, 1.0F), ColorRGB(0.0F, 0.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 0.0F), ColorRGB(0.0F, 1.0F, 0.0F),
		ColorRGB(0.0F, 0.0F, 1.0F), ColorRGB(1.0F, 1.0F, 0.0F),
		ColorRGB(1.0F, 0.0F, 1.0F), ColorRGB(0.0F, 1.0F, 1.0F),
		ColorRGB(1.0F, 1.0F, 1.0F), ColorRGB(0.0F, 0.0F, 0.0F),
	};

	const Vector2F uvs[] =
	{
		Vector2F(0.0F, 0.0F), Vector2F(1.0F, 0.0F), Vector2F(1.0F, 1.0F),
		Vector2F(0.0F, 1.0F), Vector2F(0.0F, 0.0F),	Vector2F(1.0F, 0.0F),
		Vector2F(1.0F, 1.0F), Vector2F(0.0F, 1.0F), Vector2F(0.0F, 0.0F),
		Vector2F(1.0F, 0.0F), Vector2F(1.0F, 1.0F),	Vector2F(0.0F, 1.0F),
		Vector2F(0.0F, 0.0F), Vector2F(1.0F, 0.0F),	Vector2F(1.0F, 1.0F),
		Vector2F(0.0F, 1.0F), Vector2F(0.0F, 0.0F), Vector2F(1.0F, 0.0F),
		Vector2F(1.0F, 1.0F), Vector2F(0.0F, 1.0F), Vector2F(0.0F, 0.0F),
		Vector2F(1.0F, 0.0F), Vector2F(1.0F, 1.0F), Vector2F(0.0F, 1.0F)
	};

	const UShort indices[] =
	{
		0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 14, 13,
		12, 15, 14, 16, 18, 17,	16, 19, 18, 20, 22, 21,	20, 23, 22
	};

	VertexAttributes attributes;
	attributes.SetPositionChannels(3);  // channels: X, Y, Z

	for (UInt unit = 0; unit < uvQuantity; unit++)
	{
		attributes.SetTCoordChannels(2, unit);	// channels: U, V
	}

	if (useNormals)
	{
		attributes.SetNormalChannels(3);	// channels: X, Y, Z
	}

	if (vertexColorChannels > 0)
	{
		WIRE_ASSERT(vertexColorChannels == 3 || vertexColorChannels == 4);
		attributes.SetColorChannels(vertexColorChannels);	// RGB(A)
	}

	UInt vertexQuantity = sizeof(vertices) / sizeof(Vector3F);
	WIRE_ASSERT(vertexQuantity == (sizeof(uvs) / sizeof(Vector2F)));
	WIRE_ASSERT(vertexQuantity == (sizeof(colors) / sizeof(ColorRGB)));	
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes,
		vertexQuantity);

	for (UInt i = 0; i < pVBuffer->GetQuantity(); i++)
	{
		pVBuffer->Position3(i) = vertices[i];

		for (UInt unit = 0; unit < uvQuantity; unit++)
		{
			pVBuffer->TCoord2(i, unit) = uvs[i];
		}

		if (vertexColorChannels == 3)
		{
			pVBuffer->Color3(i) = colors[i];
		}
		else if (vertexColorChannels == 4)
		{
			const ColorRGB& rCol = colors[i];
			ColorRGBA col = ColorRGBA(rCol.R(), rCol.G(), rCol.B(), 1.0F);
			pVBuffer->Color4(i) = col;
		}

		if (useNormals)
		{
			// use platonic normal as default
			Vector3F normal = vertices[i];
			normal.Normalize();
			pVBuffer->Normal3(i) =  normal;
		}
	}

	UInt indexQuantity = sizeof(indices) / sizeof(UShort);
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(indexQuantity);
	for	(UInt i = 0; i < indexQuantity; i++)
	{
		(*pIBuffer)[i] = indices[i];
	}

	RenderObject* pCube = WIRE_NEW RenderObject(pVBuffer, pIBuffer, NULL);
	return pCube;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreatePlane(const UInt xTileCount,
	const UInt yTileCount, const Float xSizeTotal, const Float ySizeTotal,
	const UInt vertexColorChannels, const UInt uvQuantity,
	const Bool useNormals)
{
	VertexAttributes attributes;
	attributes.SetPositionChannels(3);  // channels: X, Y, Z
	
	for (UInt unit = 0; unit < uvQuantity; unit++)
	{
		attributes.SetTCoordChannels(2, unit);	// channels: U, V
	}

	if (vertexColorChannels > 0)
	{
		WIRE_ASSERT(vertexColorChannels == 3 || vertexColorChannels == 4);
		attributes.SetColorChannels(vertexColorChannels);	// RGB(A)
	}

	if (useNormals)
	{
		attributes.SetNormalChannels(3);	// channels: X, Y, Z
	}

	const UInt vertexCount = (xTileCount+1) * (yTileCount+1);
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes, vertexCount);

	const Float xStride = xSizeTotal / xTileCount;
	const Float yStride = ySizeTotal / yTileCount;
	Float y = -ySizeTotal * 0.5F;
	for (UInt j = 0; j < yTileCount+1; j++)
	{
		Float x = -xSizeTotal * 0.5F;;
		for (UInt i = 0; i < xTileCount+1; i++)
		{
			const UInt idx = i + (xTileCount+1)*j;
			pVBuffer->Position3(idx) = Vector3F(x, y, 0);

			for (UInt unit = 0; unit < uvQuantity; unit++)
			{
				pVBuffer->TCoord2(i + (xTileCount+1)*j, unit) =
					Vector2F(x, y);
			}

			if (vertexColorChannels == 3)
			{
				pVBuffer->Color3(idx) = ColorRGB::WHITE;
			}
			else if (vertexColorChannels == 4)
			{
				pVBuffer->Color4(idx) = ColorRGBA::WHITE;
			}
			
			if (useNormals)
			{
				// use platonic normal as default
				Vector3F normal = pVBuffer->Position3(idx);
				normal.Normalize();
				pVBuffer->Normal3(idx) =  normal;
			}
			
			x += xStride;
		}

		y += yStride;
	}

	WIRE_ASSERT(xTileCount < 65536);
	const UShort xTC = static_cast<UShort>(xTileCount);
	const UInt indexCount = xTileCount * yTileCount * 6;
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(indexCount);

	for (UShort j = 0; j < yTileCount; j++)
	{
		UShort offset = (xTC+1)*j;
		for (UShort i = 0; i < xTileCount; i++)
		{
			UShort index = xTC*j+i;
			UShort index0 = i+offset;
			UShort index1 = index0+xTC+1;
			UShort index2 = index0+1;
			UShort index3 = index0+xTC+2;

			(*pIBuffer)[index*6] = index0;
			(*pIBuffer)[index*6+1] = index1;
			(*pIBuffer)[index*6+2] = index2;

			(*pIBuffer)[index*6+3] = index1;
			(*pIBuffer)[index*6+4] = index3;
			(*pIBuffer)[index*6+5] = index2;

		}
	}

	RenderObject* pPlane = WIRE_NEW RenderObject(pVBuffer, pIBuffer, NULL);
	return pPlane;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateQuad(const UInt vertexColorChannels,
	const UInt uvQuantity, const Bool useNormals, const Float extent)
{
	const Vector3F vertices[] =
	{
		Vector3F(-extent, extent, 0), Vector3F(extent, extent, 0),
		Vector3F(extent, -extent, 0), Vector3F(-extent, -extent, 0)
	};

	const Vector2F uvs[] =
	{
		Vector2F(0, 0), Vector2F(1, 0), Vector2F(1, 1), Vector2F(0, 1)
	};

	const ColorRGB colors[] = {
		ColorRGB(1.0F, 0.0F, 0.0F), ColorRGB(0.0F, 1.0F, 0.0F),
		ColorRGB(0.0F, 0.0F, 1.0F), ColorRGB(1.0F, 1.0F, 0.0F)
	};

	const UShort indices[] = { 0, 1, 2, 0, 2, 3 };

	VertexAttributes attributes;
	attributes.SetPositionChannels(3);  // channels: X, Y, Z

	for (UInt unit = 0; unit < uvQuantity; unit++)
	{
		attributes.SetTCoordChannels(2, unit);	// channels: U, V
	}

	if (vertexColorChannels > 0)
	{
		WIRE_ASSERT(vertexColorChannels == 3 || vertexColorChannels == 4);
		attributes.SetColorChannels(vertexColorChannels);	// RGB(A)
	}

	if (useNormals)
	{
		attributes.SetNormalChannels(3);	// channels: X, Y, Z
	}

	UInt vertexQuantity = sizeof(vertices) / sizeof(Vector3F);
	WIRE_ASSERT(vertexQuantity == (sizeof(uvs) / sizeof(Vector2F)));
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes,
		vertexQuantity);

	for (UInt i = 0; i < pVBuffer->GetQuantity(); i++)
	{
 		pVBuffer->Position3(i) = vertices[i];

		for (UInt unit = 0; unit < uvQuantity; unit++)
		{
			pVBuffer->TCoord2(i) = uvs[i];
		}

		if (vertexColorChannels == 3)
		{
			pVBuffer->Color3(i) = colors[i];
		}
		else if (vertexColorChannels == 4)
		{
			const ColorRGB& rCol = colors[i];
			ColorRGBA col = ColorRGBA(rCol.R(), rCol.G(), rCol.B(), 1.0F);
			pVBuffer->Color4(i) = col;
		}
		
		if (useNormals)
		{
			// use platonic normal as default
			Vector3F normal = vertices[i];
			normal.Normalize();
			pVBuffer->Normal3(i) =  normal;
		}
	}

	UInt indexQuantity = sizeof(indices) / sizeof(UShort);
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(indexQuantity);
	for	(UInt i = 0; i < indexQuantity; i++)
	{
		(*pIBuffer)[i] = indices[i];
	}

	RenderObject* pQuad = WIRE_NEW RenderObject(pVBuffer, pIBuffer, NULL);
	return pQuad;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateCylinder(Int radialSampleCount,
	const Float radius, const Float height, const UInt uvQuantity,
	const UInt vertexColorChannels, const Bool useNormals)
{
	const Int axisSampleCount = 4;
	RenderObject* pCylinder = CreateSphere(axisSampleCount, radialSampleCount,
		radius, uvQuantity, vertexColorChannels, useNormals);

	VertexBuffer* pVBuffer = pCylinder->GetMesh()->GetVertexBuffer();
	Int numVertices = pVBuffer->GetQuantity();

	// Flatten sphere at poles.
	Float hDiv2 = 0.5f*height;
	pVBuffer->Position3(numVertices-2)[2] = -hDiv2;  // south pole
	pVBuffer->Position3(numVertices-1)[2] = +hDiv2;  // north pole

	// Remap z-values to [-h/2,h/2].
	Float zFactor = 2.0f/(axisSampleCount-1);
	Float tmp0 = radius*(-1.0f + zFactor);
	Float tmp1 = 1.0f/(radius*(+1.0f - zFactor));
	for (Int i = 0; i < numVertices-2; ++i)
	{
		Vector3F& rPos = pVBuffer->Position3(i);
		rPos[2] = hDiv2*(-1.0f + tmp1*(rPos[2] - tmp0));
		Float adjust = radius*MathF::InvSqrt(rPos[0]*rPos[0] + rPos[1]*rPos[1]);
		rPos[0] *= adjust;
		rPos[1] *= adjust;
	}

	if (useNormals)
	{
		pCylinder->GetMesh()->GenerateNormals();
	}

	// The duplication of vertices at the seam cause the automatically
	// generated bounding volume to be slightly off center. Reset the bound
	// to use the true information.
	pCylinder->GetMesh()->GetModelBound()->SetCenter(Vector3F::ZERO);
	Float boundingSphereRadius = MathF::Sqrt(radius*radius+height*height)*0.5F;
	pCylinder->GetMesh()->GetModelBound()->SetRadius(boundingSphereRadius);
	return pCylinder;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateCapsule(Int zSampleCount,
	Int radialSampleCount, const Float radius, const Float height,
	const UInt uvQuantity, const UInt vertexColorChannels,
	const Bool useNormals)
{
	// make sure zSampleCount is even, if not, add 1
	zSampleCount = ((zSampleCount+1) >> 1) << 1;
	RenderObject* pCapsule = CreateSphere(zSampleCount, radialSampleCount,
		radius, uvQuantity, vertexColorChannels, useNormals);

	VertexBuffer* pVBuffer = pCapsule->GetMesh()->GetVertexBuffer();
	Float halfHeight = height * 0.5F - radius;
	halfHeight = halfHeight < 0 ? 0 : halfHeight;

	for (UInt i = 0; i < pVBuffer->GetQuantity(); ++i)
	{
		Vector3F& rPos = pVBuffer->Position3(i);
		rPos[2] = rPos[2] > 0 ? rPos[2] + halfHeight : rPos[2] - halfHeight; 
	}

	if (useNormals)
	{
		pCapsule->GetMesh()->GenerateNormals();
	}

	// The duplication of vertices at the seam cause the automatically
	// generated bounding volume to be slightly off center. Reset the bound
	// to use the true information.
	pCapsule->GetMesh()->GetModelBound()->SetCenter(Vector3F::ZERO);
	pCapsule->GetMesh()->GetModelBound()->SetRadius(height * 0.5F);
	return pCapsule;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateSphere(Int zSampleCount,
	Int radialSampleCount, Float radius, const UInt uvQuantity,
	const UInt vertexColorChannels, const Bool useNormals)
{
	VertexAttributes attr;
	attr.SetPositionChannels(3);  // channels: X, Y, Z

	for (UInt unit = 0; unit < uvQuantity; unit++)
	{
		attr.SetTCoordChannels(2, unit);	// channels: U, V
	}

	if (vertexColorChannels > 0)
	{
		WIRE_ASSERT(vertexColorChannels == 3 || vertexColorChannels == 4);
		attr.SetColorChannels(vertexColorChannels);	// RGB(A)
	}

	if (useNormals)
	{
		attr.SetNormalChannels(3);	// channels: X, Y, Z
	}

	const Int vertexQuantity = (zSampleCount-2)*(radialSampleCount+1) + 2;
	const Int triangleQuantity = 2*(zSampleCount-2)*radialSampleCount;
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attr, vertexQuantity);
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(3 * triangleQuantity);

	// generate geometry
	const Float invRS = 1.0F / static_cast<Float>(radialSampleCount);
	const Float zFactor = 2.0F / static_cast<Float>(zSampleCount-1);

	// Generate points on the unit circle to be used in computing the mesh
	// points on a cylinder slice.
	Float* pSin = WIRE_NEW Float[radialSampleCount+1];
	Float* pCos = WIRE_NEW Float[radialSampleCount+1];
	for (Int iR = 0; iR < radialSampleCount; iR++)
	{
		Float angle = MathF::TWO_PI * invRS * iR;
		pCos[iR] = MathF::Cos(angle);
		pSin[iR] = MathF::Sin(angle);
	}

	pSin[radialSampleCount] = pSin[0];
	pCos[radialSampleCount] = pCos[0];

	// generate the sphere itself
	const UInt uvChannelCount = attr.GetTCoordChannelQuantity();
	Int i = 0;
	for (Int iZ = 1; iZ < zSampleCount-1; iZ++)
	{
		const Float zFraction = -1.0F + zFactor * iZ;  // in (-1,1)
		const Float z = radius * zFraction;

		// compute center of slice
		Vector3F sliceCenter(0.0F, 0.0F, z);

		// compute radius of slice
		Float sliceRadius = MathF::Sqrt(MathF::FAbs(radius*radius - z*z));

		// compute slice vertices with duplication at end point
		Vector3F normal;
		Int iSave = i;
		for (Int iR = 0; iR < radialSampleCount; iR++)
		{
			Float radialFraction = iR * invRS;  // in [0,1)
			Vector3F radial(pCos[iR], pSin[iR], 0.0F);
			pVBuffer->Position3(i) = sliceCenter + sliceRadius * radial;
			if (attr.HasNormal())
			{
				normal = pVBuffer->Position3(i);
				normal.Normalize();
				pVBuffer->Normal3(i) = normal;
			}

			if (vertexColorChannels == 3)
			{
				Vector3F v = pVBuffer->Position3(i);
				v.Normalize();
				pVBuffer->Color3(i) = ColorRGB(v.X(), v.Y(), v.Z());
			}
			else if (vertexColorChannels == 4)
			{
				Vector3F v = pVBuffer->Position3(i);
				v.Normalize();
				pVBuffer->Color4(i) = ColorRGBA(v.X(), v.Y(), v.Z(), 1);
			}

			if (uvChannelCount > 0)
			{
				Vector2F tCoord = Vector2F(radialFraction, 0.5F *
					(zFraction+1.0F));
				for (UInt unit = 0; unit < uvChannelCount; unit++)
				{
					if (attr.HasTCoord(unit))
					{
						pVBuffer->TCoord2(i, unit) = tCoord;
					}
				}
			}

			i++;
		}

		pVBuffer->Position3(i) = pVBuffer->Position3(iSave);
		if (attr.HasNormal())
		{
			pVBuffer->Normal3(i) = pVBuffer->Normal3(iSave);
		}

		if (vertexColorChannels == 3)
		{
			Vector3F v = pVBuffer->Position3(i);
			v.Normalize();
			pVBuffer->Color3(i) = ColorRGB(v.X(), v.Y(), v.Z());
		}
		else if (vertexColorChannels == 4)
		{
			Vector3F v = pVBuffer->Position3(i);
			v.Normalize();
			pVBuffer->Color4(i) = ColorRGBA(v.X(), v.Y(), v.Z(), 1);
		}

		if (uvChannelCount > 0)
		{
			Vector2F tCoord = Vector2F(1.0F, 0.5F * (zFraction+1.0F));
			for (UInt unit = 0; unit < uvChannelCount; unit++)
			{
				if (attr.HasTCoord(unit))
				{
					pVBuffer->TCoord2(i, unit) = tCoord;
				}
			}
		}

		i++;
	}

	// south pole
	pVBuffer->Position3(i) = -radius * Vector3F::UNIT_Z;
	if (attr.HasNormal())
	{
		pVBuffer->Normal3(i) = -Vector3F::UNIT_Z;
	}

	if (vertexColorChannels == 3)
	{
		Vector3F v = pVBuffer->Position3(i);
		v.Normalize();
		pVBuffer->Color3(i) = ColorRGB(v.X(), v.Y(), v.Z());
	}
	else if (vertexColorChannels == 4)
	{
		Vector3F v = pVBuffer->Position3(i);
		v.Normalize();
		pVBuffer->Color4(i) = ColorRGBA(v.X(), v.Y(), v.Z(), 1);
	}

	if (uvChannelCount > 0)
	{
		Vector2F tCoord = Vector2F(0.5F, 0);
		for (UInt unit = 0; unit < uvChannelCount; unit++)
		{
			if (attr.HasTCoord(unit))
			{
				pVBuffer->TCoord2(i, unit) = tCoord;
			}
		}
	}

	i++;

	// north pole
	pVBuffer->Position3(i) = radius * Vector3F::UNIT_Z;
	if (attr.HasNormal())
	{
		pVBuffer->Normal3(i) = Vector3F::UNIT_Z;
	}

	if (vertexColorChannels == 3)
	{
		Vector3F v = pVBuffer->Position3(i);
		v.Normalize();
		pVBuffer->Color3(i) = ColorRGB(v.X(), v.Y(), v.Z());
	}
	else if (vertexColorChannels == 4)
	{
		Vector3F v = pVBuffer->Position3(i);
		v.Normalize();
		pVBuffer->Color4(i) = ColorRGBA(v.X(), v.Y(), v.Z(), 1);
	}

	if (uvChannelCount > 0)
	{
		Vector2F tCoord = Vector2F(0.5F, 1.0F);
		for (UInt unit = 0; unit < uvChannelCount; unit++)
		{
			if (attr.HasTCoord(unit))
			{
				pVBuffer->TCoord2(i, unit) = tCoord;
			}
		}
	}

	i++;
	WIRE_ASSERT(i == vertexQuantity);

	WIRE_ASSERT(radialSampleCount < (32768-1) && (radialSampleCount >= 0));
	Short rsc = static_cast<Short>(radialSampleCount);
	WIRE_ASSERT(vertexQuantity < (32768) && (vertexQuantity >= 0));
	Short vq = static_cast<Short>(vertexQuantity);
	WIRE_ASSERT(zSampleCount < (32768) && (zSampleCount >= 0));
	Short zsc = static_cast<Short>(zSampleCount);

	// generate connectivity
	UShort* pLocalIndex = pIBuffer->GetData();
	Short iZStart = 0;
	for (Int iZ = 0; iZ < zSampleCount-3; iZ++)
	{
		Short i0 = iZStart;
		Short i1 = i0 + 1;
		iZStart += rsc+1;
		Short i2 = iZStart;
		Short i3 = i2 + 1;
		for (Int i = 0; i < radialSampleCount; i++)
		{
			pLocalIndex[0] = i0++;
			pLocalIndex[1] = i2;
			pLocalIndex[2] = i1;
			pLocalIndex[3] = i1++;
			pLocalIndex[4] = i2++;
			pLocalIndex[5] = i3++;
			pLocalIndex += 6;
		}
	}

	// south pole triangles
	Short vQm2 = vq-2;
	for (Short i = 0; i < rsc; i++)
	{
		pLocalIndex[0] = i;
		pLocalIndex[1] = i+1;
		pLocalIndex[2] = vQm2;
		pLocalIndex += 3;
	}

	// north pole triangles
	Short vQm1 = vq-1;
	Short offset = (zsc-3) * (rsc+1);
	for (Short i = 0; i < rsc; i++)
	{
		pLocalIndex[0] = i+offset;
		pLocalIndex[1] = vQm1;
		pLocalIndex[2] = i+1+offset;
		pLocalIndex += 3;
	}

	WIRE_ASSERT(pLocalIndex == pIBuffer->GetData() + 3*triangleQuantity);

	WIRE_DELETE[] pCos;
	WIRE_DELETE[] pSin;

	RenderObject* pSphere = WIRE_NEW RenderObject(pVBuffer, pIBuffer, NULL);

	// The duplication of vertices at the seam cause the automatically
	// generated bounding volume to be slightly off center. Reset the bound
	// to use the true information.
	pSphere->GetMesh()->GetModelBound()->SetCenter(Vector3F::ZERO);
	pSphere->GetMesh()->GetModelBound()->SetRadius(radius);
	return pSphere;
}

//----------------------------------------------------------------------------
RenderObject* StandardMesh::CreateIcosahedron(Float radius,
	const UInt vertexColorChannels, const Bool useNormals)
{
	Float t = (1 + MathF::Sqrt(5)) * 0.5F;

	const Vector3F vertices[] = {
		Vector3F(-1, t, 0), Vector3F( 1, t, 0), Vector3F(-1,-t, 0),
		Vector3F( 1,-t, 0), Vector3F( 0,-1, t), Vector3F( 0, 1, t),
		Vector3F( 0,-1,-t), Vector3F( 0, 1,-t), Vector3F( t, 0,-1),
		Vector3F( t, 0, 1), Vector3F(-t, 0,-1), Vector3F(-t, 0, 1) };

	const UShort indices[] = {
		0, 5, 11, 0, 1, 5, 0, 7, 1, 0, 10, 7, 0, 11, 10, 1, 9, 5, 5, 4, 11, 
		11, 2, 10, 10, 6, 7, 7, 8, 1, 3, 4, 9, 3, 2, 4, 3, 6, 2, 3, 8, 6, 
		3, 9, 8, 4, 5, 9, 2, 11, 4, 6, 10, 2, 8, 7, 6, 9, 1, 8 };

	VertexAttributes attr;
	attr.SetPositionChannels(3);  // channels: X, Y, Z

	if (vertexColorChannels > 0)
	{
		WIRE_ASSERT(vertexColorChannels == 3 || vertexColorChannels == 4);
		attr.SetColorChannels(vertexColorChannels);	// RGB(A)
	}

	if (useNormals)
	{
		attr.SetNormalChannels(3);	// channels: X, Y, Z
	}

	Random rnd;
	UInt vertexQuantity = sizeof(vertices) / sizeof(Vector3F);
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attr, vertexQuantity);

	for (UInt i = 0; i < pVBuffer->GetQuantity(); i++)
	{
		Vector3F v = vertices[i];
		v.Normalize();
		pVBuffer->Position3(i) = v * radius;

		if (vertexColorChannels == 3)
		{
			pVBuffer->Color3(i) = ColorRGB(rnd.GetFloat(), rnd.GetFloat(),
				rnd.GetFloat());
		}
		else if (vertexColorChannels == 4)
		{
			pVBuffer->Color4(i) = ColorRGBA(rnd.GetFloat(), rnd.GetFloat(),
				rnd.GetFloat(), 1.0F);
		}

		if (useNormals)
		{
			// use platonic normal as default
			Vector3F normal = vertices[i];
			normal.Normalize();
			pVBuffer->Normal3(i) =  normal;
		}
	}

	UInt indexQuantity = sizeof(indices) / sizeof(UShort);
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(indexQuantity);
	for	(UInt i = 0; i < indexQuantity; i++)
	{
		(*pIBuffer)[i] = indices[i];
	}

	RenderObject* pIcosahedron = WIRE_NEW RenderObject(pVBuffer, pIBuffer, NULL);
	return pIcosahedron;
}

//----------------------------------------------------------------------------
RenderText* StandardMesh::CreateText(UInt maxLength)
{
	if (!s_spFontTexture)
	{
		const UInt texWidth = 128;
		const UInt texHeight = 64;
		UChar* const pRaw = WIRE_NEW UChar[texWidth * texHeight * 4];
		UChar* pDst = pRaw;
		UChar* pSrc = const_cast<UChar*>(s_Font);

		for (UInt j = 0; j < 16*48; j++)
		{
			for (UInt i = 0; i < 8; i++)
			{
				if (*pSrc & (0x80 >> i))
				{
					*pDst++ = 0xFF;
					*pDst++ = 0xFF;
					*pDst++ = 0xFF;
					*pDst++ = 0xFF;
				}
				else
				{
					*pDst++ = 0;
					*pDst++ = 0;
					*pDst++ = 0;
					*pDst++ = 0;
				}
			}

			pSrc++;
		}

		Image2D* pImage = WIRE_NEW Image2D(Image2D::FM_RGBA8888, texWidth,
			texHeight, pRaw, false, Buffer::UT_STATIC, 1);
		s_spFontTexture = WIRE_NEW Texture2D(pImage);
		s_spFontTexture->SetFilterType(Texture2D::FT_NEAREST);
	}

	const UInt totalCharCount = 128;
	TArray<Vector2F> uvs(totalCharCount*4);
	TArray<Vector4F> charSizes(totalCharCount);

	UInt texWidth = s_spFontTexture->GetImage()->GetBound(0);
	UInt texHeight = s_spFontTexture->GetImage()->GetBound(1);

	for (UInt i = 0; i < 32; i++)
	{
		Vector2F pos(120.0F, 40.0F);
		Float u0 = pos.X()/texWidth;
		Float v0 = pos.Y()/texHeight;
		Float u1 = (pos.X()+8.0F)/texWidth;
		Float v1 = (pos.Y()+8.0F)/texHeight;
		uvs.Append(Vector2F(u0, v0));
		uvs.Append(Vector2F(u1, v0));
		uvs.Append(Vector2F(u1, v1));
		uvs.Append(Vector2F(u0, v1));
		charSizes.Append(Vector4F(8, 8, 8, 0));
	}

	Vector2F pos(0, 0);
	for (UInt y = 0; y < 6; y++)
	{
		pos.X() = 0;
		for (UInt x = 0; x < 16; x++)
		{	
			Float u0 = pos.X()/texWidth;
			Float v0 = pos.Y()/texHeight;
			Float u1 = (pos.X()+8.0F)/texWidth;
			Float v1 = (pos.Y()+8.0F)/texHeight;
			uvs.Append(Vector2F(u0, v0));
			uvs.Append(Vector2F(u1, v0));
			uvs.Append(Vector2F(u1, v1));
			uvs.Append(Vector2F(u0, v1));
			charSizes.Append(Vector4F(8, 8, 8, 0));
			pos.X() += 8.0F;
		}

		pos.Y() += 8.0F;
	}

	RenderText* pText = WIRE_NEW RenderText(8, s_spFontTexture, uvs, charSizes,
		maxLength);
	return pText;
}

//----------------------------------------------------------------------------
const UChar StandardMesh::s_Font[] = 
{
	0,16,40,40,16,96,32,16,16,16,16,0,0,0,0,0,0,16,40,40,60,100,80,16,32,8,84,
	16,0,0,0,4,0,16,40,124,80,8,80,16,64,4,56,16,0,0,0,8,0,16,0,40,56,16,32,0,
	64,4,16,124,0,124,0,16,0,16,0,124,20,32,84,0,64,4,56,16,16,0,0,32,0,0,0,
	40,120,76,72,0,32,8,84,16,16,0,0,64,0,16,0,40,16,12,52,0,16,16,16,0,32,0,
	16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,16,56,124,8,124,28,124,56,56,0,0,
	8,0,32,56,68,48,68,4,24,64,32,4,68,68,0,0,16,0,16,68,76,16,4,8,40,120,64,
	8,68,68,16,16,32,124,8,8,84,16,24,24,72,4,120,16,56,60,0,0,64,0,4,16,100,
	16,32,4,124,4,68,32,68,4,16,16,32,124,8,16,68,16,64,68,8,68,68,32,68,8,0,
	16,16,0,16,0,56,56,124,56,8,56,56,32,56,112,0,32,8,0,32,16,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,56,16,120,56,120,124,124,60,68,56,4,68,64,68,68,56,68,
	40,68,68,68,64,64,64,68,16,4,72,64,108,68,68,84,68,68,64,68,64,64,64,68,
	16,4,80,64,84,100,68,92,68,120,64,68,120,120,64,124,16,4,96,64,84,84,68,
	88,124,68,64,68,64,64,76,68,16,4,80,64,68,76,68,64,68,68,68,68,64,64,68,
	68,16,68,72,64,68,68,68,60,68,120,56,120,124,64,60,68,56,56,68,124,68,68,
	56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,120,56,120,56,124,68,68,68,68,68,124,
	124,0,124,0,0,68,68,68,68,16,68,68,68,68,68,4,96,64,12,0,0,68,68,68,64,16,
	68,68,68,40,40,8,96,32,12,16,0,120,68,120,56,16,68,68,84,16,16,16,96,16,
	12,40,0,64,84,80,4,16,68,68,84,40,16,32,96,8,12,68,0,64,72,72,68,16,68,40,
	108,68,16,64,96,4,12,0,0,64,52,68,56,16,56,16,68,68,16,124,124,0,124,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,254,32,0,64,0,4,0,24,0,64,16,8,64,48,0,0,0,
	16,0,64,0,4,0,36,0,64,0,0,64,16,0,0,0,8,56,120,60,60,56,32,56,120,48,24,
	68,16,108,120,56,0,4,68,64,68,68,120,68,68,16,8,72,16,84,68,68,0,60,68,64,
	68,124,32,68,68,16,8,112,16,84,68,68,0,68,68,64,68,64,32,60,68,16,8,72,16,
	84,68,68,0,60,120,60,60,60,32,4,68,56,72,68,56,68,68,56,0,0,0,0,0,0,0,56,
	0,0,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,28,16,112,52,0,0,0,0,0,32,0,0,0,0,
	0,0,48,16,24,88,84,120,60,92,60,120,68,68,68,68,68,124,48,16,24,0,40,68,
	68,96,64,32,68,68,68,40,68,8,96,16,12,0,84,68,68,64,56,32,68,68,84,16,68,
	16,48,16,24,0,40,120,60,64,4,36,76,40,84,40,60,32,48,16,24,0,84,64,4,64,
	120,24,52,16,108,68,4,124,28,16,112,0,0,64,4,0,0,0,0,0,0,0,56,0,0,16,0,0,0
};
