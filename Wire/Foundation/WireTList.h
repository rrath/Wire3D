// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETLIST_H
#define WIRETLIST_H

#include "WireTypes.h"

// The class T is either native data or is class data that has the following
// member functions:
//   T::T()
//   T::T (const T&);
//   T& T::operator= (const T&)

namespace Wire
{

template <class T>
class TList
{
public:
    // construction
    TList();
    TList(const T& rItem, TList* pNext);
    ~TList();

    // member access
    void SetItem(const T& rItem);
    T& Item();
    const T& GetItem() const;
    void SetNext(TList* pNext);
    TList*& Next();
    const TList* GetNext() const;

    // total number of nodes in the list, including this one
    UInt GetQuantity() const;

private:
    T mItem;
    TList* mpNext;
};

#include "WireTList.inl"

}

#endif
