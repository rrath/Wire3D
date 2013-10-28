// Sample7 - Dynamic Buffers
// This sample demonstrates how to create and use a dynamic vertex buffer.
// On creation of the vertex buffer a usage type is passed indicating how the
// buffer is going to be used. With this information he framework can make
// correct assumptions and use the hardware accordingly.

#include "Sample7.h"

using namespace Wire;

WIRE_APPLICATION(Sample7);

//----------------------------------------------------------------------------
Bool Sample7::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	// create a cube with a dynamic vertex buffer
	mspTorus = CreateTorus();

	// We are using different parts of the same index-/vertexbuffers for 2
	// objects, therefore we duplicate the initial mesh (i.e. share the
	// buffers), to have control about what part of the mesh is being used.
	mspMeshA = mspTorus->GetMesh();
	mspMeshB = WIRE_NEW Mesh(mspMeshA);

	// Bind the geometry to the renderer, i.e. create hardware buffer objects.
	// This would happen automatically when the geometry is rendered for the
	// first time. However we want to dynamically fill the vertex buffer each
	// frame, so the hardware buffer object already has to exist before we
	// are rendering it. Thus we bind it manually.
	GetRenderer()->Bind(mspTorus);

	// setup our camera at the origin
	// looking down the -z axis with y up
	Vector3F cameraLocation(0.0F, 0.0F, 4.5F);
	Vector3F viewDirection(0.0F, 0.0F, -1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	Vector3F right = viewDirection.Cross(up);
	mspCamera = WIRE_NEW Camera;
	mspCamera->SetFrame(cameraLocation, viewDirection, up, right);
	mspCamera->SetFrustum(45, GetWidthF() / GetHeightF() , 0.1F, 300.0F);

	mAngle = 0.0F;
	mLastTime = System::GetTime();
	mIsExpanding = true;
	mSegmentCount = 0;

	mspText = StandardMesh::CreateText();

	return true;
}

//----------------------------------------------------------------------------
void Sample7::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;

	mAngle += static_cast<Float>(elapsedTime*0.5F);
	mAngle = MathF::FMod(mAngle, MathF::TWO_PI);

	mspTorus->SetMesh(mspMeshA);
	Mesh* pMesh = mspMeshA;
	IndexBuffer* pIndexBuffer = pMesh->GetIndexBuffer();

	// animate the positions of the PQ torus knot
	GeneratePositions(pMesh->GetVertexBuffer(), mAngle*2);

	// positions changed, so we need to recalculate the normals
 	GenerateNormals(pMesh->GetVertexBuffer(), pIndexBuffer);

	// update the hardware vertex buffer data
	GetRenderer()->Update(pMesh->GetVertexBuffer());

	// We are not using culling since our transformations do not change
	// and both torus knots are on screen. If we did, we would need to
	// recalculate the model bounds since the positions of the mesh changed.
//	mspTorus->GetMesh()->UpdateModelBound();

	GetRenderer()->GetStatistics()->Reset();
	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspCamera);

	// render centered torus knot
	Matrix3F rotate(Vector3F(1, 1, 0), mAngle);
	Transformation transformation;
	transformation.SetRotate(rotate);
	StateMaterial* pMaterial = StaticCast<StateMaterial>(mspTorus->
		GetStates()[State::MATERIAL]);
	WIRE_ASSERT(pMaterial);
	pMaterial->Ambient = ColorRGBA(0.9F, 1.0F, 0.8F, 1.0F); 
	GetRenderer()->Draw(mspTorus, transformation);

	// render small torus knot
	mspTorus->SetMesh(mspMeshB);
	pMesh = mspMeshB;
	transformation.SetTranslate(Vector3F(0.92F, -0.6F, 2.0F));
	transformation.SetUniformScale(0.18F);
	pMaterial->Ambient = ColorRGBA(0.8F, 1.0F, 0.9F, 1.0F);
	GetRenderer()->SetState(pMaterial);

	// Use the Mesh's StartIndex and ActiveIndexCount to control what
	// part of the mesh is being rendered.
	mSegmentCount += MathD::FMod(elapsedTime * 50, s_SegmentCount);
	UInt segmentCount = static_cast<UInt>(mSegmentCount);
	Bool isExpanding = mIsExpanding;
	if (segmentCount > (s_SegmentCount-1))
	{
		mIsExpanding = !mIsExpanding;
		mSegmentCount = 0;
		segmentCount = s_SegmentCount-1;
	}
	
	if (isExpanding)
	{
		pMesh->SetIndexCount((segmentCount + 1) * s_ShapeCount*6);
		pMesh->SetStartIndex(0);
	}
	else
	{
		pMesh->SetStartIndex(segmentCount * s_ShapeCount*6);
		pMesh->SetIndexCount(pIndexBuffer->GetQuantity() -
			pMesh->GetStartIndex());
	}

	GetRenderer()->Draw(mspTorus, transformation);

	Float fps = static_cast<Float>(1.0 / elapsedTime);
	GetRenderer()->GetStatistics()->Draw(mspText, Transformation::IDENTITY, fps);

	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
RenderObject* Sample7::CreateTorus()
{
	// Create a PQ torus knot, which we will later animate per frame.
	const UInt shapeCount = s_ShapeCount+1;
	const UInt segmentCount = s_SegmentCount+1;

	VertexAttributes attributes;
	attributes.SetPositionChannels(3);  // channels: X, Y, Z
	attributes.SetNormalChannels(3);

	const UInt vertexCount = segmentCount * shapeCount;

	// The usage types for buffers are:
	//   Buffer::UT_STATIC: This is the default. The buffer's content never
	//     (or rarely) changes.
	//   Buffer::UT_DYNAMIC: The buffer's content changes frequently (e.g.
	//     every frame), and it is also being read from.
	//   Buffer::UT_DYNAMIC_WRITE_ONLY: The buffer's content changes
	//     frequently, and is never read from.
	//
	// Specify the usage accordingly, so Wire can use the platform dependent
	// graphics API and hardware optimally.
	//
	// We are going to fill the vertex buffer every frame without having to
	// read from it, so pass Buffer::UT_DYNAMIC_WRITE_ONLY as its usage type.
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes, vertexCount,
		Buffer::UT_DYNAMIC_WRITE_ONLY);

	// here we establish connectivity information defined in an IndexBuffer
	const UInt indexCount = (segmentCount-1)*(shapeCount-1)*6;
	IndexBuffer* pIBuffer = WIRE_NEW IndexBuffer(indexCount);
	for (UInt j = 0; j < segmentCount-1; j++)
	{
		UInt offset = shapeCount*j;
		for (UInt i = 0; i < shapeCount-1; i++)
		{
			UInt index = (shapeCount-1)*j+i;
			UInt index0 = i+offset;
			UInt index1 = index0+shapeCount;
			UInt index2 = index0+1;
			UInt index3 = index0+shapeCount+1;

			(*pIBuffer)[index*6] = index0;
			(*pIBuffer)[index*6+1] = index2;
			(*pIBuffer)[index*6+2] = index1;

			(*pIBuffer)[index*6+3] = index2;
			(*pIBuffer)[index*6+4] = index3;
			(*pIBuffer)[index*6+5] = index1;
		}
	}

	RenderObject* pTorus = WIRE_NEW RenderObject(pVBuffer, pIBuffer);

	// fill the vertex buffer (i.e. initialize positions)
	GeneratePositions(pVBuffer, MathF::PI * 0.5F);

	// prepare calculation of vertex normals (requires initialized positions)
	UShort* pIndices = pIBuffer->GetData();
	mBuckets.SetQuantity(pVBuffer->GetQuantity());

	// collect the triangles each vertex is part of
	UInt triIndex = 0;
	for (UInt i = 0; i < pIBuffer->GetQuantity(); i += 3)
	{
		mBuckets[pIndices[i]].Append(triIndex);
		mBuckets[pIndices[i+1]].Append(triIndex);
		mBuckets[pIndices[i+2]].Append(triIndex);
		triIndex++;
	}

	for (UInt j = 0; j < pVBuffer->GetQuantity(); j++)
	{
		const Vector3F& vertex = pVBuffer->Position3(j);
		for (UInt i = j+1; i < pVBuffer->GetQuantity(); i++)
		{
			if (vertex == pVBuffer->Position3(i))
			{
				UInt origCount = mBuckets[j].GetQuantity();
				for (UInt k = 0; k < mBuckets[i].GetQuantity(); k++)
				{
					mBuckets[j].Append(mBuckets[i][k]);
				}

				for (UInt k = 0; k < origCount; k++)
				{
					mBuckets[i].Append(mBuckets[j][k]);
				}
			}
		}
	}
	
	// material for lighting
	pTorus->GetStates()[State::MATERIAL] = WIRE_NEW StateMaterial;

	Light* pLight = WIRE_NEW Light;
	pTorus->SetLights(WIRE_NEW TArray<LightPtr>);
	pTorus->GetLights()->Append(pLight);

	return pTorus;
}

//----------------------------------------------------------------------------
void Sample7::GeneratePositions(VertexBuffer* pVBuffer, Float radiusAngle)
{
	const Float shapeRadius = 0.2F * (MathF::Sin(radiusAngle) * 0.25F + 1.0F);
	const UInt p = s_P;
	const UInt q = s_Q;
	const UInt shapeCount = s_ShapeCount+1;
	const UInt segmentCount = s_SegmentCount+1;

	// create the inner shape (i.e. a circle)
	TArray<Vector3F> shape(shapeCount);
	Vector3F pos(0, shapeRadius, 0);
	Float angleStride = MathF::TWO_PI / (shapeCount-1);
	Float angle = 0;
	for (UInt i = 0; i < (shapeCount-1); i++)
	{
		Matrix34F rot(Vector3F(0, 0, -1), angle);
		shape.SetElement(i, rot * pos);
		angle += angleStride;
	}

	// duplicate the first vertex for the last one
	shape.SetElement(shapeCount-1, shape[0]);

	angle = 0;

	// create the pq torus knot and position & align the shape along it
	angleStride = MathF::TWO_PI / (segmentCount-1);
	for (UInt i = 0; i < segmentCount-1; i++)
	{
		Float r = 0.5F * (2 + MathF::Sin(q * angle));
		Float x = r * MathF::Cos(p * angle);
		Float y = r * MathF::Cos(q * angle);
		Float z = r * MathF::Sin(p * angle);
		Vector3F p0(x, y, z);

		r = 0.5F * (2 + MathF::Sin(q * (angle+0.1F)));
		x = r * MathF::Cos(p * (angle+0.1F));
		y = r * MathF::Cos(q * (angle+0.1F));
		z = r * MathF::Sin(p * (angle+0.1F));
		Vector3F p1(x, y, z);

		// approximate Frenet frame
		Vector3F t = p1-p0;
		Vector3F n = p1+p0;
		Vector3F b = t.Cross(n);
		n = b.Cross(t);
		n.Normalize();
		b.Normalize();

		angle += angleStride;

		const Float radiusFactor = (MathF::Sin(radiusAngle*16)+2) * 0.5F *
			(MathF::Sin(radiusAngle)*0.5F+1);
		const UInt offset = i*shapeCount;
		for (UInt j = 0; j < shapeCount; j++)
		{
			const Float xs = shape[j].X() * radiusFactor;
			const Float ys = shape[j].Y() * radiusFactor;
			Vector3F newVertex = p0 + xs * n + ys * b;
			pVBuffer->Position3(offset + j) = newVertex;
		}

		radiusAngle += angleStride;
	}

	// copy the last segment from the first
	const UInt offset = (segmentCount-1)*shapeCount;
	for (UInt j = 0; j < shapeCount; j++)
	{	
		pVBuffer->Position3(offset + j) = pVBuffer->Position3(j);
	}
}

//----------------------------------------------------------------------------
void Sample7::GenerateNormals(VertexBuffer* pVBuffer, IndexBuffer* pIBuffer)
{
	UShort* const pIndices = pIBuffer->GetData();

	// calculate the normals of the individual triangles
	TArray<Vector3F> faceNormals(pIBuffer->GetQuantity()/3);
	for (UInt i = 0; i < pIBuffer->GetQuantity(); i +=3)
	{
		Vector3F v1 = pVBuffer->Position3(pIndices[i+1]) -
			pVBuffer->Position3(pIndices[i]);
		Vector3F v2 = pVBuffer->Position3(pIndices[i+2]) -
			pVBuffer->Position3(pIndices[i+1]);

		Vector3F normal = v2.Cross(v1);
		normal.Normalize();
		faceNormals.Append(normal);
	}

	// calculate the normal of the vertex from the normals of its faces
	for (UInt i = 0; i < mBuckets.GetQuantity(); i++)
	{
		Vector3F normal(Vector3F::ZERO);
		for (UInt j = 0; j < mBuckets[i].GetQuantity(); j++)
		{
			normal += faceNormals[mBuckets[i][j]];
		}

		if (mBuckets[i].GetQuantity() > 0)
		{
			normal /= static_cast<Float>(mBuckets[i].GetQuantity());
			normal.Normalize();
		}
		else
		{
			// vertex not used in mesh, use a default normal
			normal = Vector3F::UNIT_X;
		}

		pVBuffer->Normal3(i) = normal;
	}
}
