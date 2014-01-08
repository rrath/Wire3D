// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireVisibleSet.h"

#include "WireEffect.h"
#include "WireRenderObject.h"

using namespace Wire;

//----------------------------------------------------------------------------
VisibleSet::VisibleSet(UInt maxQuantity, UInt growBy)
	:
	mVisible(maxQuantity, growBy),
	mTransformations(maxQuantity, growBy),
	mKeys(maxQuantity, growBy),
	mVisibleUnwrapped(0, growBy),
	mTransformationsUnwrapped(0, growBy),
	mKeysUnwrapped(0, growBy)
{
	Clear();
}

//----------------------------------------------------------------------------
VisibleSet::~VisibleSet()
{
}

//----------------------------------------------------------------------------
void VisibleSet::Clear()
{
	mVisible.SetQuantity(0, false);
	mTransformations.SetQuantity(0, false);
	mKeys.SetQuantity(0, false);

	mIsUnwrapped = false;
}

//----------------------------------------------------------------------------
void VisibleSet::GetSet(Object**& rObjectArrayPtr, Transformation**&
	rTransformationPtr)
{
	if (mIsUnwrapped)
	{
		rObjectArrayPtr = mVisibleUnwrapped.GetArray();
		rTransformationPtr = mTransformationsUnwrapped.GetArray();
	}
	else
	{
		rObjectArrayPtr = mVisible.GetArray();
		rTransformationPtr = mTransformations.GetArray();
	}
}

//----------------------------------------------------------------------------
void VisibleSet::Sort(Int left, Int right)
{
	QuickSort(mKeys, mVisible.GetArray(), mTransformations.GetArray(), left,
		right);
}

//----------------------------------------------------------------------------
void VisibleSet::SortUnwrapped(Int left, Int right)
{
	QuickSort(mKeysUnwrapped, mVisibleUnwrapped.GetArray(),
		mTransformationsUnwrapped.GetArray(), left,	right);
}

//----------------------------------------------------------------------------
//void VisibleSet::QuickSort(UInt64* const pKeys, Object** const pVisible,
void VisibleSet::QuickSort(TPODArray<UInt64>& rKeys, Object** const pVisible,
	Transformation** pTrafo, Int left, Int right)
{
	Int i = left;
	Int j = right;
	UInt64 pivot = rKeys[(left + right) / 2];

	while (i <= j)
	{
		while (rKeys[i] < pivot)
		{
			i++;
		}

		while (rKeys[j] > pivot)
		{
			j--;
		}

		if (i <= j)
		{
			UInt64 tmp = rKeys[i];
			rKeys[i] = rKeys[j];
			rKeys[j] = tmp;

			WIRE_ASSERT((DynamicCast<RenderObject>(pVisible[i])));
			Object* pTmp = pVisible[i];
			pVisible[i] = pVisible[j];
			pVisible[j] = pTmp;

			Transformation* pTmpTrafo = pTrafo[i];
			pTrafo[i] = pTrafo[j];
			pTrafo[j] = pTmpTrafo;

			i++;
			j--;
		}
	};

	if (left < j)
	{
		QuickSort(rKeys, pVisible, pTrafo, left, j);
	}

	if (i < right)
	{
		QuickSort(rKeys, pVisible, pTrafo, i, right);
	}
}

//----------------------------------------------------------------------------
void VisibleSet::Sort(Bool unwrap)
{
	if (unwrap)
	{
		UnwrapEffectStackAndSort();
		return;
	}

	Object** pVisible = mVisible.GetArray();
	Transformation** pTransformations = mTransformations.GetArray();
	UInt min = 0;

	for (UInt i = 0; i < GetQuantity(); i++)
	{
		if (pVisible[i])
		{
			if (pTransformations[i] == NULL)
			{
				WIRE_ASSERT(DynamicCast<Effect>(pVisible[i]));
				if (min < i)
				{
					Sort(min, i-1);
				}

				// Begin the scope of an effect.
				min = i+1;
			}
		}
		else
		{
 			WIRE_ASSERT(pTransformations[i] == NULL);
			if (min < i)			
			{
				Sort(min, i-1);
			}

			min = i+1;
		}
	}

	if (min < GetQuantity())
	{
		Sort(min, GetQuantity()-1);
	}
}

//----------------------------------------------------------------------------
void VisibleSet::UnwrapEffectStackAndSort()
{
	mVisibleUnwrapped.SetQuantity(0, false);
	mTransformationsUnwrapped.SetQuantity(0, false);
	mKeysUnwrapped.SetQuantity(0, false);

	const UInt visibleMaxQuantity = mVisible.GetMaxQuantity();
	// The destination set will have at least the size of the source set.
	if (mVisibleUnwrapped.GetMaxQuantity() < visibleMaxQuantity)
	{
		mVisibleUnwrapped.SetMaxQuantity(visibleMaxQuantity);
		mTransformationsUnwrapped.SetMaxQuantity(visibleMaxQuantity);
		mKeysUnwrapped.SetMaxQuantity(visibleMaxQuantity);
	}

	UInt indexStack[Effect::MAX_SIMULTANEOUS_EFFECTS][2];
	indexStack[0][0] = 0;
	indexStack[0][1] = 0;
	UInt top = 0;
	UInt left;

	const UInt visibleQuantity = mVisible.GetQuantity();
	Object** pVisible = mVisible.GetArray();
	Transformation** pTransformations = mTransformations.GetArray();

	for (UInt i = 0; i < visibleQuantity; i++)
	{
		if (pVisible[i])
		{
			if (pTransformations[i] == NULL)
			{
				WIRE_ASSERT(DynamicCast<Effect>(pVisible[i]));
				if (top == 0 && indexStack[0][0] < indexStack[0][1])
				{
					WIRE_ASSERT(i == indexStack[0][1]);
					// Sort leaves with no effect
					SortUnwrapped(indexStack[0][0], i-1);
				}

				// Begin the scope of an effect.
				top++;
				WIRE_ASSERT(top < Effect::MAX_SIMULTANEOUS_EFFECTS);
				indexStack[top][0] = i;
				indexStack[top][1] = i;
			}
			else
			{
				// Found a leaf object.
				WIRE_ASSERT(DynamicCast<RenderObject>(pVisible[i]));
				if (top == 0)
				{
					InsertUnwrapped(pVisible[i], pTransformations[i], mKeys[i]);
				}

				indexStack[top][1]++;
			}
		}
		else
		{
			// End the scope of an effect.
			UInt min = indexStack[top][0];
			UInt max = indexStack[top][1];

			WIRE_ASSERT(pTransformations[min] == NULL);
			WIRE_ASSERT(DynamicCast<Effect>(pVisible[min]));
			InsertUnwrapped(pVisible[min], NULL);
			left = mVisibleUnwrapped.GetQuantity();

			for (UInt j = min+1; j <= max; j++)
			{
				if (pTransformations[j])
				{
					WIRE_ASSERT(DynamicCast<RenderObject>(pVisible[j]));
					RenderObject* pRenderObject = StaticCast<RenderObject>(pVisible[j]);
					InsertUnwrapped(pRenderObject, pTransformations[j], mKeys[j]);
				}
			}

			SortUnwrapped(left, mVisibleUnwrapped.GetQuantity()-1);
			InsertUnwrapped(NULL, NULL);

			WIRE_ASSERT(top > 0 /* More 'ends' than 'starts'*/);
			--top;
			indexStack[top][1] = max + 1;
			indexStack[0][0] = mVisibleUnwrapped.GetQuantity();
			indexStack[0][1] = mVisibleUnwrapped.GetQuantity();
		}
	}

	WIRE_ASSERT(top == 0);
	UInt dstQty = mVisibleUnwrapped.GetQuantity();
	if ((dstQty > 0) && (indexStack[0][0] < dstQty-1))
	{
		SortUnwrapped(indexStack[0][0], dstQty-1);
	}

	mIsUnwrapped = true;
}
