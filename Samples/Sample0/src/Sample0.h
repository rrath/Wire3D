#pragma once
#ifndef SAMPLE0_H
#define SAMPLE0_H

#include "WireApplication.h"

namespace Wire
{

// User applications are derived from WIREAPPLICATION, which handles
// platform dependent setup and provides virtual functions for the user
// (i.e. you) to override. See WireApplication.h for details.
class Sample0 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	virtual Bool OnPrecreate();
	virtual Bool OnInitialize();
	virtual void OnTerminate();
	virtual void OnIdle();

private:
	// ObjectPtr is a smart pointer. Every object derived from Wire::Object
	// has a reference count, which the smart pointer operates on to
	// determine when an object is no longer referenced. Once the reference
	// count is zero, the object is deleted automatically. Therefore NEVER
	// delete a smart pointer referenced object manually.
	//
	// The following coding style is used throughout the framework:
	// 'pObject' = pointer to Object
	// 'rObject' = reference to Object
	// 'spObject' = smart pointer to Object
	// 'mObject' = Object is a member variable of the class
	// 'mpObject' = member pointer to Object
	// 'mspObject' = member smart pointer to Object

	RenderObjectPtr mspCube;
	CameraPtr mspCamera;
};

WIRE_REGISTER_INITIALIZE(Sample0);

}

#endif
