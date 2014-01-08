// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETSTACK_H
#define WIRETSTACK_H

// The class T is either native data or is class data that has the following
// member functions:
//   T::T ()
//   T::T (const T&);
//   T& T::operator= (const T&)

#include "WireTypes.h"

namespace Wire
{

template <class T>
class TStack
{
public:
	TStack(Int maxQuantity = 64);
	~TStack();

	Bool IsEmpty() const;
	Bool IsFull() const;
	void Push(const T& rItem);
	void Pop(T& rItem);
	void Pop();
	void Clear();
	Bool GetTop(T& rItem) const;
	T& GetTop() const;

	// for iteration over the stack elements
	inline Int GetQuantity() const;
	inline Int GetMaxQuantity() const;
	inline const T* GetData() const;

private:
    Int mMaxQuantity;
	Int mTop;
    T* mpStack;
};

#include "WireTStack.inl"

}

#endif
