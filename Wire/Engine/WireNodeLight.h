// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENODELIGHT_H
#define WIRENODELIGHT_H

#include "WireNode.h"

namespace Wire
{

class Light;

class NodeLight : public Node
{
	WIRE_DECLARE_RTTI;

public:
	// The node's world translation is used as the light's position. Column 0
	// of the node's world rotation matrix is used as the light's direction.
	// The light's transformation is only updated if its Enabled flag is true.
	//
	// On construction, the node's local transformation is set to the
	// light's current coordinate system.
	//   local translation       = light location
	//   local rotation column 0 = light direction
	//   local rotation column 1+2 are generated as an orthonormal basis from
	//   the light's direction
	NodeLight(Light* pLight = NULL);
	virtual ~NodeLight();

	// When you set the light, the node's local transformation is set to the
	// light's current coordinate system as described above. When the node's
	// world transformation is computed, the light's position and direction
	// is set to use the node's world transformation.
	void Set(Light* pLight);
	inline Light* Get();
	inline const Light* Get() const;

protected:
	// geometric updates
	virtual void UpdateWorldData(Double appTime, Bool updateControllers);

private:
	void LightToLocalTransform();

	Pointer<Light> mspLight;
};

typedef Pointer<NodeLight> NodeLightPtr;
#include "WireNodeLight.inl"

}

#endif
