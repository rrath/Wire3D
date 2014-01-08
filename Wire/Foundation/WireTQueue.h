// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETQUEUE_H
#define WIRETQUEUE_H

// The class T is either native data or is class data that has the following
// member functions:
//   T::T ()
//   T::T (const T&);
//   T& T::operator= (const T&)

#include "WireSystem.h"

namespace Wire
{

template <class T>
class TQueue
{
public:
    TQueue(UInt maxQuantity = 64);
    ~TQueue();

    inline Bool IsEmpty() const;
    inline Bool IsFull() const;
    void PushBack(const T& rItem);
    void PopFront(T& rItem);
	T& PopFront();
    void Clear();
    inline Bool GetFront(T& rItem) const;
	inline T& GetFront() const;

private:
    UInt mMaxQuantity;
	UInt mBack;
	UInt mFront;
    T* mpQueue;
	Bool mIsEmpty;
};

#include "WireTQueue.inl"

}

#endif
