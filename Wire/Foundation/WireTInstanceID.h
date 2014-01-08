// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETINSTANCEID_H
#define WIRETINSTANCEID_H

// TInstanceID assigns every instance of type T a unique identification
// number that is never bigger than the total number of objects of type T.
// Be careful when caching IDs, because just like pointers, the same ID might
// be reassigned to a different object when destroying and creating objects
// of the same type.

#include "WireTArray.h"

namespace Wire
{

template <class T>
class TInstanceID
{
public:
	TInstanceID();
	~TInstanceID();

	inline operator UInt () const;
	inline static UInt GetMaxID();

private:
	void SiftUp(UInt nodeIndex);
	void SiftDown(UInt nodeIndex);

	UInt mID;

	static TArray<UInt> s_FreeIDs;
	static UInt s_MaxID;
};

#include "WireTInstanceID.inl"

}

#endif
