// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt Node::GetQuantity() const
{
	return mChildren.GetQuantity();
}

//----------------------------------------------------------------------------
template <class T>
inline T* Node::FindChild(Bool findDerivedTypes) const
{
	return StaticCast<T>(FindChild(T::TYPE, findDerivedTypes));
}

//----------------------------------------------------------------------------
template <class T>
void Node::FindChildren(TArray<T*>& rChildren, Bool findDerivedTypes) const
{
	WIRE_ASSERT(T::TYPE.IsDerived(Spatial::TYPE));

	for (UInt i = 0; i < mChildren.GetQuantity(); i++)
	{
		if (mChildren[i])
		{
			if (findDerivedTypes)
			{
				if (mChildren[i]->IsDerived(T::TYPE))
				{
					rChildren.Append(StaticCast<T>(mChildren[i]));
				}
			}
			else
			{
				if (mChildren[i]->IsExactly(T::TYPE))
				{
					rChildren.Append(StaticCast<T>(mChildren[i]));
				}
			}

			const Node* pNode = DynamicCast<Node>(mChildren[i]);
			if (pNode)
			{
				pNode->FindChildren(rChildren, findDerivedTypes);
			}
		}
	}
}

//----------------------------------------------------------------------------
template <class T>
inline T* Node::FindController(Bool findDerivedTypes) const
{
	return StaticCast<T>(FindController(T::TYPE, findDerivedTypes));
}

//----------------------------------------------------------------------------
template <class T>
void Node::FindControllers(TArray<T*>& rControllers, Bool findDerivedTypes) const
{
	GetControllers<T>(rControllers, findDerivedTypes);

	for (UInt i = 0; i < mChildren.GetQuantity(); i++)
	{
		const Node* pNode = DynamicCast<Node>(mChildren[i]);
		if (pNode)
		{
			pNode->FindControllers<T>(rControllers, findDerivedTypes);
		}
		else if (mChildren[i])
		{
			return mChildren[i]->GetControllers<T>(rControllers, findDerivedTypes);
		}
	}
}

//----------------------------------------------------------------------------
inline UInt Node::GetEffectQuantity() const
{
	return mEffects.GetQuantity();
}

//----------------------------------------------------------------------------
inline Effect* Node::GetEffect(UInt i) const
{
	WIRE_ASSERT(i < mEffects.GetQuantity());
	return mEffects[i];
}

//----------------------------------------------------------------------------
inline void Node::DetachEffect(Effect* pEffect)
{
	mEffects.Remove(pEffect);
}

//----------------------------------------------------------------------------
inline void Node::DetachAllEffects()
{
	mEffects.RemoveAll();
}

//----------------------------------------------------------------------------
inline RenderObject* Node::GetRenderObject()
{
	return mspRenderObject;
}

//----------------------------------------------------------------------------
inline const RenderObject* Node::GetRenderObject() const
{
	return mspRenderObject;
}

//----------------------------------------------------------------------------
inline void Node::SetLayerMask(UInt64 layerMask)
{
	mLayerMask = layerMask;
}

//----------------------------------------------------------------------------
inline UInt64 Node::GetLayerMask() const
{
	return mLayerMask;
}
