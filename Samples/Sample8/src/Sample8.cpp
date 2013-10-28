// Sample8 - Bullet Physics Integration
// This sample demonstrates how to use the Bullet Physics engine in Wire.

#include "Sample8.h"

using namespace Wire;

WIRE_APPLICATION(Sample8);

//----------------------------------------------------------------------------
Sample8::Sample8()
	:
	mAngle(0.0F),
	mLastTime(0.0),
	mpCollisionConfiguration(NULL),
	mpDispatcher(NULL),
	mpOverlappingPairCache(NULL),
	mpSolver(NULL),
	mpDynamicsWorld(NULL)
{
}

//----------------------------------------------------------------------------
Bool Sample8::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	// create the bullet objects necessary to run a rigid body simulation
	CreatePhysicsWorld();

	// create the physics objects and the rendering objects representing them
	CreateGameObjects();

	mspCamera = WIRE_NEW Camera;
	mspCamera->SetFrustum(45.0F, GetWidthF()/GetHeightF(), 0.1F, 300.0F);

	mspLightA = WIRE_NEW Light;
	mspLightA->Direction = Vector3F(0.5F, -0.2F, 1.0F);
	mspLightA->Color = ColorRGB(0.8F, 0.7F, 0.5F);
	mspLightB = WIRE_NEW Light;
	mspLightB->Direction = Vector3F(-0.5F, -0.2F, -1.0F);
	mspLightB->Color = ColorRGB(0.7F, 0.8F, 0.5F);

	mspStateMaterial = WIRE_NEW StateMaterial;
	mspStateMaterial->Ambient = ColorRGBA::WHITE;

	mspWorldBound = WIRE_NEW SphereBV;

	Double time = System::GetTime();
	mResetTime = time + 4.0F;
	return true;
}

//----------------------------------------------------------------------------
void Sample8::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;
	mAngle += static_cast<Float>(elapsedTime*0.5);
	mAngle = MathF::FMod(mAngle, MathF::TWO_PI);

	if (time > mResetTime)
	{
		mResetTime += 4.0F;
		ResetRigidBodies();
	}

	Float sinus = MathF::Sin(mAngle); 
	Float d = 20.0F + 10 * sinus;
	Float camY = -1.5F;
	Vector3F camPos(sinus*d, camY, MathF::Cos(mAngle)*d);
	mspCamera->LookAt(camPos, Vector3F(0, camY, 0), Vector3F::UNIT_Y);
	mCuller.SetCamera(mspCamera);

	UpdatePhysicsWorld(static_cast<btScalar>(elapsedTime));
	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspCamera);

	GetRenderer()->SetState(mspStateMaterial);
	GetRenderer()->SetLight(mspLightA, 0);
	GetRenderer()->SetLight(mspLightB, 1);
	GetRenderer()->EnableLighting();

	for (UInt i = 0; i < mGameObjects.GetQuantity(); i++)
	{
		RenderObject* pRenderObject = mGameObjects[i].spRenderObject;
		WIRE_ASSERT(pRenderObject);

		Transformation& rWorld = mGameObjects[i].WorldTransformation;
		pRenderObject->GetMesh()->GetModelBound()->TransformBy(rWorld,
			mspWorldBound);
		if (mCuller.IsVisible(mspWorldBound))
		{
			GetRenderer()->Draw(pRenderObject, rWorld);
		}
	}

	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
void Sample8::OnTerminate()
{
	DestroyPhysicsWorld();
}

//----------------------------------------------------------------------------
void Sample8::CreateGameObjects()
{
	WIRE_ASSERT(mpDynamicsWorld); // Has the physics world been created?

	Material* pMaterial = CreateMaterial();
	
	// create a few basic rigid bodies and their rendering representation
	///Please don't make the box sizes larger then 1000: the collision
	// detection will be inaccurate.
	///See http://www.continuousphysics.com/Bullet/phpBB2/viewtopic.php?t=346

	// create the (static, i.e. mass = 0) ground floor
	const Float floorX = 10;
	const Float floorY = 0.25F;
	const Float floorZ = 10;
	const Float floorMass = 0;

	// using a smart pointer in local scope here because we only need the 
	// RenderObject from the StandardMesh returned Node so the rest of the
	// node gets deleted automatically when returning from this function.
	RenderObject* pFloor = StandardMesh::CreateCube24(0, 0, true, 0.5F);

	btCollisionShape* pColFloor = WIRE_NEW btBoxShape(btVector3(
		btScalar(floorX), btScalar(floorY), btScalar(floorZ)));
	btRigidBody* pRigidFloor = CreateRigidBody(pColFloor, floorMass,
		Vector3F(0, -4.0F, 0));

	GameObject groundFloor(pFloor, pColFloor, pRigidFloor);
	groundFloor.WorldTransformation.SetScale(Vector3F(floorX * 2, floorY,
		floorZ * 2));
	mGameObjects.Append(groundFloor);

	// 1 RenderObject Box to reuse 
	const Float extent = 0.5F;
	mspBox = StandardMesh::CreateCube24(0, 1, true, extent);
	mspBox->SetMaterial(pMaterial);
	mspBox->GetMesh()->GenerateNormals();

	// create a stack of boxes
	Bool switchX = false;
	Float y = -3.25F;
	for (UInt yCount = 0; yCount < s_BoxCountY;	yCount++)
	{
		Float x = static_cast<Float>(s_BoxCountX)*-0.5F;
		for (UInt xCount = 0; xCount < s_BoxCountX; xCount++)
		{
			btCollisionShape* pColBox = WIRE_NEW btBoxShape(btVector3(extent,
				extent, extent));
			const Float mass = 1.0F;
			btRigidBody* pRigidBox = CreateRigidBody(pColBox, mass,
				Vector3F(switchX ? x : x+0.35F, y, 0));

			GameObject box(mspBox, pColBox, pRigidBox);
			mGameObjects.Append(box);
			x += 1.25F;
		}

		y += 1.0F;
		switchX = !switchX;
	}


	// 1 RenderObject Ball to reuse 
	const Float radius = 0.75F;
	mspBall = StandardMesh::CreateSphere(16, 16, radius, 0, 0, true);

	// create 2 balls that collide with the boxes and cause them to collapse
	Float sign = 1;
	for (UInt i = 0; i < 2; i++)
	{
		btCollisionShape* pColBall = WIRE_NEW btSphereShape(radius);
		const Float mass = 1.0F;
		btRigidBody* pRigidBall = CreateRigidBody(pColBall, mass,
			Vector3F(1.5F * sign, -0.5F, 10 * sign));
		RandomizeBallVelocity(pRigidBall);

		GameObject ball(mspBall, pColBall, pRigidBall);
		mGameObjects.Append(ball);

		sign = -sign;
	}
}

//----------------------------------------------------------------------------
Material* Sample8::CreateMaterial()
{
	const UInt width = 64;
	const UInt height = 64;
	const Image2D::FormatMode format = Image2D::FM_RGB888;
	const UInt bpp = Image2D::GetBytesPerPixel(format);

	NoisePerlin<Float> perlin;

	const Float hf = static_cast<Float>(height) * 0.25F;
	const Float wf = static_cast<Float>(width) * 0.25F;
	UChar* const pDst = WIRE_NEW UChar[width * height * bpp];

	for (UInt y = 0; y < height; y++)
	{
		for (UInt x = 0; x < width; x++)
		{
			Float xf = static_cast<Float>(x);
			Float yf = static_cast<Float>(y);

			UChar t = static_cast<UChar>((perlin.Noise(xf/wf, yf/hf)*0.5F
				+ 0.5F) * 255.0F);
			pDst[(y*width + x)*bpp] = t;
			pDst[(y*width + x)*bpp+1] = t;
			pDst[(y*width + x)*bpp+2] = t;
		}
	}

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pDst);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);

	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(pTexture, Material::BM_MODULATE);

	return pMaterial;
}

//----------------------------------------------------------------------------
void Sample8::CreatePhysicsWorld()
{
	///collision configuration contains default setup for memory.
	// Advanced users can create their own configuration.
	mpCollisionConfiguration = WIRE_NEW btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you
	// can use a different dispatcher (see Extras/BulletMultiThreaded)
	mpDispatcher = WIRE_NEW btCollisionDispatcher(mpCollisionConfiguration);

	///the maximum size of the collision world. Make sure objects stay
	// within these boundaries
	///Don't make the world AABB size too large, it will harm simulation
	// quality and performance
	btVector3 worldAabbMin(-100, -100, -100);
	btVector3 worldAabbMax(100, 100, 100);
	UShort maxProxies = 1024;
	mpOverlappingPairCache = WIRE_NEW btAxisSweep3(worldAabbMin, worldAabbMax,
		maxProxies);

	///the default constraint solver. For parallel processing you can use
	// a different solver (see Extras/BulletMultiThreaded)
	mpSolver = WIRE_NEW btSequentialImpulseConstraintSolver;

	mpDynamicsWorld = WIRE_NEW btDiscreteDynamicsWorld(mpDispatcher,
		mpOverlappingPairCache, mpSolver, mpCollisionConfiguration);

	mpDynamicsWorld->setGravity(btVector3(0, -10, 0));
}

//----------------------------------------------------------------------------
btRigidBody* Sample8::CreateRigidBody(btCollisionShape* pColShape, Float mass,
	Vector3F position, Float extent /*= 1.0F*/)
{
	pColShape->setMargin(0.05F);

	/// Create Dynamic Objects
	btTransform trafo;
	trafo.setIdentity();
	btVector3 origin = Convert(position);
	trafo.setOrigin(origin);

	//rigidbody is dynamic if and only if mass is non zero
	Bool isDynamic = (mass != 0.0F);
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
	{
		pColShape->calculateLocalInertia(mass, localInertia);
	}

	//using motionstate is recommended, it provides interpolation
	//capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* pMotionState = WIRE_NEW btDefaultMotionState(
		trafo);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, pMotionState,
		pColShape, localInertia);
	btRigidBody* pBody = WIRE_NEW btRigidBody(rbInfo);

	// Only do CCD if  motion in one timestep (1/60) exceeds 
	pBody->setCcdMotionThreshold(extent*0.5F);

	//Experimental: better estimation of CCD Time of Impact:
	pBody->setCcdSweptSphereRadius(extent*0.2F);

	mpDynamicsWorld->addRigidBody(pBody);
	return pBody;
}

//----------------------------------------------------------------------------
void Sample8::ResetRigidBodies()
{
	if (!mpDynamicsWorld)
	{
		return;
	}

	Int objectCount = mpDynamicsWorld->getNumCollisionObjects();

	///create a copy of the array, not a reference!
	btCollisionObjectArray copy = mpDynamicsWorld->getCollisionObjectArray();

	for (Int i = 0; i < objectCount; i++)
	{
		btCollisionObject* pColObj = copy[i];
		btRigidBody* pBody = btRigidBody::upcast(pColObj);
		if (!pBody)
		{
			continue;
		}

		if (pBody->getMotionState())
		{
			btDefaultMotionState* pState = static_cast<
				btDefaultMotionState*>(pBody->getMotionState());
			pState->m_graphicsWorldTrans = pState->m_startWorldTrans;
			pBody->setCenterOfMassTransform(pState->m_graphicsWorldTrans);
			pColObj->setInterpolationWorldTransform(pState->m_startWorldTrans);
			pColObj->forceActivationState(ACTIVE_TAG);
			pColObj->activate();
			pColObj->setDeactivationTime(0);
			//pColObj->setActivationState(WANTS_DEACTIVATION);
		}

		//removed cached contact points (this is not necessary if all
		// objects have been removed from the dynamics world)
		btBroadphaseInterface* pPhase = mpDynamicsWorld->getBroadphase();
		if (pPhase->getOverlappingPairCache())
		{
			pPhase->getOverlappingPairCache()->cleanProxyFromPairs(
				pColObj->getBroadphaseHandle(), mpDynamicsWorld->getDispatcher());
		}

		if (pBody && !pBody->isStaticObject())
		{
			pBody->setLinearVelocity(btVector3(0,0,0));
			pBody->setAngularVelocity(btVector3(0,0,0));

			if (pColObj->getCollisionShape()->getShapeType() ==  
				SPHERE_SHAPE_PROXYTYPE)
			{
				RandomizeBallVelocity(pBody);
			}
		}
	}

	///reset some internal cached data in the broadphase
	mpDynamicsWorld->getBroadphase()->resetPool(mpDynamicsWorld->
		getDispatcher());
	mpDynamicsWorld->getConstraintSolver()->reset();
}

//----------------------------------------------------------------------------
void Sample8::RandomizeBallVelocity(btRigidBody* pBody)
{
	Float velZ = mRandom.GetFloat() + 0.5F;
	Float velY = (mRandom.GetFloat() - 0.5F) * 20.0F;
	btTransform trans;
	pBody->getMotionState()->getWorldTransform(trans);
	if (trans.getOrigin().z() > 0)
	{
		pBody->setLinearVelocity(btVector3(0, velY, -50 * velZ));
	}
	else
	{
		pBody->setLinearVelocity(btVector3(0, velY, 50 * velZ));
	}
}

//----------------------------------------------------------------------------
void Sample8::UpdatePhysicsWorld(btScalar elapsedTime)
{
	// fixed 1/60 timestep
	mpDynamicsWorld->stepSimulation(elapsedTime, 10);

	Matrix3F mat;
	const btCollisionObjectArray& objectArray = mpDynamicsWorld->
		getCollisionObjectArray();

	for (Int i = 0; i < mpDynamicsWorld->getNumCollisionObjects(); i++)
	{
		btRigidBody* pBody = btRigidBody::upcast(objectArray[i]);
		if (pBody && pBody->getMotionState())
		{
			btTransform trans;
			pBody->getMotionState()->getWorldTransform(trans);
			Vector3F pos = Convert(trans.getOrigin());
			QuaternionF quat = Convert(trans.getRotation());
			quat.ToRotationMatrix(mat);
			mGameObjects[i].WorldTransformation.SetTranslate(pos);
			mGameObjects[i].WorldTransformation.SetRotate(mat);
		}
	}
}

//----------------------------------------------------------------------------
void Sample8::DestroyPhysicsWorld()
{
	//cleanup in the reverse order of creation/initialization

	//remove the rigidbodies from the dynamics world and delete them
	for (Int i = mpDynamicsWorld->getNumCollisionObjects()-1; i >= 0; i--)
	{
		btCollisionObject* pObj = mpDynamicsWorld->
			getCollisionObjectArray()[i];
		btRigidBody* pBody = btRigidBody::upcast(pObj);
		if (pBody && pBody->getMotionState())
		{
			WIRE_DELETE pBody->getMotionState();
		}

		mpDynamicsWorld->removeCollisionObject(pObj);
		WIRE_DELETE pObj;
	}

	//delete collision shapes
	for (UInt i = 0; i < mGameObjects.GetQuantity(); i++)
	{
		btCollisionShape* pShape = mGameObjects[i].pCollisionObject;
		WIRE_DELETE pShape;
	}

	WIRE_DELETE mpDynamicsWorld;
	WIRE_DELETE mpSolver;
	WIRE_DELETE mpOverlappingPairCache;
	WIRE_DELETE mpDispatcher;
	WIRE_DELETE mpCollisionConfiguration;
}
