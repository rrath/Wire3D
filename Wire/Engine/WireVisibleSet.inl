// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt VisibleSet::GetQuantity() const
{
	return mIsUnwrapped ? mVisibleUnwrapped.GetQuantity() : mVisible.
		GetQuantity();
}

//----------------------------------------------------------------------------
inline void VisibleSet::SetMaxQuantity(UInt maxQuantity)
{
	mVisible.SetMaxQuantity(maxQuantity);
	mTransformations.SetMaxQuantity(maxQuantity);
	mKeys.SetMaxQuantity(maxQuantity);
}

//----------------------------------------------------------------------------
inline void VisibleSet::Insert(Object* pObject, Transformation*
	pTransformation, UInt64 key)
{
	mVisible.Append(pObject);
	mTransformations.Append(pTransformation);
	mKeys.Append(key);
}

//----------------------------------------------------------------------------
inline void VisibleSet::InsertUnwrapped(Object* pObject, Transformation*
	pTransformation, UInt64 key)
{
	mVisibleUnwrapped.Append(pObject);
	mTransformationsUnwrapped.Append(pTransformation);
	mKeysUnwrapped.Append(key);
}
