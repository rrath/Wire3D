// Sample10 - Material Sorting & Batching
// This sample demonstrates sorting materials for minimizing state changes
// and correct transparency/opaque geometry order, as well as draw call
// batching of static and dynamic objects.
//
// Note: Batching is disabled on the Wii. It does not suffer from operating
//   system and driver overhead issues like the PC, thus batching does not
//   improve performance.

#include "Sample10.h"

using namespace Wire;

WIRE_APPLICATION(Sample10);

//----------------------------------------------------------------------------
Sample10::Sample10()
	:
	WIREAPPLICATION(
	ColorRGBA(0.0F, 0.0F, 0.0F, 1.0F),	// background color
	"Sample10 - Material Sorting",	// title of the window,
	0, 0,		// window position
	640, 480,	// window size; (use (0,0) for current desktop resolution)
	false,		// fullscreen
	false)		// vsync
{
}

//----------------------------------------------------------------------------
Bool Sample10::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	// Create resources used by scenarios
	mspLight = WIRE_NEW Light;
	mspBound = WIRE_NEW SphereBV;
	CreateCube();
	CreateCylinderFront();
	CreateCylinderBack();

	Vector3F cameraLocation(0.0F, 0.0F, 10.0F);
	Vector3F viewDirection(0.0F, 0.0F, -1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	Vector3F right = viewDirection.Cross(up);
	mspCamera = WIRE_NEW Camera;
	mspCamera->SetFrame(cameraLocation, viewDirection, up, right);
	mspCamera->SetFrustum(45.0F, GetWidthF()/GetHeightF(), 0.1F, 300.0F);

	mCuller.SetCamera(mspCamera);
	mCullerSorting.SetCamera(mspCamera);

	mspTextCamera = WIRE_NEW Camera(/* isPerspective */ false);
	mspText = StandardMesh::CreateText();

	// see header file for description of scenarios
	CreateScenario1();
	CreateScenario2();

	// Create buffers (size in bytes) for batching draw calls.
   	GetRenderer()->CreateBatchingBuffers(20*1024, 50*1024, 1);
	return true;
}

//----------------------------------------------------------------------------
void Sample10::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;

	// disable batching by setting thresholds to 0
	GetRenderer()->SetDynamicBatchingThreshold(0, 0);
	GetRenderer()->SetStaticBatchingThreshold(0);

	// Every 5 seconds we alternate between one of the following 3 states:
	// Culler with no batching (to produce a visible set of objects in the
	//   order of the objects in the scene graph)
	// CullerSorting with no batching (which produces 2 sets of visible
	//   objects: one set of opaque objects sorted by render state, material
	//   and depth (front to back), and one set of transparent objects sorted
	//   likewise (but back to front for correct visibility)).
	// CullerSorting with batching (after sorting the renderer tries to
	//   batch as many objects with the same properties together as possible)

	Bool useScenario1 = MathD::FMod(time, 30) < 15;
	Bool usesSorting = false;
	Culler* pCuller = &mCuller;
	if (MathD::FMod(time, 15) > 5)
	{
		pCuller = &mCullerSorting;
		usesSorting = true;

		if (MathD::FMod(time, 15) > 10)
		{
			// There are 2 non exclusive methods of batching supported:
			// a) Static batching, i.e. batching of index buffers only:
			//    This is used if objects are batchable, share the same
			//    vertexbuffer and the same static transformation.
			//    This is the preferred way, but requires use of submeshes
			//    to share vertexbuffers amongst objects.
			//    (Use the "Combine static meshes" option in the exporter to
			//    prepare your scene objects for this method.)
            //
			// b) Dynamic batching, i.e. batching of vertex and index buffers:
			//    This is used if objects are batchable and do not share the
			//    same vertex buffer. If the objects do not have static
			//    transformation, manual transformation is applied. This is
			//    more CPU intensive, thus the maxVertices parameter should
			//    be kept low, or else more time might be wasted than gained.

			// Set thresholds for batching
			GetRenderer()->SetDynamicBatchingThreshold(200, 100);
			GetRenderer()->SetStaticBatchingThreshold(700);
		}
	}

	Float angle = static_cast<Float>(MathD::FMod(time, MathD::TWO_PI));
	Float sinus = MathF::Sin(angle); 
	Float d = useScenario1 ? 4.0F : 10.0F;
	Vector3F camPos(sinus*d, sinus*d, MathF::Cos(angle)*d);
	mspCamera->LookAt(camPos, Vector3F::ZERO, Vector3F::UNIT_Y);
	mCuller.SetCamera(mspCamera);
	mspLight->Direction = -camPos;

	mspText->SetLineWidth(GetWidthF());
	mspText->Clear();
	mspText->SetPen(0, GetHeightF()-mspText->GetFontHeight()-32.0F);
#ifdef WIRE_DEBUG
	mspText->Append("WARNING: RUNNING DEBUG MODE... (FPS do not represent "
		"performance)\n\n", ColorRGB(1.0F, 1.0F, 0.0));
#endif
	mspText->SetColor(Color32::WHITE);

	TArray<VisibleSet*>* pVisibleSets = &(pCuller->GetVisibleSets());
	TArray<VisibleSet*> sets(1);
	sets.Append(&mVisibleSet);

	if (useScenario1)
	{
		mspText->Append("Scenario 1:\n");
		mspLight->Ambient = ColorRGB::BLACK;
		pCuller->SetFrustum(mspCamera->GetFrustum());
		mVisibleSet.Clear();

		for (UInt i = 0; i < mScenario1Objects.GetQuantity(); i++)
		{
			Transformation* pTransformation = &(mTransformations[i]);
			mScenario1Objects[i]->GetMesh()->GetModelBound()->TransformBy(
				*pTransformation, mspBound);
			if (pCuller->IsVisible(mspBound))
			{
				// generate a sorting key from z distance to camera and
				// the state set id of the render object.
				Float z = pCuller->GetCamera()->GetDVector().Dot(
					pTransformation->GetTranslate());		
				const Float far = pCuller->GetCamera()->GetDMax();
				z += far;
				z /= far*3;
				z = z < 0 ? 0 : z;
				z = z >= 1.0F ? 1.0F - MathF::ZERO_TOLERANCE : z;
				UInt64 zKey = static_cast<UInt64>(z * (1<<24));
				UInt64 key = mScenario1Objects[i]->GetStateSetID();
				key = key << 24 | zKey;

				mVisibleSet.Insert(mScenario1Objects[i], pTransformation, key);
			}
		}

		pVisibleSets = &sets;
		if (usesSorting)
		{
			mVisibleSet.Sort();
		}
	}
	else
	{
		mspText->Append("Scenario 2:\n");
		mspLight->Ambient = ColorRGB(0.6F, 0.6F, 0.6F);
		pCuller->ComputeVisibleSet(mspScenario2);
	}

	GetRenderer()->GetStatistics()->Reset();
	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspCamera);
	GetRenderer()->Draw(*pVisibleSets);
	DrawFPS(elapsedTime, usesSorting);
	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
void Sample10::CreateCube()
{
	mspCube = StandardMesh::CreateCube24(0, 1, true, 0.35F);

	const UInt width = 256;
	const UInt height = 256;
	const Image2D::FormatMode format = Image2D::FM_RGBA8888;
	const UInt bpp = Image2D::GetBytesPerPixel(format);
	NoisePerlin<Float> perlin;
	const Float hf = static_cast<Float>(height) * 0.1F;
	const Float wf = static_cast<Float>(width) * 0.15F;
	UChar* const pDst = WIRE_NEW UChar[width * height * bpp];
	for (UInt y = 0; y < height; y++)
	{
		for (UInt x = 0; x < width; x++)
		{
			Float xf = static_cast<Float>(x);
			Float yf = static_cast<Float>(y);

			Float n = perlin.Noise(xf/wf, yf/hf);
			n = MathF::Cos(xf/wf + n) * 0.4F + 0.4F;
			UChar t = static_cast<UChar>(n * 255.0F);
			pDst[(y*width + x)*bpp] = t;
			pDst[(y*width + x)*bpp+1] = t | 0x1F;
			pDst[(y*width + x)*bpp+2] = t | 0x3F;
			pDst[(y*width + x)*bpp+3] = t;
		}
	}

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pDst);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(pTexture, Material::BM_MODULATE);
	mspCube->SetMaterial(pMaterial);

	TArray<LightPtr>* pLights = WIRE_NEW TArray<LightPtr>;
	pLights->Append(mspLight);
	mspCube->SetLights(pLights);

	State::Init(mspCube->GetStates());

	StateMaterial* pMaterialState = WIRE_NEW StateMaterial;
	pMaterialState->Ambient = ColorRGBA(1, 1, 0.9F, 1);
	mspCube->GetStates()[State::MATERIAL] = pMaterialState;

	// Scenario2 draws this manually, so we have to manually set an ID 
	// used for sorting
	mspCube->SetStateSetID(1);
}

//----------------------------------------------------------------------------
RenderObject* Sample10::CreateCylinder() const
{
	RenderObject* pCylinder = StandardMesh::CreateCylinder(10, 0.25F, 1.0F, 1,
		0, true);
	pCylinder->GetMesh()->GenerateNormals(true);

	const UInt width = 256;
	const UInt height = 256;
	const Image2D::FormatMode format = Image2D::FM_RGBA8888;
	const UInt bpp = Image2D::GetBytesPerPixel(format);
	NoisePerlin<Float> perlin;
	const Float hf = 1.0F / (static_cast<Float>(height) * 0.025F);
	const Float wf = 1.0F / (static_cast<Float>(width) * 0.025F);
	UChar* const pDst = WIRE_NEW UChar[width * height * bpp];
	for (UInt y = 0; y < height; y++)
	{
		for (UInt x = 0; x < width; x++)
		{
			Float xf = static_cast<Float>(x);
			Float yf = static_cast<Float>(y);

			Float n = perlin.Noise(xf*wf, yf*hf)*0.5F + 0.5F;
			UChar t = static_cast<UChar>(n * 255.0F) | 0x3F;
			pDst[(y*width + x)*bpp] = t;
			pDst[(y*width + x)*bpp+1] = t;
			pDst[(y*width + x)*bpp+2] = t;
			pDst[(y*width + x)*bpp+3] = t;
		}
	}

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pDst);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);

	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(pTexture, Material::BM_MODULATE);
	pCylinder->SetMaterial(pMaterial);

	TArray<LightPtr>* pLights = WIRE_NEW TArray<LightPtr>;
	pLights->Append(mspLight);
	pCylinder->SetLights(pLights);

	State::Init(pCylinder->GetStates());

	return pCylinder;
}

//----------------------------------------------------------------------------
void Sample10::CreateCylinderFront()
{
	mspCylinderFront = CreateCylinder();

	StateAlpha* pAlphaState = WIRE_NEW StateAlpha;
	pAlphaState->BlendEnabled = true;
	mspCylinderFront->GetStates()[State::ALPHA] = pAlphaState;

	StateZBuffer* pZBufferState = WIRE_NEW StateZBuffer;
	pZBufferState->Writable = false;
	mspCylinderFront->GetStates()[State::ZBUFFER] = pZBufferState;

	StateMaterial* pMaterialState = WIRE_NEW StateMaterial;
	pMaterialState->Ambient = ColorRGBA(0.9F, 1, 1, 0.5F);
	mspCylinderFront->GetStates()[State::MATERIAL] = pMaterialState;

	mspCylinderFront->SetStateSetID(3);
}

//----------------------------------------------------------------------------
void Sample10::CreateCylinderBack()
{
	mspCylinderBack = CreateCylinder();

	Mesh* pMesh = mspCylinderBack->GetMesh();
	VertexBuffer* pVertexBuffer = pMesh->GetVertexBuffer();
	WIRE_ASSERT(pVertexBuffer->GetAttributes().GetNormalChannels() > 0);
	for (UInt i = 0; i < pVertexBuffer->GetQuantity(); i++)
	{
		pVertexBuffer->Normal3(i) = -pVertexBuffer->Normal3(i);
	}

	StateCull* pCullState = WIRE_NEW StateCull;
	pCullState->CullFace = StateCull::CM_FRONT;
	mspCylinderBack->GetStates()[State::CULL] = pCullState;

	StateMaterial* pMaterialState = WIRE_NEW StateMaterial;
	pMaterialState->Ambient = ColorRGBA(0.5F, 0.6F, 0.6F, 1);
	mspCylinderBack->GetStates()[State::MATERIAL] = pMaterialState;

	mspCylinderBack->SetStateSetID(2);
}

//----------------------------------------------------------------------------
void Sample10::CreateScenario2()
{
	mspScenario2 = WIRE_NEW Node;
	Node* pRoot = mspScenario2;

	const UInt count = 12;
	Bool flip = true;

	Float z = static_cast<Float>(count-1)*-0.5F;
	for (UInt k = 0; k < count; k++)
	{
		Float y = static_cast<Float>(count-1)*-0.5F;
		for (UInt j = 0; j < count; j++)
		{
			Float x = static_cast<Float>(count-1)*-0.5F;
			for (UInt i = 0; i < count; i++)
			{
				Node* pNode = flip ? CreateNodeCylinder() : CreateNodeCube();
				pNode->Local.SetTranslate(Vector3F(x+i, y+j, z+k));
				pNode->Local.SetUniformScale(0.1F);
				pRoot->AttachChild(pNode);
				flip = !flip;
			}
		}
	}

	// Prepare for static batching by applying World transformation to
	// actual vertices. Vertex buffers will be duplicated and merged in the
	// process. Usually this should be done at export time in the content
	// pipeline (e.g. see Unity3D exporter script and 'Game' sample).
	pRoot->UpdateGS();
	pRoot->PrepareForStaticBatching(true);
}

//----------------------------------------------------------------------------
void Sample10::CreateScenario1()
{
	const UInt count = 5;
	mTransformations.SetMaxQuantity(count*count*count);
	mScenario1Objects.SetMaxQuantity(count*count*count*2);
	Bool flip = true;

	Float z = static_cast<Float>(count-1)*-0.5F;
	for (UInt k = 0; k < count; k++)
	{
		Float y = static_cast<Float>(count-1)*-0.5F;
		for (UInt j = 0; j < count; j++)
		{
			Float x = static_cast<Float>(count-1)*-0.5F;
			for (UInt i = 0; i < count; i++)
			{
				Transformation world;
				world.SetTranslate(Vector3F(x+i, y+j, z+k));
				mTransformations.Append(world);				

				if (flip)
				{
					mScenario1Objects.Append(mspCylinderBack);
					mScenario1Objects.Append(mspCylinderFront);
					mTransformations.Append(world);				
				}
				else
				{
					mScenario1Objects.Append(mspCube);
				}

				flip = !flip;
			}
		}
	}
}

//----------------------------------------------------------------------------
Node* Sample10::CreateNodeCylinder()
{
	Node* pRoot = WIRE_NEW Node;

	Node* pBack = WIRE_NEW Node(mspCylinderBack);
	pRoot->AttachChild(pBack);

	Node* pFront = WIRE_NEW Node(mspCylinderFront);
	pRoot->AttachChild(pFront);

	return pRoot;
}

//----------------------------------------------------------------------------
Node* Sample10::CreateNodeCube()
{
	return WIRE_NEW Node(mspCube);
}

//----------------------------------------------------------------------------
void Sample10::DrawFPS(Double elapsed, Bool usesSorting)
{
	// set the frustum for the text camera (screenWidth and screenHeight
	// could have been changed by the user resizing the window)
	mspTextCamera->SetFrustum(0, GetWidthF(), 0, GetHeightF(), 0, 1);
	GetRenderer()->SetCamera(mspTextCamera);

	// set to screen width (might change any time in window mode)
	mspText->SetColor(Color32::WHITE);
	// Text uses OpenGL convention of (0,0) being left bottom of window
	mspText->Append("Sorting: ");

	if (usesSorting)
	{
		mspText->Append("ON", Color32::GREEN);
	}
	else
	{
		mspText->Append("OFF", Color32::RED);
	}

	mspText->Append(", Batching: ", Color32::WHITE);
	if (GetRenderer()->UsesBatching())
	{
		mspText->Append("ON\n\n", Color32::GREEN);
	}
	else
	{
		mspText->Append("OFF\n\n", Color32::RED);
	}

	mspText->SetColor(Color32::WHITE);
	GetRenderer()->DisableLighting();

	Float fps = static_cast<Float>(1.0 / elapsed);
	GetRenderer()->GetStatistics()->Draw(mspText, Transformation::IDENTITY,
		fps, mspTextCamera);
}
