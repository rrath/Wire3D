// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class T>
TQueue<T>::TQueue(UInt maxQuantity)
	:
	mMaxQuantity(maxQuantity),
	mBack(0),
	mFront(0),
	mIsEmpty(true)
{
    mpQueue = WIRE_NEW T[mMaxQuantity];
}

//----------------------------------------------------------------------------
template <class T>
TQueue<T>::~TQueue()
{
    WIRE_DELETE[] mpQueue;
}

//----------------------------------------------------------------------------
template <class T>
inline Bool TQueue<T>::IsEmpty() const
{
    return mIsEmpty;
}

//----------------------------------------------------------------------------
template <class T>
inline Bool TQueue<T>::IsFull() const
{
	if (!mIsEmpty)
	{
		return (mBack+1) == mFront;
	}

	return false;
}

//----------------------------------------------------------------------------
template <class T>
void TQueue<T>::PushBack(const T& rItem)
{
	if (!IsFull())
	{
		mpQueue[mBack++] = rItem;
		mIsEmpty = false;

		if (mBack >= mMaxQuantity)
		{
			mBack = mBack % mMaxQuantity;
		}
	}
	else
	{
		// queue is full, use bigger mMaxQuantity
		WIRE_ASSERT(false);
	}
}

//----------------------------------------------------------------------------
template <class T>
void TQueue<T>::PopFront(T& rItem)
{
	if (!mIsEmpty)
	{
		rItem = mpQueue[mFront++];

		if (mFront >= mMaxQuantity)
		{
			mFront = mFront % mMaxQuantity;
		}

		if (mFront == mBack)
		{
			mIsEmpty = true;
		}
	}
	else
	{
		// queue is empty, more Pops then Pushes
		WIRE_ASSERT(false);
	}
}

//----------------------------------------------------------------------------
template <class T>
T& TQueue<T>::PopFront()
{
	if (!mIsEmpty)
	{
		T& rItem = mpQueue[mFront++];

		if (mFront >= mMaxQuantity)
		{
			mFront = mFront % mMaxQuantity;
		}

		if (mFront == mBack)
		{
			mIsEmpty = true;
		}

		return rItem;
	}
	else
	{
		// queue is empty, more Pops then Pushes
		WIRE_ASSERT(false);
		return mpQueue[mFront];
	}
}

//----------------------------------------------------------------------------
template <class T>
void TQueue<T>::Clear()
{
    mBack = 0;
	mFront = 0;
	mIsEmpty = true;
}

//----------------------------------------------------------------------------
template <class T>
inline Bool TQueue<T>::GetFront(T& rItem) const
{
	if (!mIsEmpty)
    {
        rItem = mpQueue[mFront];
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
template <class T>
inline T& TQueue<T>::GetFront() const
{
    WIRE_ASSERT(!mIsEmpty);
    return mpQueue[mFront];
}
