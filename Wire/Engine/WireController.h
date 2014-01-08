// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRECONTROLLER_H
#define WIRECONTROLLER_H

#include "WireObject.h"

namespace Wire
{

class Camera;
class SceneObject;

class Controller : public Object
{
	WIRE_DECLARE_RTTI;

public:
	// destruction (abstract base class)
	virtual ~Controller();

	// the controlled object
	inline SceneObject* GetSceneObject() const;

	// geometric update called by UpdateGS()
	virtual Bool Update(Double appTime);

	// update called by culler only when the controlled object is visible
	virtual Bool OnGetVisibleUpdate(const Camera* pCamera);

	// called when the controller is attached to a SceneObject
	inline virtual void OnAttach();

	// called when the controller is detached from a SceneObject
	inline virtual void OnDetach();

protected:
	// construction (abstract base class)
	Controller();

	// the controlled scene object
	friend class SceneObject;
	inline void SetSceneObject(SceneObject* pObject);

	// Regular pointer used for controlled object to avoid circular smart
	// pointers between controller and object.
	SceneObject* mpSceneObject;

	Double mLastApplicationTime;
};

typedef Pointer<Controller> ControllerPtr;
#include "WireController.inl"

}

#endif
