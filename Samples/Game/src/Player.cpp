#include "Player.h"

#include "ProbeRobot.h"
#include "PhysicsWorld.h"
#include "WireApplication.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(Player, PhysicsController);

//----------------------------------------------------------------------------
Player::Player(Camera* pCamera, PhysicsWorld* pPhysicsWorld)
	:
	PhysicsController(pPhysicsWorld),
	mHeadHeight(1.0F),
	mMaximumShootingDistance(1000.0F),
	mMaximumVerticalAngle(MathF::DEG_TO_RAD * 45.0F),
	mLookUpDeadZone(Vector2F(0.5F, 0.5F)),
	mRotateSpeed(MathF::PI),
	mMoveSpeed(5.0F),
	mPitch(0),
	mYaw(0),
	mPitchIncrement(0),
	mYawIncrement(0),
	mRolls(3, 0),
	mMove(Vector3F::ZERO),
	mLookAt(Vector2F::ZERO),
	mpNode(NULL),
	mspCamera(pCamera),
	mShoot(0),
	mJump(false)
{
	WIRE_ASSERT(pCamera);

	SetEnabled(true);
}

//----------------------------------------------------------------------------
Player::~Player()
{
}

//----------------------------------------------------------------------------
Bool Player::Update(Double appTime)
{
	Float deltaTime = static_cast<Float>(appTime - mLastApplicationTime);
	if (mLastApplicationTime == -MathD::MAX_REAL)
	{
		deltaTime = 0.0F;
	}

	mLastApplicationTime = appTime;

	ProcessInput();

	// Apply accumulators
	if (mpPhysicsWorld)
	{
		mMove *= static_cast<Float>(mpPhysicsWorld->GetFixedTimeStep());
	}

	mPitch += (mPitchIncrement * deltaTime);
	mPitch = MathF::Clamp(-mMaximumVerticalAngle, mPitch, mMaximumVerticalAngle);
	mYaw += mYawIncrement * deltaTime;
	if (mYaw > MathF::TWO_PI) 
	{
		mYaw = mYaw - MathF::TWO_PI;
	}
	else if (mYaw < -MathF::TWO_PI)
	{
		mYaw = MathF::TWO_PI - mYaw;
	}

	// Calculate rotation matrices
	Matrix3F rotationX(Vector3F::UNIT_X, mPitch);
	Matrix3F rotationY(Vector3F::UNIT_Y, mYaw);
	Matrix3F rotationYX = rotationY * rotationX;

	// Calculate up, gunDirection and lookup
	Vector3F eyeDirection = rotationYX.GetColumn(2);
	Vector3F up = rotationYX.GetColumn(1);
	mForward = rotationY.GetColumn(2);
	mRight = eyeDirection.Cross(up);

	// update player node
	mpNode->Local.SetTranslate(GetPosition());
	mpNode->Local.SetRotate(rotationYX);

	// update camera
	mspCamera->SetFrame(GetPosition(), eyeDirection, up, mRight);

	UpdateGun(deltaTime);

	// move physics entity
	if (mJump)
	{
		mspCharacter->GetCharacter()->jump();
		mJump = false;
	}

	mspCharacter->GetCharacter()->setWalkDirection(PhysicsWorld::Convert(mMove));

	// Reset accumulators
	mMove = Vector3F::ZERO;
	mPitchIncrement = 0;
	mYawIncrement = 0;

	return true;
}

//----------------------------------------------------------------------------
void Player::ProcessInput()
{
	Application* pApplication = Application::GetApplication();
	InputSystem* pInputSystem = pApplication->GetInputSystem();
	if (!pInputSystem)
	{
		return;
	}

	if (pInputSystem->GetMainDevicesCount() == 0)
	{
		return;
	}

	const MainInputDevice* pInputDevice = pInputSystem->GetMainDevice(0);

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

	// Processing analog/digital pad
	//
	if (pInputDevice->HasCapability(AnalogPad::TYPE, true))
	{
		const AnalogPad* pAnalogPad = DynamicCast<const AnalogPad>(pInputDevice->
			GetCapability(AnalogPad::TYPE, true));
		WIRE_ASSERT(pAnalogPad);

		if (pAnalogPad->GetUp() > 0)
		{
			MoveForward();
		}
		else if (pAnalogPad->GetDown() > 0)
		{
			MoveBackward();
		}

		if (pAnalogPad->GetRight() > 0)
		{
			StrafeRight();
		}
		else if (pAnalogPad->GetLeft() > 0)
		{
			StrafeLeft();
		}
	}
	else 
	{
		const DigitalPad* pDigitalPad = DynamicCast<const DigitalPad>(pInputDevice->
			GetCapability(DigitalPad::TYPE, false));
		WIRE_ASSERT(pDigitalPad);

		if (pDigitalPad->GetUp())
		{
			MoveForward();
		}
		else if (pDigitalPad->GetDown())
		{
			MoveBackward();
		}

		if (pDigitalPad->GetLeft())
		{
			StrafeLeft();
		}
		else if (pDigitalPad->GetRight())
		{
			StrafeRight();
		}
	}

	// Processing buttons
	//
	const Buttons* pButtons = DynamicCast<const Buttons>(pInputDevice->
		GetCapability(Buttons::TYPE, false));
	WIRE_ASSERT(pButtons);

	// 'A' button makes the player shoot
	if (pButtons->GetButton(Buttons::BUTTON_A))
	{
		if (mpGun && mpGun->Culling == Spatial::CULL_DYNAMIC)
		{
			ShootGun();
		}
	}

	// 'B' button makes the player jump
	if (pButtons->GetButton(Buttons::BUTTON_B))
	{
		Jump();
	}

	// If there's a nunchuk, start reading its buttons instead
	if (pInputDevice->HasExtension(Nunchuk::TYPE))
	{
		pButtons = DynamicCast<const Buttons>(pInputDevice->GetExtension(Nunchuk::TYPE)->
			GetCapability(Buttons::TYPE));
		WIRE_ASSERT(pButtons);
	}

	// 'Z' button makes the player run
	if (pButtons->GetButton(Buttons::BUTTON_Z))
	{
		SetMoveSpeed(8.0F);
	}
	else
	{
		SetMoveSpeed(4.0F);
	}

	// get the main device tilt (win32: mouse wheel) (in degrees)
	const Tilt* pTilt = DynamicCast<const Tilt>(pInputDevice->
		GetCapability(Tilt::TYPE, false));
	if (pTilt)
	{
		Float tilt = MathF::DEG_TO_RAD * (pTilt->GetLeft());
		if (mRolls.GetQuantity() == mRolls.GetMaxQuantity())
		{
			for (UInt i = 1; i < mRolls.GetQuantity(); i++)
			{
				mRolls[i-1] = mRolls[i];
			}

			mRolls[mRolls.GetQuantity()-1] = tilt;
		}
		else
		{
			mRolls.Append(tilt);
		}
	}
}

//----------------------------------------------------------------------------
void Player::OnAttach()
{
	mpNode = DynamicCast<Node>(GetSceneObject());
	WIRE_ASSERT(mpNode);

	// Init gun
	mpGun = DynamicCast<Node>(mpNode->FindChild("Gun"));
	WIRE_ASSERT(mpGun);
	Node* pMuzzleFlash = DynamicCast<Node>(mpGun->FindChild("muzzleFlash"));
	WIRE_ASSERT(pMuzzleFlash);

	mspMuzzleflashMaterialState = pMuzzleFlash->GetState<StateMaterial>();
	WIRE_ASSERT(mspMuzzleflashMaterialState);

	NodeLight* pLightNode = mpNode->FindChild<NodeLight>();
	WIRE_ASSERT(pLightNode);
	mspMuzzleflashLight = pLightNode->Get();
	mMuzzleflashLightColor = mspMuzzleflashLight->Color;

	// since render state cannot be extracted from Unity materials, we have
	// to set the appropriate states manually for the muzzle flash
	StateCull* pCullState = WIRE_NEW StateCull;
	pCullState->CullFace = StateCull::CM_OFF;
	pMuzzleFlash->AttachState(pCullState);

	StateAlpha* pAlphaState = WIRE_NEW StateAlpha;
	pAlphaState->BlendEnabled = true;
	pAlphaState->SrcBlend = StateAlpha::SBM_SRC_ALPHA;
	pAlphaState->DstBlend = StateAlpha::DBM_ONE;
	pMuzzleFlash->AttachState(pAlphaState);

	StateZBuffer* pZBufferState = WIRE_NEW StateZBuffer;
	pZBufferState->Writable = false;
	pMuzzleFlash->AttachState(pZBufferState);
	pMuzzleFlash->UpdateRS();

	WIRE_ASSERT(pMuzzleFlash->GetRenderObject());
	pMuzzleFlash->GetRenderObject()->GetLights()->RemoveAll();
	Light* pLight = WIRE_NEW Light(Light::LT_POINT);
	pLight->Ambient = ColorRGB::WHITE;
	pMuzzleFlash->GetRenderObject()->GetLights()->Append(pLight);
	pMuzzleFlash->AttachChild(WIRE_NEW NodeLight(pLight));
	pMuzzleFlash->GetRenderObject()->GetMaterial()->SetBlendMode(Material::BM_MODULATE);

	mspCharacter = mpNode->GetController<CharacterController>();
	WIRE_ASSERT(mspCharacter);

	// Add a reference to Player in the physics object
	mspCharacter->Get()->setUserPointer(this);
}

//----------------------------------------------------------------------------
void Player::SetMoveSpeed(Float moveSpeed)
{
	mMoveSpeed = moveSpeed;
}

//----------------------------------------------------------------------------
void Player::MoveForward()
{
	mMove += mForward * mMoveSpeed;
}

//----------------------------------------------------------------------------
void Player::MoveBackward()
{
	mMove -= mForward * mMoveSpeed;
}

//----------------------------------------------------------------------------
void Player::StrafeLeft()
{
	mMove -= mRight * mMoveSpeed;
}

//----------------------------------------------------------------------------
void Player::StrafeRight()
{
	mMove += mRight * mMoveSpeed;
}

//----------------------------------------------------------------------------
void Player::Jump()
{
	mJump = true;
}

//----------------------------------------------------------------------------
void Player::ShootGun()
{
	if (mShoot > -1.0F)
	{
		return;
	}

	mShoot = 1;
}

//----------------------------------------------------------------------------
void Player::LookAt(const Vector2F& rLookAt)
{
	mLookAt = rLookAt;
	Application* pApp = Application::GetApplication();

	const Float width = pApp->GetWidthF() * 0.5F;
	Float deadZoneX = width * mLookUpDeadZone.X();
	Float xB = width - deadZoneX;
	if (rLookAt.X() > deadZoneX)
	{
		mYawIncrement -= mRotateSpeed * (rLookAt.X() - deadZoneX) / xB;
	}
	else if (rLookAt.X() < -deadZoneX)
	{
		mYawIncrement -= mRotateSpeed * (rLookAt.X() + deadZoneX) / xB;
	}

	const Float height = pApp->GetHeightF() * 0.5F;
	Float deadZoneY = height * mLookUpDeadZone.Y();
	Float yB = height - deadZoneY;
	if (rLookAt.Y() > deadZoneY)
	{
		mPitchIncrement -= mRotateSpeed * (rLookAt.Y() - deadZoneY) / yB;
	}
	else if (rLookAt.Y() < -deadZoneY)
	{
		mPitchIncrement -= mRotateSpeed * (rLookAt.Y() + deadZoneY) / yB;
	}
}

//----------------------------------------------------------------------------
void Player::ShowGun()
{
	if (mpGun)
	{
		mpGun->Culling = Spatial::CULL_DYNAMIC;
	}
}

//----------------------------------------------------------------------------
void Player::HideGun()
{
	if (mpGun)
	{
		mpGun->Culling = Spatial::CULL_ALWAYS;
	}
}

//----------------------------------------------------------------------------
Vector3F Player::GetPosition()
{
	btVector3 origin = mspCharacter->Get()->getWorldTransform().getOrigin();
	return Vector3F(origin.x(), origin.y() + mHeadHeight, origin.z());
}

//----------------------------------------------------------------------------
void Player::UpdateGun(Double deltaTime)
{
	Float width = Application::GetApplication()->GetWidthF();
	Float height = Application::GetApplication()->GetHeightF();
	Vector2F cursorPosition(mLookAt.X()*2/width, mLookAt.Y()*2/height);

	Matrix4F projectionMatrix = mspCamera->GetProjectionMatrix();
	Vector3F pickDirection(-cursorPosition.X() / projectionMatrix(0, 0),
		cursorPosition.Y() / projectionMatrix(1, 1), 1);

	Vector3F tempUp(0, 1, 0);
	Vector3F right = pickDirection.Cross(tempUp);
	Vector3F up = right.Cross(pickDirection);
	pickDirection.Normalize();
	right.Normalize();
	up.Normalize();
	Matrix3F mat(-right, up, pickDirection, true);

	// to reduce the Wiimote's tilt jitter we average the last few sampled
	// tilt values
	Float tilt = 0;
	if (mRolls.GetQuantity() > 0)
	{
		for (UInt i = 0; i < mRolls.GetQuantity(); i++)
		{
			tilt += mRolls[i];
		}

		tilt /= mRolls.GetQuantity();
	}

	Matrix3F roll(Vector3F(0, 0, 1), tilt);
	mpGun->Local.SetRotate(mat * roll);

	UpdateShot(deltaTime, cursorPosition);
}

//----------------------------------------------------------------------------
void Player::UpdateShot(Double deltaTime, const Vector2F& rCursorPosition)
{
	// If not shooting, exit
	if (mShoot < 1)
	{
		Float alpha = mShoot < 0 ? 0 : mShoot;
		mspMuzzleflashMaterialState->Ambient.A() = alpha;

		mspMuzzleflashLight->Color = mMuzzleflashLightColor * alpha;

		mShoot -= static_cast<Float>(deltaTime)*10.0F;
		return;
	}

	mShoot -= static_cast<Float>(deltaTime)*10.0F;

	Vector3F origin;
	Vector3F direction;
	mspCamera->GetPickRay(rCursorPosition, origin, direction);
	direction.Normalize();
	btVector3 rayStart = PhysicsWorld::Convert(origin);
	btVector3 rayEnd = rayStart + PhysicsWorld::Convert(direction * mMaximumShootingDistance);

	btCollisionWorld::ClosestRayResultCallback hitCallback(rayStart, rayEnd);

	if (!mpPhysicsWorld)
	{
		return;
	}

	mpPhysicsWorld->Get()->rayTest(rayStart, rayEnd, hitCallback);
	if (hitCallback.hasHit()) 
	{
		btCollisionObject* pColObj = hitCallback.m_collisionObject;
 		if (!pColObj->isStaticOrKinematicObject())
 		{
			btRigidBody* pRigidBody = btRigidBody::upcast(pColObj);
			if (pRigidBody)
			{
				btVector3 y = hitCallback.m_hitPointWorld - pRigidBody->getCenterOfMassPosition();
				pColObj->activate(true);
				pRigidBody->applyImpulse(PhysicsWorld::Convert(direction)*7.5F, y);
			}
 		}

		ProbeRobot* pProbeRobotController = static_cast<ProbeRobot*>(hitCallback.m_collisionObject->getUserPointer());
		if (pProbeRobotController) 
		{
			pProbeRobotController->TakeDamage(5.0F);
		}
	}
}
