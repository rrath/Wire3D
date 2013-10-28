// Sample3 - Level of Detail
// This sample demonstrates how to use discrete LODs in the scene graph.

#include "Sample3.h"

using namespace Wire;

WIRE_APPLICATION(Sample3);

//----------------------------------------------------------------------------
Bool Sample3::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	mspRoot = WIRE_NEW Node;

	// create the torus knot shown on the left side of the screen.
	Node* pLeft = CreateLods();
	mspRoot->AttachChild(pLeft);

	// create the wireframe torus knot shown on the right
	Node* pRight = CreateLods();
	mspRoot->AttachChild(pRight);

	StateWireframe* pWireframe = WIRE_NEW StateWireframe;
	pWireframe->Enabled = true;
	pRight->AttachState(pWireframe);

	StateCull* pCull = WIRE_NEW StateCull;
	pCull->CullFace = StateCull::CM_OFF;
	pRight->AttachState(pCull);

	mspRoot->UpdateRS();

	// Bind all resources of the scene graph (vertex buffers, index buffers,
	// textures) to the renderer. If we don't bind the resources here, they
	// will be bound automatically the first time the object is rendered.
	// When using LODs, this would mean that resources will be created some
	// time during execution of the main loop (i.e. when the respective level
	// of the object is drawn for the first time). Thus we bind beforehand in
	// order to avoid wasting time in the main loop.
	mspRoot->Bind(GetRenderer());

	// setup our camera at the origin
	// looking down the -z axis with y up
	Vector3F cameraLocation(0.0F, 0.0F, 10.0F);
	Vector3F viewDirection(0.0F, 0.0F, -1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	Vector3F right = viewDirection.Cross(up);
	mspCamera = WIRE_NEW Camera;
	mspCamera->SetFrame(cameraLocation, viewDirection, up, right);
	mspCamera->SetFrustum(45, GetWidthF()/GetHeightF() , 0.1F, 300.0F);

	mCuller.SetCamera(mspCamera);

	mspOrthographic = WIRE_NEW Camera(/* isPerspective */ false);
	mspText = StandardMesh::CreateText();

	GetRenderer()->SetClearColor(ColorRGBA::WHITE);

	mAngle = 0.0F;
	mLastTime = System::GetTime();
	return true;
}

//----------------------------------------------------------------------------
void Sample3::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;

	mAngle += static_cast<Float>(elapsedTime);
	mAngle = MathF::FMod(mAngle, MathF::TWO_PI);

	// rotate and zoom the torus knots
	Matrix34F trafo(Vector3F(1, 1, 0), mAngle,
		Vector3F(-2, 0, MathF::Sin(mAngle) * 7));

	Spatial* pLeft = mspRoot->GetChild(0);
	pLeft->Local.SetMatrix(trafo, false);

	Spatial* pRight = mspRoot->GetChild(1);
	trafo.SetColumn(3, Vector3F(2, 0, MathF::Sin(mAngle) * 7));
	pRight->Local.SetMatrix(trafo, false);

 	mspRoot->UpdateGS(time);
 	mCuller.ComputeVisibleSet(mspRoot);

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspCamera);
	GetRenderer()->Draw(mCuller.GetVisibleSets());
	DrawLodTextLabel();
	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
void Sample3::DrawLodTextLabel()
{
	NodeDLod* pLodRoot = DynamicCast<NodeDLod>(mspRoot->GetChild(0));
	WIRE_ASSERT(pLodRoot);	// assumption that lod node is child of root node
	if (!pLodRoot)
	{
		return;
	}

	Int activeLod = pLodRoot->GetActiveChild();
	if (activeLod < 0)
	{
		// no lod is visible
		return;
	}

	Node* pGeo = DynamicCast<Node>(pLodRoot->GetChild(activeLod));
	WIRE_ASSERT(pLodRoot);	// assumption that lod geo is child of lod node

	// set the frustum for the text camera (screenWidth and screenHeight
	// could have been changed by the user resizing the window)
	mspOrthographic->SetFrustum(0, GetWidthF(), 0, GetHeightF(), 0, 1);
	GetRenderer()->SetCamera(mspOrthographic);

	const UInt textArraySize = 100;
	Char text[textArraySize];
	System::Sprintf(text, textArraySize, "LOD %d, %d Triangles",
		activeLod, pGeo->GetRenderObject()->GetMesh()->GetIndexBuffer()->
		GetQuantity() / 3);
	mspText->Set(text, Color32::BLACK);
	mspText->Update(GetRenderer());

	// center text (window resizing can happen any time)
	Float offsetX = (GetWidthF() - 21.0F*8.0F) * 0.5F;
	Float offsetY = GetHeightF() - 6.0F * 8.0F;
	Transformation transformation;
	Vector3F textPosition(offsetX, offsetY, 0);
	transformation.SetTranslate(textPosition);
	GetRenderer()->Draw(mspText, transformation);
}

//----------------------------------------------------------------------------
NodeDLod* Sample3::CreateLods()
{
	// Create different levels of the same object
	Spatial* pLod0 = CreateLod(14, 192);	// 5376 triangles
	Spatial* pLod1 = CreateLod(10, 160);	// 3200 triangles
	Spatial* pLod2 = CreateLod(8, 128);	// 2048 triangles
	Spatial* pLod3 = CreateLod(6, 96);	// 1152 triangles

	NodeDLod* pLod = WIRE_NEW NodeDLod;
	pLod->SetLod(0, pLod0, 0, 5);		// level0 from distance 0-5 units
	pLod->SetLod(1, pLod1, 5, 10);		// level1 from distance 5-10 units
	pLod->SetLod(2, pLod2, 10, 15);		// level2 from distance 10-15 units
	pLod->SetLod(3, pLod3, 15, 100);	// level3 from distance 15-100 units
	// if the object is farther away from the camera than 100 units,
	// the object will not be drawn at all

	return pLod;
}

//----------------------------------------------------------------------------
Spatial* Sample3::CreateLod(UInt shapeCount, UInt segmentCount)
{
	// Create a PQ (4,3) torus knot with a inner radius of 0.2
	RenderObject* pTorus = CreatePqTorusKnot(shapeCount, 0.2F, segmentCount,
		4, 3);

	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(CreateTexture(), Material::BM_REPLACE);
	pTorus->SetMaterial(pMaterial);

	Node* pTorusNode = WIRE_NEW Node(pTorus);
	return pTorusNode;
}

//----------------------------------------------------------------------------
RenderObject* Sample3::CreatePqTorusKnot(UInt shapeCount, Float shapeRadius,
	UInt segmentCount, UInt p, UInt q)
{
	shapeCount++;
	segmentCount++;

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

	// the last vertex can't share uv-coords, so we duplicate the first vertex
	shape.SetElement(shapeCount-1, shape[0]);

	VertexAttributes attributes;
	attributes.SetPositionChannels(3);  // channels: X, Y, Z
	attributes.SetTCoordChannels(2);	// channels: U, V

	const UInt vertexCount = segmentCount * shapeCount;
	VertexBuffer* pVBuffer = WIRE_NEW VertexBuffer(attributes, vertexCount);

	// create the pq torus knot and position & align the shape along it
	angleStride = MathF::TWO_PI / (segmentCount-1);
	angle = 0;
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

		angle += angleStride;		

		// approximate Frenet frame
		Vector3F t = p1-p0;
		Vector3F n = p1+p0;
		Vector3F b = t.Cross(n);
		n = b.Cross(t);
		n.Normalize();
		b.Normalize();

		for (UInt j = 0; j < shapeCount; j++)
		{	
			Vector3F newVertex = p0 + shape[j].X() * n + shape[j].Y() * b;
			pVBuffer->Position3(i*shapeCount + j) = newVertex;

			Vector2F uv((1.0F/(shapeCount-1))*j, (1.0F/(segmentCount-1))*i);
			pVBuffer->TCoord2(i*shapeCount + j) = uv;
		}
	}

	// the last segment can't share uv-coords, so we copy it from the first
	for (UInt j = 0; j < shapeCount; j++)
	{	
		pVBuffer->Position3((segmentCount-1)*shapeCount + j) = pVBuffer->
			Position3(j);
		Vector2F uv = pVBuffer->TCoord2(j);
		uv.Y() += 1.0F;
		pVBuffer->TCoord2((segmentCount-1)*shapeCount + j) = uv;
	}

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

			(*pIBuffer)[index*6+3] = index1;
			(*pIBuffer)[index*6+4] = index2;
			(*pIBuffer)[index*6+5] = index3;
		}
	}

	RenderObject* pTorus = WIRE_NEW RenderObject(pVBuffer, pIBuffer);
	return pTorus;
}

//----------------------------------------------------------------------------
struct Cell
{
	Vector2F point;
	ColorRGB color;
};

//----------------------------------------------------------------------------
Texture2D* Sample3::CreateTexture()
{
	// create the texture once and cache it for subsequent calls
	if (mspTexture)
	{
		return mspTexture;
	}

	// define the properties of the image to be used as a texture
	const UInt width = 32;
	const UInt height = 1024;
	const Image2D::FormatMode format = Image2D::FM_RGB888;
	const UInt bpp = Image2D::GetBytesPerPixel(format);
	ColorRGB* const pColorDst = WIRE_NEW ColorRGB[width*height];

	// create points with random x,y position and color
	TArray<Cell> cells;
	Random random;
	for (UInt i = 0; i < 100; i++)
	{
		Cell cell;
		cell.point.X() = random.GetFloat() * width;
		cell.point.Y() = random.GetFloat() * height;
		cell.color.R() = random.GetFloat();
		cell.color.G() = random.GetFloat();
		cell.color.B() = random.GetFloat();

		Float max = 0.0F;
		max = max < cell.color.R() ? cell.color.R() : max;
		max = max < cell.color.G() ? cell.color.G() : max;
		max = max < cell.color.B() ? cell.color.B() : max;
		max = 1.0F / max;
		cell.color *= max;
		cells.Append(cell);
	}

	// iterate over all texels and use the distance to the 2 closest random
	// points to calculate the texel's color
	Float max = 0;
	for (UInt y = 0; y < height; y++)
	{
		for (UInt x = 0; x < width; x++)
		{
			Float minDist = MathF::MAX_REAL;
			Float min2Dist = MathF::MAX_REAL;
			UInt minIndex = 0;

			for (UInt i = 0; i < cells.GetQuantity(); i++)
			{
				Vector2F pos(static_cast<Float>(x), static_cast<Float>(y));

				// Handle tiling
				Vector2F vec = cells[i].point - pos;
				vec.X() = MathF::FAbs(vec.X());
				vec.Y() = MathF::FAbs(vec.Y());
				vec.X() = vec.X() > width/2 ? width-vec.X() : vec.X();
				vec.Y() = vec.Y() > height/2 ? height-vec.Y() : vec.Y();

				Float distance = vec.Length();

				if (minDist > distance)
				{
					min2Dist = minDist;
					minDist = distance;
					minIndex = i;
				}
				else if (min2Dist > distance)
				{
					min2Dist = distance;
				}
			}

			Float factor = (min2Dist - minDist) + 3;
			ColorRGB color = cells[minIndex].color * factor;
			pColorDst[y*width+x] = color;

			// determine the maximum value of all texels for later
			// normalization of the texture
			max = max < color.R() ? color.R() : max;
			max = max < color.G() ? color.G() : max;
			max = max < color.B() ? color.B() : max;
		}
	}

	// convert and normalize the ColorRGBA float array to an 8-bit per
	// channel texture
	max = 255.0F / max;
	UChar* const pDst = WIRE_NEW UChar[width * height * bpp];
	for (UInt i = 0; i < width*height; i++)
	{
		const ColorRGB& rColor = pColorDst[i];
		pDst[i*bpp] = static_cast<UChar>(rColor.R() * max);
		pDst[i*bpp+1] = static_cast<UChar>(rColor.G() * max);
		pDst[i*bpp+2] = static_cast<UChar>(rColor.B() * max);
	}

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pDst);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	// The texture tiles and the torus is supposed to be seamless, therefore
	// we need the UV set to be repeating.
	pTexture->SetWrapType(0, Texture2D::WT_REPEAT);
	pTexture->SetWrapType(1, Texture2D::WT_REPEAT);

	mspTexture = pTexture;
	return pTexture;
}
