// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Camera* Renderer::GetCamera() const
{
	return mspCamera;
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetHeight() const
{
	return mHeight;
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetWidth() const
{
	return mWidth;
}

//----------------------------------------------------------------------------
inline Float Renderer::GetMaxAnisotropy() const
{
	return mMaxAnisotropy;
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetMaxTextureStages() const
{
	return mTexture2Ds.GetQuantity();
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetMaxTextureWidth() const
{
	return mMaxTextureWidth;
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetMaxTextureHeight() const
{
	return mMaxTextureHeight;
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetMaxVertexStreams() const
{
	return mVertexBuffers.GetQuantity();
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetShaderVersion() const
{
	return mShaderVersion;
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetMaxSimultaneousRenderTargets() const
{
	return mMaxSimultaneousRenderTargets;
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetMaxLights() const
{
	return mLights.GetQuantity();
}

//----------------------------------------------------------------------------
inline const ColorRGBA& Renderer::GetClearColor() const
{
	return mClearColor;
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline Bool Renderer::Bind(const Resource* pResource, THashTable<const
	Resource*, PdrResource*>& rMap, UInt* pCount, UInt* pSize)
{
	WIRE_ASSERT(pResource);
	PdrResource** pValue = rMap.Find(pResource);

	if (!pValue)
	{
		PdrResource* pPdrResource = WIRE_NEW PdrResource(this, pResource);
		rMap.Insert(pResource, pPdrResource);
		if (pCount)
		{
			(*pCount)++;
		}

		if (pSize)
		{
			(*pSize) += pPdrResource->GetBufferSize();
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline void Renderer::Unbind(const Resource* pResource, THashTable<const
	Resource*, PdrResource*>& rMap, UInt* pCount, UInt* pSize)
{
	PdrResource** pValue = rMap.Find(pResource);

	if (pValue)
	{
		if (pCount)
		{
			(*pCount)--;
		}

		if (pSize)
		{
			(*pSize) -= (*pValue)->GetBufferSize();
		}

		WIRE_DELETE *pValue;
		rMap.Remove(pResource);
	}
}

//----------------------------------------------------------------------------
template <typename Resource>
void Renderer::UnbindAll(const Resource* pResource)
{
	if (s_pRenderer)
	{
		s_pRenderer->Unbind(pResource);
	}
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline void Renderer::Enable(const Resource* pResource, THashTable<
	const Resource*, PdrResource*>& rMap)
{
	WIRE_ASSERT(pResource);
	PdrResource** pValue = rMap.Find(pResource);

	if (!pValue)
	{
		Bind(pResource);
		pValue = rMap.Find(pResource);
		WIRE_ASSERT(pValue);
	}

	WIRE_ASSERT(*pValue);
	(*pValue)->Enable(this);
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline void Renderer::Disable(const Resource* pResource, THashTable<
	const Resource*, PdrResource*>& rMap)
{
	WIRE_ASSERT(pResource);
	PdrResource** pValue = rMap.Find(pResource);

	if (pValue)
	{
		(*pValue)->Disable(this);
	}
	else
	{
		WIRE_ASSERT(false); // resource is not bound
	}
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline void Renderer::Disable(const Resource* pResource, UInt index,
	THashTable<const Resource*, PdrResource*>& rMap)
{
	WIRE_ASSERT(pResource);
	PdrResource** pValue = rMap.Find(pResource);

	if (pValue)
	{
		(*pValue)->Disable(this, index);
	}
	else
	{
		WIRE_ASSERT(false); // resource is not bound
	}
}

//----------------------------------------------------------------------------
template <typename PdrResource>
inline void Renderer::Disable(const UInt key, THashTable<UInt, PdrResource*>&
	rMap)
{
	PdrResource** pValue = rMap.Find(key);

	if (pValue)
	{
		(*pValue)->Disable(this);
	}
	else
	{
		WIRE_ASSERT(false); // key is not bound
	}
}

//----------------------------------------------------------------------------
template <typename Resource>
inline void Renderer::Set(const Resource* pResource, Pointer<Resource> spInUse)
{
	if (spInUse != pResource)
	{
		if (spInUse)
		{
			Disable(spInUse);
		}

		Enable(pResource);
	}
}

//----------------------------------------------------------------------------
template <typename Resource>
inline void Renderer::Set(const Resource* pResource, UInt index, TArray<
	Pointer<Resource> >& rInUse)
{
	WIRE_ASSERT(rInUse.GetQuantity() > index);
	if (rInUse[index] != pResource)
	{
		if (rInUse[index])
		{
			Disable(rInUse[index], index);
		}

		Enable(pResource, index);
	}
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline void Renderer::Update(const Resource* pResource, THashTable<
	const Resource*, PdrResource*>& rMap)
{
	PdrResource** pValue = rMap.Find(pResource);

	if (pValue)
	{
		(*pValue)->Update(pResource);
	}
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline void Renderer::Update(const Resource* pResource, THashTable<
	const Resource*, PdrResource*>& rMap, UInt count, UInt offset)
{
	PdrResource** pValue = rMap.Find(pResource);
	if (pValue)
	{
		(*pValue)->Update(pResource, count, offset);
	}
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
inline PdrResource* Renderer::GetResource(const Resource* pResource,
	THashTable<const Resource*, PdrResource*>& rMap)
{
	PdrResource** pValue = rMap.Find(pResource);

	if (pValue)
	{
		return *pValue;
	}

	return NULL;
}

//----------------------------------------------------------------------------
inline const StateAlpha* Renderer::GetStateAlpha() const
{
	return StaticCast<StateAlpha>(mspStates[State::ALPHA]);
}

//----------------------------------------------------------------------------
inline const StateCull* Renderer::GetStateCull() const
{
	return StaticCast<StateCull>(mspStates[State::CULL]);
}

//----------------------------------------------------------------------------
inline const StateFog* Renderer::GetStateFog() const
{
	return StaticCast<StateFog>(mspStates[State::FOG]);
}

//----------------------------------------------------------------------------
inline const StateMaterial* Renderer::GetStateMaterial() const
{
	return StaticCast<StateMaterial>(mspStates[State::MATERIAL]);
}

//----------------------------------------------------------------------------
inline const StateWireframe* Renderer::GetStateWireframe() const
{
	return StaticCast<StateWireframe>(mspStates[State::WIREFRAME]);
}

//----------------------------------------------------------------------------
inline const StateZBuffer* Renderer::GetStateZBuffer() const
{
	return StaticCast<StateZBuffer>(mspStates[State::ZBUFFER]);
}

//----------------------------------------------------------------------------
inline Bool Renderer::SupportsBatching() const
{
	return mSupportsBatching;
}

//----------------------------------------------------------------------------
inline Bool Renderer::UsesBatching() const
{
	Bool usesStaticBatching = mStaticBatchingMaxIndexCount > 0;
	Bool usesDynamicBatching = (mDynamicBatchingMaxVertexCount > 0) &&
		(mDynamicBatchingMaxIndexCount > 0);
	return mBatchedIndexBuffer && (usesStaticBatching || 
		(usesDynamicBatching && mBatchedVertexBuffers.GetQuantity() > 0));
}

//----------------------------------------------------------------------------
inline UInt Renderer::GetStaticBatchingThreshold() const
{
	return mStaticBatchingMaxIndexCount / 3;
}

//----------------------------------------------------------------------------
inline void Renderer::SetStaticBatchingThreshold(UInt maxTriangles)
{
	if (mSupportsBatching)
	{
		mStaticBatchingMaxIndexCount = maxTriangles * 3;
	}
}

//----------------------------------------------------------------------------
inline void Renderer::GetDynamicBatchingThreshold(UInt& rMaxVertices,
	UInt& rMaxTriangles) const
{
	rMaxVertices = mDynamicBatchingMaxVertexCount;
	rMaxTriangles = mDynamicBatchingMaxIndexCount / 3;
}

//----------------------------------------------------------------------------
inline void Renderer::SetDynamicBatchingThreshold(UInt maxVertices,
	UInt maxTriangles)
{
	if (mSupportsBatching)
	{
		mDynamicBatchingMaxVertexCount = maxVertices;
		mDynamicBatchingMaxIndexCount = maxTriangles * 3;
	}
}

//----------------------------------------------------------------------------
inline RendererStatistics* Renderer::GetStatistics()
{
	return &mStatistics;
}

//----------------------------------------------------------------------------
inline PdrRendererData* Renderer::GetRendererData() const
{
	return mpData;
}
