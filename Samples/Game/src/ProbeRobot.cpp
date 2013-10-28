#include "ProbeRobot.h"

#include "PhysicsWorld.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(ProbeRobot, PhysicsController);

//----------------------------------------------------------------------------
ProbeRobot::ProbeRobot(PhysicsWorld* pPhysicsWorld, Spatial* pPlayerSpatial,
	Spatial* pHealthBar)
	:
	PhysicsController(pPhysicsWorld),
	mspPlayerSpatial(pPlayerSpatial),
	mspHealthBar(pHealthBar),
	mTotalHealth(100.0F),
	mHealth(100.0F),
	mSpeed(7.5F),
	mMaximumPlayerDistanceSquared(10*10)
{
	WIRE_ASSERT(mspPlayerSpatial);
	WIRE_ASSERT(mspHealthBar);

	mHealthBarScale = mspHealthBar->Local.GetScale();
	SetEnabled(true);
}

//----------------------------------------------------------------------------
ProbeRobot::~ProbeRobot()
{
}

//----------------------------------------------------------------------------
Bool ProbeRobot::Update(Double appTime)
{
	if (mHealth <= 0)
	{
		Die();
		return true;
	}

	CalculateMovementAndRotation();

	// update the red health bar
	Float healthRatio = mHealth / mTotalHealth;
	Vector3F healthBarScale = mHealthBarScale;
	healthBarScale.X() = MathF::Max(healthBarScale.X() * healthRatio, 0.0001F);
	mspHealthBar->Local.SetScale(healthBarScale);

	return true;
}

//----------------------------------------------------------------------------
void ProbeRobot::Die()
{
	Node* pNode = DynamicCast<Node>(GetSceneObject());
	if (!pNode)
	{
		return;
	}

	// probe robot is dead, disable character controller
	mspCharacter->SetEnabled(false);
	btTransform worldTrafo = mspCharacter->Get()->getWorldTransform();
	mspCharacter->Get()->setUserPointer(NULL);
	mspCharacter->GetCharacter()->setWalkDirection(btVector3(0, 0, 0));

	// cull left over energy bar
	for (UInt i = 0; i < pNode->GetQuantity(); i++)
	{
		if (pNode->GetChild(i))
		{
			pNode->GetChild(i)->Culling = Spatial::CULL_ALWAYS;
		}
	}

	// enable robot corpse, i.e. rigid body
	RigidBodyController* pRigidBodyController = pNode->GetController<
		RigidBodyController>();
	if (pRigidBodyController)
	{
		pRigidBodyController->Get()->setWorldTransform(worldTrafo);
		worldTrafo.setOrigin(btVector3(0, 0, 0));
		btVector3 velocity(-3, 0.7F, 0.3F);
		velocity = worldTrafo * velocity;	
		pRigidBodyController->Get()->setAngularVelocity(velocity);
		pRigidBodyController->SetEnabled(true);
	}

	// detach this controller, no more Updates() will be received.
	pNode->DetachController(this);
}

//----------------------------------------------------------------------------
void ProbeRobot::OnAttach()
{
	mspCharacter = GetSceneObject()->GetController<CharacterController>();
	WIRE_ASSERT(mspCharacter);

	// Add a reference to the controller in the physics object
	mspCharacter->Get()->setUserPointer(this);
}

//----------------------------------------------------------------------------
void ProbeRobot::TakeDamage(Float damage)
{
	mHealth -= damage;
}

//----------------------------------------------------------------------------
void ProbeRobot::CalculateMovementAndRotation()
{
	btVector3 origin = mspCharacter->Get()->getWorldTransform().getOrigin();
	const Vector3F probeRobotPosition(origin.x(), origin.y(), origin.z());

	Vector3F playerPosition = mspPlayerSpatial->Local.GetTranslate();
	Vector3F direction = playerPosition - probeRobotPosition;
	Vector3F distance = direction;
	distance.Y() = 0;
	direction.Normalize();

	Vector3F move = Vector3F::ZERO;
	Float distanceSquared = distance.SquaredLength();
	if (distanceSquared > mMaximumPlayerDistanceSquared)
	{
		distanceSquared -= mMaximumPlayerDistanceSquared;
		const Float range = 60;
		distanceSquared = MathF::Min(distanceSquared, range);
		Float speed = mSpeed * distanceSquared / range;
		if (mpPhysicsWorld)
		{
			move = direction * speed * static_cast<Float>(mpPhysicsWorld->
				GetFixedTimeStep());
		}
	}

	// update the scene object
	Spatial* pSpatial = DynamicCast<Spatial>(GetSceneObject());
	WIRE_ASSERT(pSpatial);
	distance.Normalize();

	Vector3F right = distance.Cross(Vector3F::UNIT_Y);
	right.Normalize();
	Matrix3F rotation(-right, Vector3F::UNIT_Y, distance, true);
	pSpatial->Local.SetRotate(rotation);
	pSpatial->Local.SetTranslate(probeRobotPosition);

	// update the physics object
	mspCharacter->GetCharacter()->setWalkDirection(PhysicsWorld::Convert(move));
	QuaternionF q(rotation);
	mspCharacter->Get()->getWorldTransform().setRotation(PhysicsWorld::Convert(q));
}
