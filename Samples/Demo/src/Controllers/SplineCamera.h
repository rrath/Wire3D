#pragma once
#ifndef SPLINECAMERA_H
#define SPLINECAMERA_H

#include "WireCamera.h"
#include "WireController.h"
#include "WireNode.h"
#include "WireQuaternion.h"

class SplineCamera : public Wire::Controller
{
public:
	SplineCamera(Wire::Node* pSplineRoot, Wire::Camera* pCamera);

	virtual Bool Update(Double appTime);

private:
	Wire::Vector3F GetHermite(Wire::TArray<Wire::Transformation*>& rControlPoints,
		UInt idx, Float t);
	Wire::QuaternionF GetSquad(Wire::TArray<Wire::Transformation*>& rControlPoints,
		UInt idx, Float t);

	Wire::TArray<Wire::Transformation*> mSplinePoints;
	Wire::CameraPtr mspCamera;
	Float mT;
	UInt mSplinePointIndex;
};

#endif
