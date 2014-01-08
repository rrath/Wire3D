// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
// smart pointers
//----------------------------------------------------------------------------
inline void Object::IncrementReferences()
{
	mReferences++;
}

//----------------------------------------------------------------------------
inline void Object::DecrementReferences()
{
	if (--mReferences == 0)
	{
		WIRE_DELETE this;
	}
}

//----------------------------------------------------------------------------
inline Int Object::GetReferences() const
{
	return mReferences;
}

//----------------------------------------------------------------------------
// run-time type information
//----------------------------------------------------------------------------
inline const Rtti& Object::GetType() const
{
	return TYPE;
}

//----------------------------------------------------------------------------
inline Bool Object::IsExactly(const Rtti& rType) const
{
	return GetType().IsExactly(rType);
}

//----------------------------------------------------------------------------
inline Bool Object::IsDerived(const Rtti& rType) const
{
	return GetType().IsDerived(rType);
}

//----------------------------------------------------------------------------
inline Bool Object::IsExactlyTypeOf(const Object* pObj) const
{
	return pObj && GetType().IsExactly(pObj->GetType());
}

//----------------------------------------------------------------------------
inline Bool Object::IsDerivedTypeOf(const Object* pObj) const
{
	return pObj && GetType().IsDerived(pObj->GetType());
}

//----------------------------------------------------------------------------
template <class T>
inline T* StaticCast(Object* pObj)
{
	return static_cast<T*>(pObj);
}

//----------------------------------------------------------------------------
template <class T>
inline const T* StaticCast(const Object* pObj)
{
	return static_cast<const T*>(pObj);
}

//----------------------------------------------------------------------------
template <class T>
T* DynamicCast(Object* pObj)
{
	return pObj && pObj->IsDerived(T::TYPE) ? static_cast<T*>(pObj) : 0;
}

//----------------------------------------------------------------------------
template <class T>
const T* DynamicCast(const Object* pObj)
{
	return pObj && pObj->IsDerived(T::TYPE) ? static_cast<const T*>(pObj) : 0;
}
