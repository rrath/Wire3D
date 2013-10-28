// Sample5 - Lighting
// This sample demonstrates how to render objects using lights.
// For that purpose we create 2 cubes which are both lit by 2 point lights
// using the scene graph. Furthermore we create a plane which is lit by a 
// spot light and rendered without using the scene graph.
// The positions of the lights are represent by small cubes which are colored
// using the light's color.

#include "Sample5.h"

using namespace Wire;

WIRE_APPLICATION(Sample5);

//----------------------------------------------------------------------------
Bool Sample5::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	// We are building the following scenegraph:
	// +Root
	// +->LitGroup (with StateMaterial, Light1, Light2 attached)
	// | +->Cube1 (textured cube)
	// | +->Cube2 (flat shaded cube)
	// |
	// +->LightNode1 (owning Light1)
	// | +->LightSrc1 (cube representing Light1)
	// |
	// +->LightNode2 (owning Light2)
	//   +->LightSrc2 (cube representing Light2)

	mspRoot = WIRE_NEW Node;
	Node* pLitGroup = WIRE_NEW Node;
	mspRoot->AttachChild(pLitGroup);

	// create a green and a red point light and a material state
	// which defines how to reflect the lights.
	Light* pLight1 = WIRE_NEW Light(Light::LT_POINT);
	pLight1->Color = ColorRGB::GREEN;

	Light* pLight2 = WIRE_NEW Light(Light::LT_POINT);
	pLight2->Color = ColorRGB::RED;
	pLight2->Ambient = ColorRGB(0.1F, 0.1F, 0.1F);

	StateMaterial* pMaterial = WIRE_NEW StateMaterial;
	pMaterial->Ambient = ColorRGBA(1, 1, 0, 1);

	// attach lights and material state
	pLitGroup->AttachState(pMaterial);
	pLitGroup->AttachLight(pLight1);
	pLitGroup->AttachLight(pLight2);

	// create and position the actual objects to be lit
	Spatial* pCube1 = CreateCube();
	Spatial* pCube2 = CreateCube(false, true);
	pCube1->Local.SetTranslate(Vector3F(-2.0F, 0, 0));
	pCube2->Local.SetTranslate(Vector3F(2.5F, 0, 0));
	pLitGroup->AttachChild(pCube1);
	pLitGroup->AttachChild(pCube2);

	// create light nodes. These act the same as normal nodes, except that
	// they own a light and apply scene graph transformations on that light.
	NodeLight* pLightNode1 = WIRE_NEW NodeLight;
	mspRoot->AttachChild(pLightNode1);
	pLightNode1->Set(pLight1);

	NodeLight* pLightNode2 = WIRE_NEW NodeLight;
	mspRoot->AttachChild(pLightNode2);
	pLightNode2->Set(pLight2);

	// create two cubes representing the position and color of the lights
	Spatial* pLightSrc1 = CreateCube(false, false, true, ColorRGBA::GREEN);
	pLightSrc1->Local.SetUniformScale(0.15F);
	pLightNode1->AttachChild(pLightSrc1);

	Spatial* pLightSrc2 = CreateCube(false, false, true, ColorRGBA::RED);
	pLightSrc2->Local.SetUniformScale(0.15F);
	pLightNode2->AttachChild(pLightSrc2);

	// update all render state and light objects in the scene graph
	// (i.e. Light1, Light2 and StateMaterial are propagated from LitGroup
	// to Cube1 and Cube2)
	mspRoot->UpdateRS();

	// create the bottom plane, which already has a white light attached,
	// to be rendered manually in the render loop. This is only for the
	// purpose of demonstrating scene graph and manual rendering with lights.
	mspPlane = CreatePlane();
	NodePtr spWhiteCubeNode = CreateCube(false, false, true,
		ColorRGBA::WHITE);
	mspWhiteCube = spWhiteCubeNode->GetRenderObject();

	// Setup the position and orientation of the camera to look down
	// the -z axis with y up.
	Vector3F cameraLocation(0.0F, -1.0F, 10.0F);
	Vector3F viewDirection(0.0F, 0.0F, -1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	Vector3F right = viewDirection.Cross(up);
	mspCamera = WIRE_NEW Camera;
	mspCamera->SetFrame(cameraLocation, viewDirection, up, right);

	// By providing a field of view (FOV) in degrees, aspect ratio,
	// near and far plane, we define a viewing frustum used for culling.
	mspCamera->SetFrustum(45, GetWidthF() / GetHeightF() , 0.1F, 300.0F);
	mCuller.SetCamera(mspCamera);

	// Initialize working variables used in the render loop (i.e. OnIdle()).
	mAngle = 0.0F;
	mLastTime = System::GetTime();

	return true;
}

//----------------------------------------------------------------------------
void Sample5::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;
	mAngle += static_cast<Float>(elapsedTime);
	mAngle = MathF::FMod(mAngle, MathF::TWO_PI * 2);

	// scene graph transformations
	//
	Node* pLitGroup = DynamicCast<Node>(mspRoot->GetChild(0));
	WIRE_ASSERT(pLitGroup);

	// rotate the 2 cubes
	Matrix3F rotate1(Vector3F(0.75F, 0.25F, 0.5F), -mAngle * 0.5F);
	Spatial* pCube1 = pLitGroup->GetChild(0);
	pCube1->Local.SetRotate(rotate1);

	Matrix3F rotate2(Vector3F(-0.75F, -0.25F, -0.5F), -mAngle * 0.5F);
	Spatial* pCube2 = pLitGroup->GetChild(1);
	pCube2->Local.SetRotate(rotate2);

	// move the green light up and down
	Float y = MathF::FMod(static_cast<Float>(time), MathF::TWO_PI);
	Vector3F lightPos1(0, MathF::Sin(y*2) * 1.5F, 2);
	Node* pLightNode1 = DynamicCast<Node>(mspRoot->GetChild(1));
	WIRE_ASSERT(pLightNode1);
	pLightNode1->Local.SetTranslate(lightPos1);

	// rotate the red light about the y axis
	Node* pLightNode2 = DynamicCast<Node>(mspRoot->GetChild(2));
	WIRE_ASSERT(pLightNode2);
	Matrix34F rotateLight2(Vector3F::UNIT_Y, -mAngle);
	Vector3F lightPos2 = rotateLight2 * Vector3F(5, 0, 0);
	pLightNode2->Local.SetTranslate(lightPos2);

	mspRoot->UpdateGS(time);
	mCuller.ComputeVisibleSet(mspRoot);

	// manual transformation from local to world space of 
	// the non-scene graph part
	Transformation transformation;
	Float angle = MathF::Sin(mAngle*2);
	angle = angle * MathF::HALF_PI*0.3F + MathF::PI;
	Matrix34F rotateLocalLight3(Vector3F(0, 1, 0), angle);
	Matrix34F rotateWorldLight3(Vector3F(1, 0, 0), -0.5F);
	transformation.SetTranslate(Vector3F(0.5F, -1.0F, 4+MathF::Sin(y*1.0F)*2));
	transformation.SetRotate(rotateWorldLight3 * rotateLocalLight3);
	transformation.SetUniformScale(0.15F);
	mspSpotLight->Position = transformation.GetTranslate();
	mspSpotLight->Direction = transformation.GetMatrix().GetColumn(2);

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspCamera);

	// render the scene graph
	GetRenderer()->Draw(mCuller.GetVisibleSets());

	// before we start drawing objects in 'manual' mode, release all resources
	// cached by the Renderer to return the renderer to its default state.
	// This is necessary when identical resources (mspTexture in this case)
	// are used by the scene graph and other objects that are being draw
	// manually.
	GetRenderer()->ReleaseResources();

	// render the white cube representing the spot light
	GetRenderer()->Draw(mspWhiteCube, transformation);

	Matrix34F matrix(Vector3F(1.0F, 0, 0), -1.0F, Vector3F(0, -2.5F, 0));
	transformation.SetMatrix(matrix, false);
	transformation.SetUniformScale(1);

	// render the bottom plane which is being lit by the spot light
	GetRenderer()->SetLight(mspSpotLight);
	GetRenderer()->EnableLighting(mspSpotLight->Ambient);
	GetRenderer()->Draw(mspPlane, transformation);
	GetRenderer()->DisableLighting();

	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
Node* Sample5::CreateCube(Bool useTexture, Bool useNormals,
	Bool useVertexColor, ColorRGBA vertexColor)
{
	RenderObject* pCube = StandardMesh::CreateCube24(useVertexColor ? 4 : 0,
		useTexture ? 1 : 0, useNormals);
	VertexBuffer* const pVBuffer = pCube->GetMesh()->GetVertexBuffer();

	for (UInt i = 0; i < pVBuffer->GetQuantity(); i++)
	{
		if (useVertexColor)
		{
			if (pVBuffer->Position3(i).Z() > 0)
			{
				pVBuffer->Color4(i) = vertexColor;
			}
			else
			{
				pVBuffer->Color4(i) = vertexColor * 0.5F;
			}
		}
	}

	pCube->GetMesh()->GenerateNormals();

	if (useTexture)
	{
		Material* pMaterial = WIRE_NEW Material;
		pMaterial->AddTexture(CreateTexture(), Material::BM_MODULATE);
		pCube->SetMaterial(pMaterial);
	}

	Node* pCubeNode = WIRE_NEW Node(pCube);
	return pCubeNode;
}

//----------------------------------------------------------------------------
RenderObject* Sample5::CreatePlane()
{
	const UInt tileXCount = 30;
	const UInt tileYCount = 20;
	const Float xSizeTotal = 12.0F;
	const Float ySizeTotal = 8.0F;
	RenderObject* pPlane = StandardMesh::CreatePlane(tileXCount, tileYCount,
		xSizeTotal, ySizeTotal, 0, 1, true);
	pPlane->GetMesh()->GenerateNormals();

	Texture2D* pTexture = CreateTexture();
	pTexture->SetWrapType(0, Texture2D::WT_REPEAT);
	pTexture->SetWrapType(1, Texture2D::WT_REPEAT);
	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(pTexture, Material::BM_MODULATE);
	pPlane->SetMaterial(pMaterial);

	// attach a material state and a light to the plane geometry directly
	StateMaterial* pStateMaterial = WIRE_NEW StateMaterial;
	pStateMaterial->Ambient = ColorRGBA(1, 1, 1, 1);
	pPlane->GetStates()[State::MATERIAL] = pStateMaterial;

	mspSpotLight = WIRE_NEW Light(Light::LT_SPOT);
	mspSpotLight->Position = Vector3F(0, 0, 10);
	mspSpotLight->Direction = Vector3F(0, 0, -1);
	mspSpotLight->Angle = 0.5F;
	mspSpotLight->Ambient = ColorRGB(0.2F, 0.2F, 0.2F);
	return pPlane;
}

//----------------------------------------------------------------------------
struct Cell
{
	Vector2F point;
	ColorRGB color;
};

//----------------------------------------------------------------------------
Texture2D* Sample5::CreateTexture()
{
	if (mspTexture)
	{
		return mspTexture;
	}

	// define the properties of the image to be used as a texture
	const UInt width = 256;
	const UInt height = 256;
	const Image2D::FormatMode format = Image2D::FM_RGB888;
	const UInt bpp = Image2D::GetBytesPerPixel(format);
	ColorRGB* const pColorDst = WIRE_NEW ColorRGB[width*height];

	// create points with random x,y position and color
	TArray<Cell> cells;
	Random random;
	for (UInt i = 0; i < 10; i++)
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
		ColorRGB color = pColorDst[i];
		pDst[i*bpp] = static_cast<UChar>(color.R() * max);
		pDst[i*bpp+1] = static_cast<UChar>(color.G() * max);
		pDst[i*bpp+2] = static_cast<UChar>(color.B() * max);
	}

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pDst);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	// The texture tiles are supposed to be seamless, therefore
	// we need the UV set to be repeating.
	pTexture->SetWrapType(0, Texture2D::WT_REPEAT);
	pTexture->SetWrapType(1, Texture2D::WT_REPEAT);

	// save the texture for later reference
	mspTexture = pTexture;

	return pTexture;
}
