// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRESMARTPOINTER_H
#define WIRESMARTPOINTER_H

#include "WireSystem.h"

namespace Wire
{

template <class T>
class Pointer
{
public:
    // construction and destruction
	Pointer(T* pObject = NULL);
	Pointer(const Pointer& rPointer);
	~Pointer();

    // implicit conversions
	inline operator T* () const;
	inline T& operator* () const;
	inline T* operator-> () const;

	inline T* Get() const;

    // assignment
	Pointer& operator= (T* pObject);
	Pointer& operator= (const Pointer& rReference);

    // comparisons
	inline Bool operator== (T* pObject) const;
	inline Bool operator!= (T* pObject) const;
	inline Bool operator== (const Pointer& rReference) const;
	inline Bool operator!= (const Pointer& rReference) const;

protected:
    // the shared object
    T* mpObject;
};

#include "WireSmartPointer.inl"

}

#endif
