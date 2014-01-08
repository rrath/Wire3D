// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline void Spatial::SetParent(Spatial* pParent)
{
	mpParent = pParent;
}

//----------------------------------------------------------------------------
inline Spatial* Spatial::GetParent()
{
	return mpParent;
}

//----------------------------------------------------------------------------
inline UInt Spatial::GetStateQuantity() const
{
	return mStates.GetQuantity();
}

//----------------------------------------------------------------------------
inline State* Spatial::GetState(UInt i) const
{
	WIRE_ASSERT(i < mStates.GetQuantity());
	return mStates[i];
}

//----------------------------------------------------------------------------
inline void Spatial::DetachAllStates()
{
	mStates.RemoveAll();
}

//----------------------------------------------------------------------------
inline UInt Spatial::GetLightQuantity() const
{
	return mLights.GetQuantity();
}

//----------------------------------------------------------------------------
inline Light* Spatial::GetLight(UInt i) const
{
	WIRE_ASSERT(i < mLights.GetQuantity());
	return mLights[i];
}

//----------------------------------------------------------------------------
inline void Spatial::DetachLight(Light* pLight)
{
	mLights.Remove(pLight);
}

//----------------------------------------------------------------------------
inline void Spatial::DetachAllLights()
{
	mLights.RemoveAll();
}

//----------------------------------------------------------------------------
template<class T>
T* Spatial::GetState() const
{
	// check if type of state already exists
	for (UInt i = 0; i < mStates.GetQuantity(); i++)
	{
		if (mStates[i]->IsExactly(T::TYPE))
		{
			// type of state exists, return it
			return StaticCast<T>(mStates[i]);
		}
	}

	return NULL;
}
