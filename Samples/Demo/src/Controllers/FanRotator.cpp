#include "FanRotator.h"

#include "WireSpatial.h"

using namespace Wire;

//----------------------------------------------------------------------------
FanRotator::FanRotator(Float speed)
	:
	mSpeed(speed)
{
}

//----------------------------------------------------------------------------
Bool FanRotator::Update(Double appTime)
{
	Spatial* pSpatial = DynamicCast<Spatial>(mpSceneObject);
	
	if (!pSpatial || (appTime == -MathD::MAX_REAL))
	{
		return false;
	}

	Double angle = MathD::FMod(appTime, 1) * MathD::TWO_PI;
	Matrix3F rotate(Vector3F::UNIT_Y, static_cast<Float>(angle));
	pSpatial->Local.SetRotate(rotate);

	return true;
}
