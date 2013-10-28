// Sample6 - Custom Nodes
// This sample demonstrates how to create your own nodes to be used in the
// scene graph. This way you can extend the functionality of the system and
// suit it your own needs. See LensflareNode(.cpp/.h) for details.

#include "Sample6.h"
#include "LensflareNode.h"

using namespace Wire;

WIRE_APPLICATION(Sample6);

//----------------------------------------------------------------------------
Bool Sample6::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	// create a simple scene consisting of a sun, terrain and a lens flare
	mspRoot = WIRE_NEW Node;
	
	Node* pSun = CreateSun();
	mspRoot->AttachChild(pSun);

	Node* pTerrain = CreateTerrain();
	mspRoot->AttachChild(pTerrain);

	LensflareNode* pLensflare = WIRE_NEW LensflareNode(pSun);
	mspRoot->AttachChild(pLensflare);

	// initialize the render state of the scene graph
	mspRoot->UpdateRS();

	mspRoot->Bind(GetRenderer());

	// setup the camera
	Vector3F cameraLocation(0.0F, 0.0F, 0.0F);
	Vector3F viewDirection(0.0F, 0.0F, -1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	Vector3F right = viewDirection.Cross(up);
	mspCamera = WIRE_NEW Camera;
	mspCamera->SetFrame(cameraLocation, viewDirection, up, right);
	mspCamera->SetFrustum(45, GetWidthF() / GetHeightF(), 0.1F, 300.0F);

	mCuller.SetCamera(mspCamera);

	// initialize working variables used in the render loop (i.e. OnIdle())
	mAngle = 0.0F;
	mLastTime = System::GetTime();

	GetRenderer()->SetClearColor(ColorRGBA(0.62F, 0.7F, 1.0F, 1.0F));

	return true;
}

//----------------------------------------------------------------------------
void Sample6::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;
	mAngle += static_cast<Float>(elapsedTime);
	mAngle = MathF::FMod(mAngle, MathF::TWO_PI);

	// tilt the camera left/right & up/down to emphasize the lensflare effect
	Float angleY = MathF::Sin(mAngle*2) * 0.4F + 75 * MathF::DEG_TO_RAD;
	Matrix34F rotY(Vector3F(0, 1, 0), angleY);
	Float angleX = MathF::Cos(mAngle) * 0.2F - 5 * MathF::DEG_TO_RAD;
	Matrix34F rotX(Vector3F(0, 0, 1), angleX);
	Matrix34F rot = rotY * rotX;
	mspCamera->SetAxes(rot.GetColumn(0), rot.GetColumn(1), rot.GetColumn(2));

	// Calculate local to world transformation
	mspRoot->UpdateGS(time);

	Vector3F lightPos = mspRoot->GetChild(0)->World.GetTranslate();
	mspRoot->GetChild(1)->GetLight()->Position = lightPos;

	mCuller.ComputeVisibleSet(mspRoot);

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspCamera);
	GetRenderer()->Draw(mCuller.GetVisibleSets());
	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
Node* Sample6::CreateSun()
{
	// create a sun using a bright yellow colored sphere
	RenderObject* pSun = StandardMesh::CreateSphere(4, 16, 3, 0, 3);
	VertexBuffer* pSunVB = pSun->GetMesh()->GetVertexBuffer();
	for (UInt i = 0; i < pSunVB->GetQuantity(); i++)
	{
		pSunVB->Color3(i) = ColorRGB(1.0F, 1.0F, 0.8F);
	}

	Node* pSunNode = WIRE_NEW Node(pSun);
	pSunNode->Local.SetTranslate(Vector3F(20, 20, -100));
	return pSunNode;
}

//----------------------------------------------------------------------------
Node* Sample6::CreateTerrain()
{
	// create a simple terrain using a plane with sinus modulated height
	// values to represent hills
	UInt tileXCount = 40;
	UInt tileYCount = 20;
	RenderObject* pPlane = StandardMesh::CreatePlane(tileXCount, tileYCount,
		5, 5, 0, 0, true);
	VertexBuffer* const pVBuffer = pPlane->GetMesh()->GetVertexBuffer();

	Float angleY = 0;
	for (UInt y = 0; y <= tileYCount; y++)
	{
		Float angleX = 0;
		Float factor = MathF::Sin(angleY);
		for (UInt x = 0; x <= tileXCount; x++)
		{
			Float height = MathF::Sin(angleX) * 0.2F * factor;
			UInt offset = (tileXCount+1) * y + x;
			Vector3F pos = pVBuffer->Position3(offset);
			pos.Z() = height;
			pVBuffer->Position3(offset) = pos;
			angleX += 0.5F;
		}

		angleY += 1;
	}

	// we changed the vertices so we need to recreate the model bound
	// (which was automatically created by the constructor of RenderObject)
	pPlane->GetMesh()->UpdateModelBound();
	pPlane->GetMesh()->GenerateNormals();

	Node* pPlaneNode = WIRE_NEW Node(pPlane);
	Matrix34F mat(Vector3F(1, 0, 0), MathF::DEG_TO_RAD * -90.0F,
		Vector3F(0, -0.25F, 0));
	pPlaneNode->Local.SetMatrix(mat, false);

	Light* pLight = WIRE_NEW Light(Light::LT_POINT);
	pPlaneNode->AttachLight(pLight);
	StateMaterial* pStateMaterial = WIRE_NEW StateMaterial;
	pStateMaterial->Ambient = ColorRGBA::GREEN;
	pPlaneNode->AttachState(pStateMaterial);

	return pPlaneNode;
}
