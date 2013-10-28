#pragma once
#ifndef PHYSICSCONTROLLER_H
#define PHYSICSCONTROLLER_H

#include "WireController.h"

class PhysicsWorld;

class PhysicsController : public Wire::Controller
{
	WIRE_DECLARE_RTTI;

public:
	PhysicsController(PhysicsWorld* pPhysicsWorld);
	virtual ~PhysicsController();

 	// update called by PhysicsWorld::StepSimulation()
 	virtual void PhysicsUpdate(Double appTime) {}

	// can be used to draw debug information
	virtual void ToggleDebugShape(Bool show = true, Bool destroyOnHide = false) {}

	// add/remove from the physics world
	virtual Bool SetEnabled(Bool isEnabled);

	Bool IsEnabled() const { return mIsEnabled; }

protected:
	friend class PhysicsWorld;
	PhysicsWorld* mpPhysicsWorld;
	Bool mIsEnabled;
};

typedef Wire::Pointer<PhysicsController> PhysicsControllerPtr;

#endif
