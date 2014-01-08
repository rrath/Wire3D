// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class TKEY>
THashSet<TKEY>::THashSet(UInt tableSize)
	:
	UserHashFunction(NULL),
	mTableSize(tableSize),
	mQuantity(0)
{
    mpTable = WIRE_NEW HashItem*[mTableSize];
	WIRE_ASSERT(mpTable);
	System::Memset(mpTable, 0, mTableSize*sizeof(HashItem*));
}

//----------------------------------------------------------------------------
template <class TKEY>
THashSet<TKEY>::~THashSet()
{
    RemoveAll();
    WIRE_DELETE[] mpTable;
}

//----------------------------------------------------------------------------
template <class TKEY>
inline UInt THashSet<TKEY>::GetQuantity() const
{
    return mQuantity;
}

//----------------------------------------------------------------------------
template <class TKEY>
TKEY* THashSet<TKEY>::Insert(const TKEY& rKey)
{
    // find hash table entry for given key
    UInt index = HashFunction(rKey);
    HashItem* pItem = mpTable[index];

    // search for item in list associated with key
    while (pItem)
    {
        if (rKey == pItem->mKey)
        {
            // item already in hash table
            return &pItem->mKey;
        }

        pItem = pItem->mpNext;
    }

    // add item to beginning of list
    pItem = WIRE_NEW HashItem;
    pItem->mKey = rKey;
    pItem->mpNext = mpTable[index];
    mpTable[index] = pItem;
    mQuantity++;

    return &pItem->mKey;
}

//----------------------------------------------------------------------------
template <class TKEY>
TKEY* THashSet<TKEY>::Get(const TKEY& rtKey) const
{
    // find hash table entry for given key
    UInt index = HashFunction(rtKey);
    HashItem* pItem = mpTable[index];

    // search for item in list associated with key
    while (pItem)
    {
        if (rtKey == pItem->mKey)
        {
            // item is in hash table
            return &pItem->mKey;
        }

        pItem = pItem->mpNext;
    }

    return NULL;
}

//----------------------------------------------------------------------------
template <class TKEY>
inline Bool THashSet<TKEY>::Contains(const TKEY& rKey) const
{
	return Get(rKey) != NULL;
}

//----------------------------------------------------------------------------
template <class TKEY>
Bool THashSet<TKEY>::Remove(const TKEY& rKey)
{
    // find hash table entry for given key
    UInt index = HashFunction(rKey);
    HashItem* pItem = mpTable[index];

    if (!pItem)
    {
        return false;
    }

    if (rKey == pItem->mKey)
    {
        // item is at front of list, strip it off
        HashItem* pSave = pItem;
        mpTable[index] = pItem->mpNext;
        WIRE_DELETE pSave;
        mQuantity--;
        return true;
    }

    // search for item in list
    HashItem* pPrev = pItem;
    HashItem* pCurr = pItem->mpNext;
    while (pCurr && rKey != pCurr->mKey)
    {
        pPrev = pCurr;
        pCurr = pCurr->mpNext;
    }

    if (pCurr)
    {
        // found the item
        pPrev->mpNext = pCurr->mpNext;
        WIRE_DELETE pCurr;
        mQuantity--;
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
template <class TKEY>
void THashSet<TKEY>::RemoveAll()
{
    if (mQuantity > 0)
    {
        for (UInt index = 0; index < mTableSize; index++)
        {
            while (mpTable[index])
            {
                HashItem* pSave = mpTable[index];
                mpTable[index] = mpTable[index]->mpNext;
                WIRE_DELETE pSave;
                if (--mQuantity == 0)
                {
                    return;
                }
            }
        }
    }
}

//----------------------------------------------------------------------------
template <class TKEY>
UInt THashSet<TKEY>::HashFunction(const TKEY& rKey) const
{
    if (UserHashFunction)
    {
        return (*UserHashFunction)(rKey);
    }

	// default hash function
	static Double s_HashMultiplier = 0.5 * (MathD::Sqrt(5.0) - 1.0);
	UInt key = (UInt)(rKey);
	key %= mTableSize;
	Double fraction = MathD::FMod(s_HashMultiplier * key, 1.0);
	return static_cast<UInt>(MathD::Floor(mTableSize*fraction));
}

//----------------------------------------------------------------------------
template <class TKEY>
THashSet<TKEY>::Iterator::Iterator(const THashSet* pHashSet)
	:
	mpHashSet(pHashSet),
	mIndex(0),
	mpItem(NULL)
{
	WIRE_ASSERT(pHashSet);
}

//----------------------------------------------------------------------------
template <class TKEY>
TKEY* THashSet<TKEY>::Iterator::GetFirst() const
{
    if (mpHashSet->mQuantity > 0)
    {
        for (mIndex = 0; mIndex < mpHashSet->mTableSize; mIndex++)
        {
            if (mpHashSet->mpTable[mIndex])
            {
                mpItem = mpHashSet->mpTable[mIndex];
                return &mpItem->mKey;
            }
        }
    }

    return NULL;
}

//----------------------------------------------------------------------------
template <class TKEY>
TKEY* THashSet<TKEY>::Iterator::GetNext() const
{
    if (mpHashSet->mQuantity > 0)
    {
        mpItem = mpItem->mpNext;
        if (mpItem)
        {
            return &mpItem->mKey;
        }

        for (mIndex++; mIndex < mpHashSet->mTableSize; mIndex++)
        {
            if (mpHashSet->mpTable[mIndex])
            {
                mpItem = mpHashSet->mpTable[mIndex];
                return &mpItem->mKey;
            }
        }
    }

    return NULL;
}
