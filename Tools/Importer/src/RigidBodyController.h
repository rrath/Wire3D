#pragma once
#ifndef RIGIDBODYCONTROLLER_H
#define RIGIDBODYCONTROLLER_H

#include "CollisionObjectController.h"

class PhysicsWorld;

class RigidBodyController : public CollisionObjectController
{
	WIRE_DECLARE_RTTI;

public:
	RigidBodyController(PhysicsWorld* pPhysicsWorld, btRigidBody* pRigidBody);
	virtual ~RigidBodyController();

	// geometric update called by UpdateGS()
	virtual Bool Update(Double appTime);

	// add/remove from the physics world
	virtual Bool SetEnabled(Bool isEnabled);

	inline btRigidBody* Get() { return (btRigidBody*)mpCollisionObject; }
};

typedef Wire::Pointer<RigidBodyController> RigidBodyControllerPtr;

#endif
