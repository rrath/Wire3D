#include "PhysicsController.h"

#include "PhysicsWorld.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(PhysicsController, Wire::Controller);

//----------------------------------------------------------------------------
PhysicsController::PhysicsController(PhysicsWorld* pPhysicsWorld)
	:
	mpPhysicsWorld(pPhysicsWorld),
	mIsEnabled(false)
{
	WIRE_ASSERT(mpPhysicsWorld);
}

//----------------------------------------------------------------------------
PhysicsController::~PhysicsController()
{
	if (mpPhysicsWorld && mIsEnabled)
	{
		mpPhysicsWorld->RemoveController(this);
		mIsEnabled = false;
	}
}

//----------------------------------------------------------------------------
Bool PhysicsController::SetEnabled(Bool isEnabled)
{
	if (mIsEnabled == isEnabled)
	{
		return false;
	}

	mIsEnabled = isEnabled;
	if (!mpPhysicsWorld)
	{
		return false;
	}

	if (mIsEnabled)
	{
		mpPhysicsWorld->AddController(this);
	}
	else
	{
		mpPhysicsWorld->RemoveController(this);
	}

	return true;
}
