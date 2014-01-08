// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class T>
TPODArray<T>::TPODArray(UInt maxQuantity, UInt growBy)
{
	mQuantity = 0;
	mMaxQuantity = maxQuantity;
	mGrowBy = growBy;
	mpArray = (mMaxQuantity > 0 ? WIRE_NEW T[mMaxQuantity] : NULL);
}

//----------------------------------------------------------------------------
template <class T>
TPODArray<T>::TPODArray(const TPODArray& rObject)
{
	mpArray = NULL;
	*this = rObject;
}

//----------------------------------------------------------------------------
template <class T>
TPODArray<T>::~TPODArray()
{
	WIRE_DELETE[] mpArray;
}

//----------------------------------------------------------------------------
template <class T>
TPODArray<T>& TPODArray<T>::operator= (const TPODArray& rObject)
{
	mQuantity = rObject.mQuantity;
	mMaxQuantity = rObject.mMaxQuantity;
	mGrowBy = rObject.mGrowBy;

	WIRE_DELETE[] mpArray;

	if (mMaxQuantity > 0)
	{
		mpArray = WIRE_NEW T[mMaxQuantity];
		size_t size = sizeof(T) * mMaxQuantity;
		System::Memcpy(mpArray, size, rObject.mpArray, size);
	}
	else
	{
		mpArray = NULL;
	}

	return *this;
}

//----------------------------------------------------------------------------
template <class T>
inline UInt TPODArray<T>::GetQuantity() const
{
	return mQuantity;
}

//----------------------------------------------------------------------------
template <class T>
inline T* TPODArray<T>::GetArray()
{
	return mpArray;
}

//----------------------------------------------------------------------------
template <class T>
inline const T* TPODArray<T>::GetArray() const
{
	return mpArray;
}

//----------------------------------------------------------------------------
template <class T>
inline T& TPODArray<T>::operator[] (UInt i)
{
	WIRE_ASSERT(i < mQuantity && mpArray);
	if (i >= mQuantity)
	{
		i = mQuantity-1;
	}

	return mpArray[i];
}

//----------------------------------------------------------------------------
template <class T>
inline const T& TPODArray<T>::operator[] (UInt i) const
{
	WIRE_ASSERT(i < mQuantity && mpArray);
	if (i >= mQuantity)
	{
		i = mQuantity-1;
	}

	return mpArray[i];
}

//----------------------------------------------------------------------------
template <class T>
Bool TPODArray<T>::Contains(const T& rElement)
{
	for (UInt i = 0; i < mQuantity; i++)
	{
		if (mpArray[i] == rElement)
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
template <class T>
UInt TPODArray<T>::Find(const T& rElement)
{
	for (UInt i = 0; i < mQuantity; i++)
	{
		if (mpArray[i] == rElement)
		{
			return i;
		}
	}

	return mQuantity;
}

//----------------------------------------------------------------------------
template <class T>
void TPODArray<T>::RemoveAt(UInt i)
{
	WIRE_ASSERT(i < mQuantity);
	if (i >= mQuantity)
	{
		return;
	}

	size_t size = sizeof(T) * (mQuantity-(i+1));
	System::Memmove(mpArray+i, mpArray+i+1, size);

	mQuantity--;
}

//----------------------------------------------------------------------------
template <class T>
Bool TPODArray<T>::Remove(const T& rElement)
{
	for (UInt i = 0; i < mQuantity; i++)
	{
		if (mpArray[i] == rElement)
		{
			RemoveAt(i);
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
template <class T>
inline void TPODArray<T>::RemoveLast()
{
	RemoveAt(mQuantity-1);
}

//----------------------------------------------------------------------------
template <class T>
inline void TPODArray<T>::RemoveAll()
{
	mQuantity = 0;
}

//----------------------------------------------------------------------------
template <class T>
void TPODArray<T>::SetMaxQuantity(UInt newMaxQuantity, Bool copy)
{
	if (newMaxQuantity == 0)
	{
		WIRE_DELETE[] mpArray;
		mpArray = NULL;
		mQuantity = 0;
		mMaxQuantity = 0;
		return;
	}

	if (newMaxQuantity != mMaxQuantity)
	{
		T* pNewArray = WIRE_NEW T[newMaxQuantity];

		if (copy)
		{
			UInt copyQuantity;
			if (newMaxQuantity > mMaxQuantity)
			{
				copyQuantity = mMaxQuantity;
			}
			else
			{
				copyQuantity = newMaxQuantity;
			}

			size_t size = sizeof(T) * copyQuantity;
			System::Memcpy(pNewArray, size, mpArray, size);

			if (mQuantity > newMaxQuantity)
			{
				mQuantity = newMaxQuantity;
			}
		}
		else
		{
			mQuantity = 0;
		}

		WIRE_DELETE[] mpArray;
		mpArray = pNewArray;
		mMaxQuantity = newMaxQuantity;
	}
}

//----------------------------------------------------------------------------
template <class T>
inline UInt TPODArray<T>::GetMaxQuantity() const
{
	return mMaxQuantity;
}

//----------------------------------------------------------------------------
template <class T>
inline void TPODArray<T>::SetGrowBy(UInt growBy)
{
	mGrowBy = growBy;
}

//----------------------------------------------------------------------------
template <class T>
void TPODArray<T>::SetQuantity(UInt newQuantity, Bool copy)
{
	if (newQuantity > mMaxQuantity)
	{
		SetMaxQuantity(newQuantity, copy);
	}
	
	mQuantity = newQuantity;
}

//----------------------------------------------------------------------------
template <class T>
inline UInt TPODArray<T>::GetGrowBy() const
{
	return mGrowBy;
}

//----------------------------------------------------------------------------
template <class T>
void TPODArray<T>::Append(const T& rElement)
{
	if (++mQuantity > mMaxQuantity)
	{
		if (mGrowBy > 0)
		{
			// Increase the size of the array. In the event rElement is a
			// reference to something in the current array, the reallocation
			// in SetMaxQuantity will invalidate the reference. A copy of
			// rElement is made here to avoid the invalidation.
			T saveElement(rElement);
			SetMaxQuantity(mMaxQuantity + mGrowBy, true);
			mpArray[mQuantity-1] = saveElement;
			return;
		}

		// cannot grow the array, overwrite the last element
		WIRE_ASSERT(false);
		--mQuantity;
	}

	mpArray[mQuantity-1] = rElement;
}

//----------------------------------------------------------------------------
template <class T>
void TPODArray<T>::Insert(UInt i, const T& rElement)
{
	WIRE_ASSERT(i <= mQuantity);
	if (mQuantity == mMaxQuantity)
	{
		WIRE_ASSERT(mGrowBy > 0 /* cannot insert, not allowed to grow */);
		SetMaxQuantity(mMaxQuantity+mGrowBy, true);
	}

	SetQuantity(mQuantity+1);

 	size_t size = sizeof(T) * (mQuantity-(i+1));
 	System::Memmove(mpArray+i+1, mpArray+i, size);

	mpArray[i] = rElement;
}

//----------------------------------------------------------------------------
template <class T>
void TPODArray<T>::SetElement(UInt i, const T& rElement)
{
	if (i >= mQuantity)
	{
		if (i >= mMaxQuantity)
		{
			if (mGrowBy > 0)
			{
				// increase the size of the array
				if (i+1 >= mMaxQuantity)
				{
					UInt n = 1 + static_cast<Int>(0.5F + (i+1 - mMaxQuantity) /
						static_cast<Float>(mGrowBy));

					// Increase the size of the array. In the event rElement
					// is a reference to something in the current array, the
					// reallocation in SetMaxQuantity will invalidate the
					// reference. A copy of rElement is made here to avoid
					// the invalidation.
					T saveElement(rElement);
					SetMaxQuantity(mMaxQuantity + n * mGrowBy, true);
					mQuantity = i+1;
					mpArray[i] = saveElement;
					return;
				}
			}
			else
			{
				// cannot grow the array, overwrite the last element
				WIRE_ASSERT(false);
				i = mQuantity-1;
			}
		}

		mQuantity = i+1;
	}

	mpArray[i] = rElement;
}
