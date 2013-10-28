#ifndef PROBEROBOT_H
#define PROBEROBOT_H

#include "PhysicsController.h"
#include "CharacterController.h"
#include "WireSpatial.h"

class PhysicsWorld;

class ProbeRobot : public PhysicsController
{
	WIRE_DECLARE_RTTI;

public:
	ProbeRobot(PhysicsWorld* pPhysicsWorld, Wire::Spatial* pPlayerSpatial,
		Wire::Spatial* pHealthBar);
	virtual ~ProbeRobot();

	virtual Bool Update(Double appTime);
	virtual void OnAttach();
	void TakeDamage(Float damage);

private:
	void CalculateMovementAndRotation();
	void Die();

	Wire::SpatialPtr mspPlayerSpatial;
	Wire::SpatialPtr mspHealthBar;
	Wire::Vector3F mHealthBarScale;

	Float mTotalHealth;
	Float mHealth;
	Float mSpeed;
	Float mMaximumPlayerDistanceSquared;

	CharacterControllerPtr mspCharacter;
};

#endif
