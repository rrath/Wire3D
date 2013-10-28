#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "PhysicsController.h"

class PhysicsWorld;

class Elevator : public PhysicsController
{
	WIRE_DECLARE_RTTI;

public:
	Elevator(PhysicsWorld* pPhysicsWorld);
	virtual ~Elevator();

 	virtual void PhysicsUpdate(Double appTime);

private:
	Double mTime;
};

#endif
