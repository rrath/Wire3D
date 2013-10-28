#pragma once
#ifndef SAMPLE10_H
#define SAMPLE10_H

#include "WireApplication.h"

namespace Wire
{

class Sample10 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	Sample10();
	virtual Bool OnInitialize();
	virtual void OnIdle();

private:
	// There are 2 scenarios that consist of cubes and transparent cylinders.
	// They use different materials and are placed in the scene graph/an array
	// in alternating order to create a worst-case scenario (the renderer has
	// to switch materials and render states in every draw call (unless
	// sorting is used to alter the order of objects being rendered))

	// Scenario 1 sorts and draws an array of 188 objects.
	// draw call count is relatively low, overdraw is high
	void CreateScenario1();
	TArray<RenderObjectPtr> mScenario1Objects;
	TArray<Transformation> mTransformations;
	SphereBVPtr mspBound;
	VisibleSet mVisibleSet;

	// Scenario 2 uses a scene graph of more than 2500 objects.
	// draw call count is high, overdraw is low
	void CreateScenario2();
	NodePtr mspScenario2;

	// Resources to be used be all scenarios
	void CreateCube();
	RenderObjectPtr mspCube;

	RenderObject* CreateCylinder() const;

	void CreateCylinderFront();
	RenderObjectPtr mspCylinderFront;

	void CreateCylinderBack();
	RenderObjectPtr mspCylinderBack;

	Node* CreateNodeCylinder();
	Node* CreateNodeCube();

	LightPtr mspLight;
	CameraPtr mspCamera;
	Culler mCuller;
	CullerSorting mCullerSorting;

	// frames per second text
	void DrawFPS(Double elapsed, Bool usesSorting);
	CameraPtr mspTextCamera;
	RenderTextPtr mspText;
	Double mLastTime;
};

WIRE_REGISTER_INITIALIZE(Sample10);

}

#endif
