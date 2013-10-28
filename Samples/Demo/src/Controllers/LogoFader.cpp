#include "LogoFader.h"

#include "WireSpatial.h"
#include "WireStateMaterial.h"

using namespace Wire;

const Float LogoFader::s_FadeTime = 1.5F;
const Float LogoFader::s_BeforeOutTime = 2.5F;
const Float LogoFader::s_BeforeInTime = 21.0F;

//----------------------------------------------------------------------------
LogoFader::LogoFader()
	:
	mBeforeOutTime(s_BeforeOutTime),
	mBeforeInTime(30.0F),
	mIsFadeOut(true)
{
}

//----------------------------------------------------------------------------
Bool LogoFader::Update(Double appTime)
{
	Spatial* pSpatial = DynamicCast<Spatial>(mpSceneObject);

	if (!pSpatial || (appTime == -MathD::MAX_REAL))
	{
		return false;
	}

	StateMaterial* pMaterialState = DynamicCast<StateMaterial>(pSpatial->
		GetState(State::MATERIAL));
	if (!pMaterialState)
	{
		return false;
	}

	Float elapsedTime = static_cast<Float>(appTime - mLastApplicationTime);
	if (mLastApplicationTime == -MathD::MAX_REAL)
	{
		elapsedTime = 0.0F;
	}

	mLastApplicationTime = appTime;

	if (mIsFadeOut)
	{
		if (mBeforeOutTime > 0.0F)
		{
			mBeforeOutTime -= elapsedTime;
		}
		else
		{
			if (pMaterialState->Ambient.A() > 0)
			{
				pSpatial->Culling = Spatial::CULL_DYNAMIC;
				pMaterialState->Ambient.A() -= elapsedTime * (1/s_FadeTime);
			}
			else
			{
				pSpatial->Culling = Spatial::CULL_ALWAYS;
				mIsFadeOut = !mIsFadeOut;
				pMaterialState->Ambient.A() = 0.0F;
				mBeforeOutTime = s_BeforeOutTime;
			}
		}
	}
	else
	{
		if (mBeforeInTime > 0.0F)
		{
			mBeforeInTime -= elapsedTime;
		}
		else
		{
			if (pMaterialState->Ambient.A() < 1.0F)
			{
				pSpatial->Culling = Spatial::CULL_DYNAMIC;
				pMaterialState->Ambient.A() += elapsedTime * (1/s_FadeTime);
			}
			else
			{
				mIsFadeOut = !mIsFadeOut;
				pMaterialState->Ambient.A() = 1.0F;
				mBeforeInTime = s_BeforeInTime;
			}
		}
	}

	return true;
}
