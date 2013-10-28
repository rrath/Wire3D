#include "Game.h"

#include "Importer.h"
#include "Elevator.h"

WIRE_APPLICATION(Game);

using namespace Wire;

//----------------------------------------------------------------------------
Game::Game() 
 	:
	mShowWarning(false),
	mCursorPosition(Vector2F::ZERO),
	mShowColliders(false),
	mShowFps(false),
	mWasButton1Pressed(false),
	mWasButton2Pressed(false)
{
}

//----------------------------------------------------------------------------
Bool Game::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	mspPhysicsWorld = WIRE_NEW PhysicsWorld();

	mspLogo = LoadAndInitializeLoading();

	if (!mspLogo)
	{
		WIRE_ASSERT(false /* Could not load logo.xml */);
		return false;
	}

	mAppState = AS_LOADING;
	mLastApplicationTime = System::GetTime();

	// Font for render statistics debug text
	mspText = Importer::CreateText("Data/Logo/font.ttf", 12, 12);
	WIRE_ASSERT(mspText);
	GetRenderer()->Bind(mspText);

	mspWarningText = Importer::CreateText("Data/Logo/font.ttf", 24, 24);
	WIRE_ASSERT(mspWarningText);
	GetRenderer()->Bind(mspWarningText);

	GetRenderer()->CreateBatchingBuffers(50*1024, 10*1024);
	GetRenderer()->SetDynamicBatchingThreshold(300, 100);
	GetRenderer()->SetStaticBatchingThreshold(700);

	return true;
}

//----------------------------------------------------------------------------
void Game::OnTerminate()
{
	mspPhysicsWorld = NULL;
}

//----------------------------------------------------------------------------
void Game::OnIdle()
{
	Double appTime = System::GetTime();
	Double deltaTime = appTime - mLastApplicationTime;
	mLastApplicationTime = appTime;

	mspLogoCamera->SetFrustum(0, GetWidthF(), 0, GetHeightF(), 0, 1);

	switch (mAppState)
	{
	case AS_LOADING:
		OnLoading(appTime, deltaTime);
		break;

	case AS_RUNNING:
		OnRunning(appTime, deltaTime);
		break;

	default:
		WIRE_ASSERT(false /* Invalid application state */);
	}
}

//----------------------------------------------------------------------------
void Game::ProcessInput()
{
	if (GetInputSystem()->GetMainDevicesCount() == 0)
	{
		return;
	}

	const MainInputDevice* pInputDevice = GetInputSystem()->GetMainDevice(0);

	// Checking minimum capabilities
	if (!pInputDevice->HasCapability(AnalogPad::TYPE, true) && !pInputDevice->
		HasCapability(DigitalPad::TYPE, true))
	{
		return;
	}

	if (!pInputDevice->HasCapability(IR::TYPE, true))
	{
		return;
	}

	if (!pInputDevice->HasCapability(Buttons::TYPE, true))
	{
		return;
	}

	// Processing IR
	//
	const IR* pIR = DynamicCast<const IR>(pInputDevice->GetCapability(IR::TYPE, false));
	WIRE_ASSERT(pIR);

	Vector2F cursorPosition(pIR->GetRight(), pIR->GetUp());

	// If the IR/mouse is pointing outside of the capture area,
	// set the lookAt vector to the center of the screen.
	if (cursorPosition.X() == MathF::MAX_REAL || cursorPosition.Y() == MathF::MAX_REAL)
	{
		// outside the screen
		mShowWarning = true;
		cursorPosition = Vector2F(GetWidthF() * 0.5F, GetHeightF() * 0.5F);
		if (mspCrosshair)
		{
			mspCrosshair->Culling = Spatial::CULL_ALWAYS;
		}

		if (mspPlayer)
		{
			mspPlayer->HideGun();
		}
	}
	else
	{
		mShowWarning = false;
		// Height correction
		cursorPosition.Y() = GetHeightF() - cursorPosition.Y();
		if (mspCrosshair)
		{
			mspCrosshair->Culling = Spatial::CULL_DYNAMIC;
		}

		if (mspPlayer)
		{
			mspPlayer->ShowGun();
		}
	}

	mCursorPosition = cursorPosition;
	if (mspCrosshair)
	{
		mspCrosshair->Local.SetTranslate(Vector3F(cursorPosition.X() - 16,
			cursorPosition.Y() - 16, 0));
	}

	// Converting from (top, left) to (horizontal screen center, vertical screen center) coordinate system
	cursorPosition.X() -= GetWidthF() * 0.5F;
	cursorPosition.Y() -= GetHeightF() * 0.5F;

	if (mspPlayer)
	{
		mspPlayer->LookAt(cursorPosition);
	}

	// Processing buttons/keys
	//
	const Buttons* pButtons = DynamicCast<const Buttons>(pInputDevice->
		GetCapability(Buttons::TYPE, false));
	WIRE_ASSERT(pButtons);

	// '1' button/key toggles display of renderer statistics
	if (pButtons->GetButton(Buttons::BUTTON_1))
	{
		if (!mWasButton1Pressed)
		{
			mShowFps = !mShowFps;
			mWasButton1Pressed = true;
		}
	}
	else
	{
		mWasButton1Pressed = false;
	}

	// '2' button/key toggles debug draw of physics collision shapes
	if (pButtons->GetButton(Buttons::BUTTON_2))
	{
		if (!mWasButton2Pressed)
		{
			mShowColliders = !mShowColliders;
			mspPhysicsWorld->ToggleDebugShapes(mShowColliders);
			mWasButton2Pressed = true;
		}
	}
	else
	{
		mWasButton2Pressed = false;
	}
}

//----------------------------------------------------------------------------
void Game::OnRunning(Double time, Double deltaTime)
{
	ProcessInput();

	mspPhysicsWorld->StepSimulation(deltaTime, 10);

	mspScene->UpdateGS(time);
	mSortingCuller.ComputeVisibleSet(mspScene);

	mspGUI->UpdateGS(time);
	mspGUICamera->SetFrustum(0, GetWidthF(), 0, GetHeightF(), 0, 1);
	mCuller.SetCamera(mspGUICamera);
	mCuller.ComputeVisibleSet(mspGUI);

	GetRenderer()->GetStatistics()->Reset();
	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspSceneCamera);
	GetRenderer()->Draw(mSortingCuller.GetVisibleSets());

	GetRenderer()->SetCamera(mspGUICamera);
	GetRenderer()->Draw(mCuller.GetVisibleSets());

	if (mShowWarning)
	{
		DrawWarning(time);
	}

	if (mShowFps)
	{
		GetRenderer()->GetStatistics()->Draw(mspText, Transformation::IDENTITY,
			static_cast<Float>(1.0 / deltaTime));
	}

	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
void Game::OnLoading(Double time, Double deltaTime)
{
	Spatial* pLoading = mspLogo->FindChild("Loading");
	Bool isFadedIn = false;

	if (pLoading)
	{
		StateMaterial* pMaterialState = DynamicCast<StateMaterial>(pLoading->
			GetState(State::MATERIAL));
		if (pMaterialState)
		{
			pMaterialState->Ambient.A() += static_cast<Float>(deltaTime);

			if (pMaterialState->Ambient.A() > 1.0F)
			{
				pMaterialState->Ambient.A() = 1.0F;
				isFadedIn = true;
			}
		}
	}

	mspLogo->UpdateGS(time);

	mCuller.SetCamera(mspLogoCamera);
	mCuller.ComputeVisibleSet(mspLogo);

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspLogoCamera);
	GetRenderer()->Draw(mCuller.GetVisibleSets());
	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();

	if (!isFadedIn)
	{
		return;
	}

	mspGUI = LoadAndInitializeGUI();
	if (!mspGUI)
	{
		WIRE_ASSERT(false /* Could not load GUI.xml */);
	}

	mspScene = LoadAndInitializeScene();
	if (!mspScene)
	{
		WIRE_ASSERT(false /* Could not load scene.xml */);
	}

	mAppState = AS_RUNNING;
	pLoading->Culling = Spatial::CULL_ALWAYS;

	mspLogo = NULL; // discard Logo scene graph to free memory
}

//----------------------------------------------------------------------------
Node* Game::LoadAndInitializeLoading()
{
	Importer importer("Data/Logo/");
	Node* pRoot = importer.LoadSceneFromXml("logo.xml");
	if (!pRoot)
	{
		return NULL;
	}

	NodeCamera* pCameraNode = pRoot->FindChild<NodeCamera>();
	WIRE_ASSERT(pCameraNode /* No Camera in logo.xml */);
	mspLogoCamera = pCameraNode->Get();

	// center 512x256 logo on screen
	pRoot->Local.SetTranslate(Vector3F((GetWidthF()-512.0F) * 0.5F,
		(GetHeightF() - 256.0F)  * 0.5F, 0));

	pRoot->Bind(GetRenderer());
	return pRoot;
}

//----------------------------------------------------------------------------
Node* Game::LoadAndInitializeGUI()
{
	Importer importer("Data/GUI/");
	Node* pRoot = importer.LoadSceneFromXml("GUI.xml");
	if (!pRoot)
	{
		return NULL;
	}

	NodeCamera* pCameraNode = pRoot->FindChild<NodeCamera>();
	WIRE_ASSERT(pCameraNode /* No Camera in GUI.xml */);
	mspGUICamera = pCameraNode->Get();

	mspCrosshair = pRoot->FindChild("Crosshair");
	WIRE_ASSERT(mspCrosshair /* No Crosshair in GUI.xml */);

	pRoot->Bind(GetRenderer());

	return pRoot;
}

//----------------------------------------------------------------------------
Node* Game::LoadAndInitializeScene()
{
	Importer importer("Data/Scene/");
	Node* pScene = importer.LoadSceneFromXml("Scene.xml", mspPhysicsWorld);
	if (!pScene)
	{
		return NULL;
	}

	NodeCamera* pCameraNode = pScene->FindChild<NodeCamera>();
	WIRE_ASSERT(pCameraNode /* No Camera in scene.xml */);
	mspSceneCamera = pCameraNode->Get();
	mSortingCuller.SetCamera(mspSceneCamera);

	// The maximum number of objects that are going to be culled is the
	// number of objects we imported. If we don't set the size of the set now,
	// the culler will dynamically increase it during runtime. This is not
	// a big deal, however it is better to avoid memory allocations during the
	// render loop.
	UInt renderObjectCount = importer.GetStatistics()->RenderObjectCount;
	mSortingCuller.SetMaxQuantity(renderObjectCount);

	// Create and configure probe robot controller
	SpatialPtr spRedHealthBar = mspGUI->FindChild("RedHealthBar");
	WIRE_ASSERT(spRedHealthBar /* No RedHealthBar in GUI.xml */);

	Node* pProbeRobotSpatial = DynamicCast<Node>(pScene->FindChild("Probe Robot"));
	WIRE_ASSERT(pProbeRobotSpatial /* No Probe Robot in Scene.xml */);

	// Detach red energy/health bar and attach it robot probe as a billboard
	NodeBillboard* pBillboard = WIRE_NEW NodeBillboard;
	pProbeRobotSpatial->AttachChild(pBillboard);
	Node* pParent = DynamicCast<Node>(spRedHealthBar->GetParent());
	WIRE_ASSERT(pParent);
	pParent->DetachChild(spRedHealthBar);
	pBillboard->AttachChild(spRedHealthBar);

	Spatial* pPlayerSpatial = pScene->FindChild("Player");
	WIRE_ASSERT(pPlayerSpatial /* No Player in Scene.xml */);

	mspProbeRobot = WIRE_NEW ProbeRobot(mspPhysicsWorld, pPlayerSpatial,
		spRedHealthBar);
	pProbeRobotSpatial->AttachController(mspProbeRobot);

	// Create and configure player controller
	mspPlayer = WIRE_NEW Player(mspSceneCamera, mspPhysicsWorld);
	pPlayerSpatial->AttachController(mspPlayer);

 	Spatial* pPlatform = pScene->FindChild("Platform");
  	WIRE_ASSERT(pPlatform /* Platform object missing in scene */);
  	pPlatform->AttachController(WIRE_NEW Elevator(mspPhysicsWorld));

	pScene->Bind(GetRenderer());
	pScene->WarmUpRendering(GetRenderer());

	return pScene;
}

//----------------------------------------------------------------------------
void Game::DrawWarning(Double time)
{
	GetRenderer()->DisableLighting();
	Float f = (MathF::Sin(MathF::FMod(static_cast<Float>(time*5),
		MathF::TWO_PI)) + 1) * 0.5F;
	ColorRGB color(1, f, f);
	mspWarningText->Clear(color);
	mspWarningText->Append("Aim at the screen!");
	mspWarningText->Update(GetRenderer());

	Vector3F center = mspWarningText->GetMesh()->GetModelBound()->GetCenter();
	center.Y() -= static_cast<Float>(mpRenderer->GetHeight()) * 0.5F;
	center.X() -= static_cast<Float>(mpRenderer->GetWidth()) * 0.5F;
	Transformation translate;
	translate.SetTranslate(-center);

	GetRenderer()->Draw(mspWarningText, translate);
}
