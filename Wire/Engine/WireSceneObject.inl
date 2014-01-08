// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt SceneObject::GetControllerQuantity() const
{
	return mControllers.GetQuantity();
}

//----------------------------------------------------------------------------
inline Controller* SceneObject::GetController(UInt i) const
{
	WIRE_ASSERT(i < mControllers.GetQuantity());
	return mControllers[i];
}

//----------------------------------------------------------------------------
inline void SceneObject::SetName(const String& rName)
{
	mName = rName;
}

//----------------------------------------------------------------------------
inline const String& SceneObject::GetName() const
{
	return mName;
}

//----------------------------------------------------------------------------
template <class T>
inline T* SceneObject::GetController(Bool getDerivedTypes) const
{
	return StaticCast<T>(GetController(T::TYPE, getDerivedTypes));
}

//----------------------------------------------------------------------------
template <class T>
void SceneObject::GetControllers(TArray<T*>& rControllers,
	Bool getDerivedTypes) const
{
	WIRE_ASSERT(T::TYPE.IsDerived(Controller::TYPE));

	for (UInt i = 0; i < mControllers.GetQuantity(); i++)
	{
		if (getDerivedTypes)
		{
			if (mControllers[i]->IsDerived(T::TYPE))
			{
				rControllers.Append(StaticCast<T>(mControllers[i]));
			}
		}
		else if (mControllers[i]->IsExactly(T::TYPE))
		{
			rControllers.Append(StaticCast<T>(mControllers[i]));
		}
	}
}
