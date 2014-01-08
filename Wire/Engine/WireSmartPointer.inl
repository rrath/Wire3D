// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class T>
Pointer<T>::Pointer(T* pObject)
{
    mpObject = pObject;
    if (mpObject)
    {
        mpObject->IncrementReferences();
    }
}

//----------------------------------------------------------------------------
template <class T>
Pointer<T>::Pointer(const Pointer& rPointer)
{
    mpObject = rPointer.mpObject;
    if (mpObject)
    {
        mpObject->IncrementReferences();
    }
}

//----------------------------------------------------------------------------
template <class T>
Pointer<T>::~Pointer()
{
    if (mpObject)
    {
        mpObject->DecrementReferences();
    }
}

//----------------------------------------------------------------------------
template <class T>
inline Pointer<T>::operator T* () const
{
    return mpObject;
}

//----------------------------------------------------------------------------
template <class T>
inline T& Pointer<T>::operator* () const
{
    return *mpObject;
}

//----------------------------------------------------------------------------
template <class T>
inline T* Pointer<T>::operator-> () const
{
    return mpObject;
}

//----------------------------------------------------------------------------
template <class T>
inline T* Pointer<T>::Get() const
{
	return mpObject;
}

//----------------------------------------------------------------------------
template <class T>
Pointer<T>& Pointer<T>::operator= (T* pObject)
{
    if (mpObject != pObject)
    {
        if (pObject)
        {
            pObject->IncrementReferences();
        }

        if (mpObject)
        {
            mpObject->DecrementReferences();
        }

        mpObject = pObject;
    }

	return *this;
}

//----------------------------------------------------------------------------
template <class T>
Pointer<T>& Pointer<T>::operator= (const Pointer& rPointer)
{
    if (mpObject != rPointer.mpObject)
    {
        if (rPointer.mpObject)
        {
            rPointer.mpObject->IncrementReferences();
        }

        if (mpObject)
        {
            mpObject->DecrementReferences();
        }

        mpObject = rPointer.mpObject;
    }

    return *this;
}

//----------------------------------------------------------------------------
template <class T>
inline Bool Pointer<T>::operator== (T* pObject) const
{
    return mpObject == pObject;
}

//----------------------------------------------------------------------------
template <class T>
inline Bool Pointer<T>::operator!= (T* pObject) const
{
    return mpObject != pObject;
}

//----------------------------------------------------------------------------
template <class T>
inline Bool Pointer<T>::operator== (const Pointer& rPointer) const
{
    return mpObject == rPointer.mpObject;
}

//----------------------------------------------------------------------------
template <class T>
inline Bool Pointer<T>::operator!= (const Pointer& rPointer) const
{
    return mpObject != rPointer.mpObject;
}
