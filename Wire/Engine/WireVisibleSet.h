// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREVISIBLESET_H
#define WIREVISIBLESET_H

#include "WireObject.h"
#include "WireTPODArray.h"
#include "WireTransformation.h"

namespace Wire
{

class VisibleSet
{

public:
	enum
	{
		VS_DEFAULT_MAX_QUANTITY = 32,
		VS_DEFAULT_GROWBY = 32,
	};

	VisibleSet(UInt maxQuantity = VS_DEFAULT_MAX_QUANTITY, UInt growBy =
		VS_DEFAULT_GROWBY);
	virtual ~VisibleSet();

	inline UInt GetQuantity() const;
	inline void SetMaxQuantity(UInt maxQuantity);

	void GetSet(Object**& rObjectArrayPtr, Transformation**&
		rTransformationPtr);

	inline void Insert(Object* pObject, Transformation* pTransformation,
		UInt64 key = 0);

	void Clear();
	void Sort(Bool unwrap = false);

	static void QuickSort(TPODArray<UInt64>& rKeys, Object** const pVisible,
		//	static void QuickSort(UInt* const pKeys, Object** const pVisible,
		Transformation** const pTrafo, Int left, Int right);

private:
	void UnwrapEffectStackAndSort();
	void Sort(Int left, Int right);
	void SortUnwrapped(Int left, Int right);
	inline void InsertUnwrapped(Object* pObject, Transformation*
		pTransformation, UInt64 key = 0);

	TPODArray<Object*> mVisible;
	TPODArray<Transformation*> mTransformations;
	TPODArray<UInt64> mKeys;

	TPODArray<Object*> mVisibleUnwrapped;
	TPODArray<Transformation*> mTransformationsUnwrapped;
	TPODArray<UInt64> mKeysUnwrapped;
	Bool mIsUnwrapped;
};

#include "WireVisibleSet.inl"

}

#endif
