// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class T>
TStack<T>::TStack(Int maxQuantity)
	:
	mMaxQuantity(maxQuantity),
	mTop(-1)
{
    WIRE_ASSERT(mMaxQuantity > 0);
    mpStack = WIRE_NEW T[mMaxQuantity];
}

//----------------------------------------------------------------------------
template <class T>
TStack<T>::~TStack()
{
    WIRE_DELETE[] mpStack;
}

//----------------------------------------------------------------------------
template <class T>
Bool TStack<T>::IsEmpty() const
{
    return mTop == -1;
}

//----------------------------------------------------------------------------
template <class T>
Bool TStack<T>::IsFull() const
{
    return mTop >= mMaxQuantity-1;
}

//----------------------------------------------------------------------------
template <class T>
void TStack<T>::Push(const T& rkItem)
{
    mTop++;
    if (mTop <= mMaxQuantity-1)
    {
        mpStack[mTop] = rkItem;
    }
    else
    {
        // stack full, should not happen
        WIRE_ASSERT(false);
    }
}

//----------------------------------------------------------------------------
template <class T>
void TStack<T>::Pop(T& rItem)
{
    if (mTop >= 0)
    {
        if (mTop <= mMaxQuantity-1)
        {
            rItem = mpStack[mTop];
        }
        else
        {
            // stack full, should not happen
            WIRE_ASSERT(false);
        }

        mTop--;
    }
    else
    {
        // stack empty, should not happen
        WIRE_ASSERT(false);
    }
}

//----------------------------------------------------------------------------
template <class T>
void TStack<T>::Pop()
{
    if (mTop >= 0)
    {
        if (mTop > mMaxQuantity-1)
        {
            // stack full, should not happen
            WIRE_ASSERT(false);
        }

        mTop--;
    }
    else
    {
        // stack empty, should not happen
        WIRE_ASSERT(false);
    }
}

//----------------------------------------------------------------------------
template <class T>
void TStack<T>::Clear()
{
    mTop = -1;
}

//----------------------------------------------------------------------------
template <class T>
Bool TStack<T>::GetTop(T& rItem) const
{
    if (0 <= mTop && mTop <= mMaxQuantity-1)
    {
        rItem = mpStack[mTop];
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
template <class T>
T& TStack<T>::GetTop() const
{
    WIRE_ASSERT(0 <= mTop && mTop <= mMaxQuantity-1);
    return mpStack[mTop];
}

//----------------------------------------------------------------------------
template <class T>
inline Int TStack<T>::GetQuantity() const
{
    return mTop+1;
}

//----------------------------------------------------------------------------
template <class T>
inline Int TStack<T>::GetMaxQuantity() const
{
    return mMaxQuantity;
}

//----------------------------------------------------------------------------
template <class T>
inline const T* TStack<T>::GetData() const
{
    return mpStack;
}
