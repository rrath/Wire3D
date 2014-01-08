// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETPODARRAY_H
#define WIRETPODARRAY_H

#include "WireSystem.h"

namespace Wire
{

// Same as TArray, but must only be used with POD types.
// A POD type is defined as either a scalar type or a POD class.
// A POD class has no user-defined copy assignment operator,
// no user-defined destructor, and no non-static data members
// that are not themselves PODs. Moreover, a POD class must be an aggregate,
// meaning it has no user-declared constructors, no private nor protected
// non-static data, no base classes and no virtual functions.

template <class T>
class TPODArray
{
public:
	// construction and destruction
	TPODArray(UInt maxQuantity = 0, UInt growBy = 1);
	TPODArray(const TPODArray& rObject);
	~TPODArray();

	// assignment
	TPODArray& operator= (const TPODArray& rObject);

	// element access, index i must be in range
	inline UInt GetQuantity() const;
	inline T* GetArray();
	inline const T* GetArray() const;
	inline T& operator[] (UInt i);
	inline const T& operator[] (UInt i) const;

	// add new element, array will dynamically grow if necessary
	void Append(const T& rElement);
	void Insert(UInt i, const T& rElement);
	void SetElement(UInt i, const T& rElement);

	// linear search
	Bool Contains(const T& rElement);
	// returns index of rElement if found, otherwise Quantity
	UInt Find(const T& rElement);

	// Remove the element or at the specified index. The elements occurring
	// after that one are shifted so that the array remains contiguous. After
	// the shift, but before the decrement on quantity, array[quantity-1] is
	// a duplicate of array[quantity-2]. The element at index quantity-1 is
	// reconstructed using the default constructor for class T. Then the
	// quantity is decremented.

	// Remove the element. Return value indicates if the operation succeeded.
	Bool Remove(const T& rElement);

	// Remove the element at the specified index. 
	void RemoveAt(UInt i);

	// Remove the last element of the array.
	inline void RemoveLast();

	// All elements are set to the default object of class T.
	inline void RemoveAll();

	// dynamic growth, new array elements are default constructed
	void SetMaxQuantity(UInt newMaxQuantity, Bool copy = true);
	inline UInt GetMaxQuantity() const;
	inline void SetGrowBy(UInt growBy);
	inline UInt GetGrowBy() const;

	// set the quantity without con-/destructing array elements
	void SetQuantity(UInt newQuantity, Bool copy = true);

private:
	UInt mQuantity;
	UInt mMaxQuantity;
	UInt mGrowBy;
	T* mpArray;
};

#include "WireTPODArray.inl"

}

#endif
