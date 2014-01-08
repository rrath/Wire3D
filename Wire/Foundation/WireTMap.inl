// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
template <class TKEY, class TVALUE>
TMap<TKEY, TVALUE>::TMap(UInt quantity, UInt growBy)
{
	mpArray = WIRE_NEW TArray<MapElement>(quantity, growBy);
}

//----------------------------------------------------------------------------
template <class TKEY, class TVALUE>
TMap<TKEY, TVALUE>::~TMap()
{
	WIRE_DELETE mpArray;
}

//----------------------------------------------------------------------------
template <class TKEY, class TVALUE>
TVALUE* TMap<TKEY, TVALUE>::Find(const TKEY& rKey)
{
	const UInt quantity = mpArray->GetQuantity();
	UInt low = 0;
	UInt high = quantity;

	while (low < high)
	{
		UInt mid = low + (high - low) / 2;
		if ((*mpArray)[mid].Key < rKey)
		{
			low = mid + 1; 
		}
		else
		{
			high = mid; 
		}
	}

	if ((low < quantity) && ((*mpArray)[low].Key == rKey))
	{
		return &((*mpArray)[low].Value);
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------
template <class TKEY, class TVALUE>
void TMap<TKEY, TVALUE>::Insert(const TKEY& rKey, const TVALUE& rValue)
{
	WIRE_ASSERT(!Find(rKey));

	UInt low = 0;
	UInt high = mpArray->GetQuantity();

	while (low < high)
	{
		UInt mid = low + (high - low) / 2;
		if ((*mpArray)[mid].Key < rKey)
		{
			low = mid + 1; 
		}
		else
		{
			high = mid; 
		}
	}

	MapElement element;
	element.Key = rKey;
	element.Value = rValue;
	mpArray->Insert(low, element);
}

//----------------------------------------------------------------------------
template <class TKEY, class TVALUE>
void TMap<TKEY, TVALUE>::Remove(const TKEY& rKey)
{
	const UInt quantity = mpArray->GetQuantity();
	UInt low = 0;
	UInt high = quantity;

	while (low < high)
	{
		UInt mid = low + (high - low) / 2;
		if ((*mpArray)[mid].Key < rKey)
		{
			low = mid + 1; 
		}
		else
		{
			high = mid; 
		}
	}

	WIRE_ASSERT((low < quantity) && ((*mpArray)[low].Key == rKey));
	mpArray->RemoveAt(low);
}
