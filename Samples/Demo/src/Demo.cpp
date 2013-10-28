#include "Demo.h"

#include "Controllers/ConveyorBelt.h"
#include "Controllers/FanRotator.h"
#include "Controllers/LogoFader.h"
#include "Controllers/SplineCamera.h"
#include "Importer.h"

using namespace Wire;

WIRE_APPLICATION(Demo);

//----------------------------------------------------------------------------
Bool Demo::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	mspLogo = LoadAndInitLogo();
	if (!mspLogo)
	{
		WIRE_ASSERT(false /* Could not load Logo.xml */);
		return false;
	}

	mAppState = AS_LOADING;
	mLastTime = System::GetTime();
	mShowFps = false;

	// frames per second and render statistics debug text
	mspText = Importer::CreateText("Data/Logo/font.ttf", 12, 12);
	WIRE_ASSERT(mspText);
	GetRenderer()->Bind(mspText);

 	GetRenderer()->CreateBatchingBuffers(25*1024, 5*1024, 3);
 	GetRenderer()->SetDynamicBatchingThreshold(200, 200);
 	GetRenderer()->SetStaticBatchingThreshold(3000);

	return true;
}

//----------------------------------------------------------------------------
void Demo::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;

	mspLogoCamera->SetFrustum(0, GetWidthF(), 0, GetHeightF(), 0, 1);
	Vector3F centered((GetWidthF()-512)*0.5F, (GetHeightF()-256)*0.5F, 0);
	mspLogo->Local.SetTranslate(centered);

	switch (mAppState)
	{
	case AS_LOADING:
		StateLoading(elapsedTime);
		break;

	case AS_RUNNING:
		StateRunning(time);
		break;

	default:
		WIRE_ASSERT(false /* Invalid application state */);
	}
}

//----------------------------------------------------------------------------
void Demo::OnInput()
{
	if (GetInputSystem()->GetMainDevicesCount() == 0)
	{
		return;
	}

	const MainInputDevice* pInputDevice = GetInputSystem()->GetMainDevice(0);

	// checking for minimum capabilities
	if (!pInputDevice->HasCapability(Buttons::TYPE, true))
	{
		return;
	}

	const Buttons* pButtons = DynamicCast<Buttons>(pInputDevice->
		GetCapability(Buttons::TYPE, false));
	WIRE_ASSERT(pButtons);

	// when home is pressed, exit the game
	if (pButtons->GetButton(Buttons::BUTTON_HOME))
	{
		Close();
		return;
	}

	mShowFps = pButtons->GetButton(Buttons::BUTTON_B);
}

//----------------------------------------------------------------------------
void Demo::StateRunning(Double time)
{
	mspLogo->UpdateGS(time);
	mLogoCuller.ComputeVisibleSet(mspLogo);

	mspScene->UpdateGS(time);
	mSceneCuller.ComputeVisibleSet(mspScene);

	GetRenderer()->GetStatistics()->Reset();

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspSceneCamera);
	GetRenderer()->Draw(mSceneCuller.GetVisibleSets());

	GetRenderer()->SetCamera(mspLogoCamera);
	GetRenderer()->Draw(mLogoCuller.GetVisibleSets());

	static Double lastTime = 0.0F;
	Double elapsed = time - lastTime;
	lastTime = time;

	if (mShowFps)
	{
		Float fps = static_cast<Float>(1.0 / elapsed);
		GetRenderer()->GetStatistics()->Draw(mspText, Transformation::IDENTITY,
			fps);		
	}

	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
void Demo::StateLoading(Double elapsedTime)
{
	mspLogo->UpdateGS();
	mLogoCuller.ComputeVisibleSet(mspLogo);

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspLogoCamera);
	GetRenderer()->Draw(mLogoCuller.GetVisibleSets());
	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();

	mspScene = LoadAndInitScene();
	if (!mspScene)
	{
		WIRE_ASSERT(false /* Could not load Scene.xml */);
	}

	mAppState = AS_RUNNING;
}

//----------------------------------------------------------------------------
Node* Demo::LoadAndInitLogo()
{
	Importer importer("Data/Logo/");
	Node* pRoot = importer.LoadSceneFromXml("logo.xml");
	if (!pRoot)
	{
		return NULL;
	}

	NodeCamera* pCameraNode = pRoot->FindChild<NodeCamera>();
	WIRE_ASSERT(pCameraNode /* No Camera in Logo.xml */);
	mspLogoCamera = pCameraNode->Get();
	mLogoCuller.SetCamera(mspLogoCamera);

	Spatial* pLogo = pRoot->FindChild("Logo");
	if (pLogo)
	{
		pLogo->AttachController(WIRE_NEW LogoFader);
	}

	pRoot->Bind(GetRenderer());
	return pRoot;
}

//----------------------------------------------------------------------------
Node* Demo::LoadAndInitScene()
{
	Importer importer("Data/");
	Node* pScene = importer.LoadSceneFromXml("scene.xml");
	if (!pScene)
	{
		return NULL;
	}

	NodeCamera* pCameraNode = pScene->FindChild<NodeCamera>();
	WIRE_ASSERT(pCameraNode /* No Camera in Scene.xml */);
	mspSceneCamera = pCameraNode->Get();
	mSceneCuller.SetCamera(mspSceneCamera);

	// The maximum number of objects that are going to be culled is the
	// number of objects we imported. If we don't set the size of the set now,
	// the culler will dynamically increase it during runtime. This is not
	// a big deal, however it is better to avoid memory allocations during the
	// render loop.
	UInt renderObjectCount = importer.GetStatistics()->RenderObjectCount;
	mSceneCuller.SetMaxQuantity(renderObjectCount);

	// Use controllers to animate scene objects (rotating fan,
	// moving conveyor belt and spline camera).
	// Controllers are attached to scene objects and receive calls,
	// e.g. Update() durings the scene's UpdateGS(), OnGetVisibleUpdate()
	// from the culler for visible objects, etc.
	// See WireController.h for details.
	TArray<Spatial*> fans;
	pScene->FindChildren("ceilingFan", fans);
	for (UInt i = 0; i < fans.GetQuantity(); i++)
	{
		Float f = static_cast<Float>(i+1);
		fans[i]->AttachController(WIRE_NEW FanRotator(f));
	}

	Node* pSplineRoot = DynamicCast<Node>(pScene->FindChild("Spline"));
	pScene->AttachController(WIRE_NEW SplineCamera(pSplineRoot, mspSceneCamera));

	Node* pConveyorBelt = DynamicCast<Node>(pScene->FindChild(
		"polySurface437"));
	if (pConveyorBelt)
	{
		pConveyorBelt->AttachController(WIRE_NEW ConveyorBelt(pConveyorBelt->
			GetRenderObject(), GetRenderer()));
	}

	pScene->Bind(GetRenderer());
	pScene->WarmUpRendering(GetRenderer());
	return pScene;
}
