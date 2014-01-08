// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

template<class T> TArray<UInt> TInstanceID<T>::s_FreeIDs(0, 10);
template<class T> UInt TInstanceID<T>::s_MaxID = 0;

//----------------------------------------------------------------------------
template <class T>
TInstanceID<T>::TInstanceID()
{
	if (s_FreeIDs.GetQuantity() > 0)
	{
		mID = s_FreeIDs[0];
		s_FreeIDs[0] = s_FreeIDs[s_FreeIDs.GetQuantity()-1];
		s_FreeIDs.SetQuantity(s_FreeIDs.GetQuantity()-1);
		if (s_FreeIDs.GetQuantity() > 0)
		{
			SiftDown(0);
		}

		return;
	}

	mID = ++s_MaxID;
}

//----------------------------------------------------------------------------
template <class T>
TInstanceID<T>::~TInstanceID()
{
	s_FreeIDs.Append(mID);
	SiftUp(s_FreeIDs.GetQuantity()-1);
}

//----------------------------------------------------------------------------
template <class T>
inline TInstanceID<T>::operator UInt () const
{
	return mID;
}

//----------------------------------------------------------------------------
template <class T>
inline UInt TInstanceID<T>::GetMaxID()
{
	return s_MaxID;
}

//----------------------------------------------------------------------------
template <class T>
void TInstanceID<T>::SiftUp(UInt nodeIndex)
{
	if (nodeIndex != 0)
	{
		UInt parentIndex = (nodeIndex - 1) / 2;

		if (s_FreeIDs[parentIndex] > s_FreeIDs[nodeIndex])
		{
			UInt tmp = s_FreeIDs[parentIndex];
			s_FreeIDs[parentIndex] = s_FreeIDs[nodeIndex];
			s_FreeIDs[nodeIndex] = tmp;
			SiftUp(parentIndex);
		}
	}
}
	
//----------------------------------------------------------------------------
template <class T>
void TInstanceID<T>::SiftDown(UInt nodeIndex)
{
	UInt minIndex;
	UInt leftChildIndex = 2*nodeIndex+1;
	UInt rightChildIndex = leftChildIndex+1;

	if (rightChildIndex >= s_FreeIDs.GetQuantity())
	{
		if (leftChildIndex >= s_FreeIDs.GetQuantity())
		{
			return;
		}
		else
		{
			minIndex = leftChildIndex;
		}
	}
	else
	{
		if (s_FreeIDs[leftChildIndex] <= s_FreeIDs[rightChildIndex])
		{
			minIndex = leftChildIndex;
		}
		else
		{
			minIndex = rightChildIndex;
		}
	}

	if (s_FreeIDs[nodeIndex] > s_FreeIDs[minIndex])
	{
		UInt tmp = s_FreeIDs[minIndex];
		s_FreeIDs[minIndex] = s_FreeIDs[nodeIndex];
		s_FreeIDs[nodeIndex] = tmp;
		SiftDown(minIndex);
	}
}
