#include "Elevator.h"

#include "PhysicsWorld.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(Elevator, PhysicsController);

//----------------------------------------------------------------------------
Elevator::Elevator(PhysicsWorld* pPhysicsWorld)
	:
	PhysicsController(pPhysicsWorld),
	mTime(0)
{
	SetEnabled(true);
}

//----------------------------------------------------------------------------
Elevator::~Elevator()
{
}

//----------------------------------------------------------------------------
void Elevator::PhysicsUpdate(Double appTime)
{
	RigidBodyController* pRigidBodyController =	GetSceneObject()->
		GetController<RigidBodyController>();

	if (!pRigidBodyController || !pRigidBodyController->Get())
	{
		return;
	}

	btTransform trans;
	pRigidBodyController->Get()->getMotionState()->getWorldTransform(trans);
	btVector3 origin = trans.getOrigin();
	static Float yCoord = origin.y();
	Float angle = static_cast<Float>(MathD::Sin(mTime) * 6 + 6);
	origin[1] = yCoord + angle;
	trans.setOrigin(origin);
	pRigidBodyController->Get()->getMotionState()->setWorldTransform(trans);

	mTime += appTime;
}
