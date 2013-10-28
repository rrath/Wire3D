#include "RigidBodyController.h"

#include "PhysicsWorld.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(RigidBodyController, CollisionObjectController);

//----------------------------------------------------------------------------
RigidBodyController::RigidBodyController(PhysicsWorld* pPhysicsWorld,
	btRigidBody* pRigidBody)
	:
	CollisionObjectController(pPhysicsWorld, pRigidBody)
{
}

//----------------------------------------------------------------------------
RigidBodyController::~RigidBodyController()
{
	WIRE_DELETE Get()->getMotionState();
}

//----------------------------------------------------------------------------
Bool RigidBodyController::Update(Double appTime)
{
	if (!mIsEnabled)
	{
		return false;
	}

	WIRE_ASSERT(DynamicCast<Spatial>(mpSceneObject));
	if (!mpCollisionObject || !Controller::Update(appTime))
	{
		return false;
	}

	const Bool isStatic = mpCollisionObject->isStaticObject();
	if (!isStatic || mspDebugShape)
	{
		btTransform trans = mpCollisionObject->getWorldTransform();
		Vector3F pos = PhysicsWorld::Convert(trans.getOrigin());
		QuaternionF quat = PhysicsWorld::Convert(trans.getRotation());
		Matrix3F mat;
		quat.ToRotationMatrix(mat);

		if (!isStatic)
		{
			WIRE_ASSERT(DynamicCast<Spatial>(mpSceneObject));
			Spatial* pSpatial = StaticCast<Spatial>(mpSceneObject);
			pSpatial->World.SetTranslate(pos);
			pSpatial->World.SetRotate(mat);
			pSpatial->WorldIsCurrent = true;
		}

		if (mspDebugShape)
		{
			mspDebugShape->World.SetTranslate(pos);
			mspDebugShape->World.SetRotate(mat);
			mspDebugShape->WorldIsCurrent = true;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
Bool RigidBodyController::SetEnabled(Bool isEnabled)
{
	if (CollisionObjectController::SetEnabled(isEnabled))
	{
		if (isEnabled)
		{
			mpPhysicsWorld->Get()->addRigidBody(Get());
		}

		return true;
	}

	return false;
}
