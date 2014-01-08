// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETHASHSET_H
#define WIRETHASHSET_H

// The class TKEY is either native data or is class data that has the
// following member functions:
//   TKEY::TKEY ()
//   TKEY& TKEY::operator= (const TKEY&)
//   Bool TKEY::operator== (const TKEY&) const
//   Bool TKEY::operator!= (const TKEY&) const
//   TKEY::operator UInt () const
// The implicit conversion to UInt is used to select a hash table
// index for the T object. The return value need not be within the range of
// hash table indices. THashSet will use modular arithmetic to make this
// happen.

#include "WireTypes.h"
#include "WireMath.h"
#include "WireMemory.h"

namespace Wire
{

template <class TKEY>
class THashSet
{
public:
	// construction and destruction
	THashSet(UInt tableSize = 128);
	~THashSet ();

	// element access
	inline UInt GetQuantity() const;

	// A pointer to the actual storage is returned so that the caller has
	// direct access to it. This allows a subset of TKEY members to be used
	// in key comparison.
	TKEY* Insert(const TKEY& rKey);

	// If the input key exists, a pointer to the actual storage is returned.
	// This allows a subset of TKEY members to be used in key comparison,
	// but gives the caller a chance to modify other TKEY members.
	TKEY* Get(const TKEY& rKey) const;
	inline Bool Contains(const TKEY& rKey) const;

	Bool Remove(const TKEY& rKey);
	void RemoveAll();

	// user-specified key-to-index construction
	UInt (*UserHashFunction)(const TKEY&);

	struct HashItem
	{
		TKEY mKey;
		HashItem* mpNext;
	};

	// linear traversal of table
	class Iterator
	{
	public:
		Iterator(const THashSet* pHashSet);

		TKEY* GetFirst() const;
		TKEY* GetNext() const;

	private:
		const THashSet* mpHashSet;
		mutable UInt mIndex;
		mutable HashItem* mpItem;
	};

private:
	// Default key-to-index construction (override by user-specified when
	// requested).
	UInt HashFunction(const TKEY& rKey) const;

	// hash table
	UInt mTableSize;
	UInt mQuantity;
	HashItem** mpTable;
};

#include "WireTHashSet.inl"

}

#endif
