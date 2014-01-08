// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireEffect.h"
#include "WireImage2D.h"
#include "WireIndexBuffer.h"
#include "WireLight.h"
#include "WireRenderObject.h"
#include "WireRenderTarget.h"
#include "WireShader.h"
#include "WireVisibleSet.h"

#ifdef WIRE_WII
#include "WireGXIndexBuffer.h"
#include "WireGXRenderTarget.h"
#include "WireGXShader.h"
#include "WireGXTexture2D.h"
#include "WireGXVertexBuffer.h"
#else
#include "WireDx9IndexBuffer.h"
#include "WireDx9RenderTarget.h"
#include "WireDx9Shader.h"
#include "WireDx9Texture2D.h"
#include "WireDx9VertexBuffer.h"
#endif

using namespace Wire;

Renderer* Renderer::s_pRenderer = NULL;

//----------------------------------------------------------------------------
void Renderer::Initialize(UInt width, UInt height)
{
	mpData = NULL;
	mWidth = width;
	mHeight = height;

	mVertexFormatKey = 0;

	mIdentityNonRS.SetMatrix(Matrix34F::IDENTITY, true);
	mBatchedIndexBuffer = NULL;
	mStaticBatchingMaxIndexCount = 0;
	mDynamicBatchingMaxVertexCount = 0;
	mDynamicBatchingMaxIndexCount = 0;

	mStatistics.mpRenderer = this;
	mStatistics.Reset();

	s_pRenderer = this;
}

//----------------------------------------------------------------------------
void Renderer::Terminate()
{
	ReleaseResources();
	mspCamera = NULL;

	DestroyAll(mIndexBufferMap);
	DestroyAll(mVertexBufferMap);
	DestroyAll(mImage2DMap);
	DestroyAll(mShaderMap);
	DestroyAll(mVertexFormatMap);
	DestroyBatchingBuffers();

	s_pRenderer = NULL;
}

//----------------------------------------------------------------------------
void Renderer::ReleaseResources()
{
	if (mspIndexBuffer)
	{
		Disable(mspIndexBuffer);
	}

	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i])
		{
			Disable(mVertexBuffers[i], i);
		}
	}

	if (mspMaterial)
	{
		Disable(mspMaterial);
	}

	for (UInt i = 0; i < mTexture2Ds.GetQuantity(); i++)
	{
		if (mTexture2Ds[i])
		{
			Disable(mTexture2Ds[i], i);
		}
	}

	for (UInt i = 0; i < mLights.GetQuantity(); i++)
	{
		if (mLights[i])
		{
			SetLight(NULL, i);
		}
	}

	for (UInt i = 0; i < State::MAX_STATE_TYPE; i++)
	{
		mspStates[i] = NULL;
	}

	if (mspPixelShader)
	{
		Disable(mspPixelShader, mspPixelShader);
	}

	if (mspVertexShader)
	{
		Disable(mspVertexShader, mspVertexShader);
	}

	if (mspRenderTarget)
	{
		Disable(mspRenderTarget);
	}

	mspMesh = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Bind(const RenderObject* pRenderObject)
{
	if (!pRenderObject)
	{
		return;
	}

	const Mesh* pMesh = pRenderObject->GetMesh();
	if (pMesh)
	{
		Bind(pMesh->GetIndexBuffer());

		for (UInt i = 0; i < pMesh->GetVertexBuffers().GetQuantity(); i++)
		{
			Bind(pMesh->GetVertexBuffer(i));
		}

		Bind(pMesh->GetVertexBuffers());

		Bind(pMesh); // Binds display lists on Wii
	}

	const Material* pMaterial = pRenderObject->GetMaterial();
	if (pMaterial)
	{
		for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
		{
			Bind(pMaterial->GetTexture(i)->GetImage());
		}

		if (pMaterial->GetPixelShader())
		{
			Bind(pMaterial->GetPixelShader());
		}
	}
}

//----------------------------------------------------------------------------
void Renderer::Unbind(const RenderObject* pRenderObject)
{
	if (!pRenderObject)
	{
		return;
	}

	const Mesh* pMesh = pRenderObject->GetMesh();
	if (pMesh)
	{
		Unbind(pMesh->GetIndexBuffer());

		for (UInt i = 0; i < pMesh->GetVertexBuffers().GetQuantity(); i++)
		{
			Unbind(pMesh->GetVertexBuffer(i));
		}
	}

	const Material* pMaterial = pRenderObject->GetMaterial();
	if (pMaterial)
	{
		for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
		{
			Unbind(pMaterial->GetTexture(i)->GetImage());
		}
	}
}

//----------------------------------------------------------------------------
void Renderer::Bind(const IndexBuffer* pIndexBuffer)
{
	Bind(pIndexBuffer, mIndexBufferMap, &mStatistics.mIBOCount,
		&mStatistics.mIBOsSize);
}

//----------------------------------------------------------------------------
void Renderer::Unbind(const IndexBuffer* pIndexBuffer)
{
	Unbind(pIndexBuffer, mIndexBufferMap, &mStatistics.mIBOCount,
		&mStatistics.mIBOsSize);
}

//----------------------------------------------------------------------------
void Renderer::Enable(const IndexBuffer* pIndexBuffer)
{
	WIRE_ASSERT(mspIndexBuffer == NULL /* Disable the previous IB first. */);
	Enable(pIndexBuffer, mIndexBufferMap);
	mspIndexBuffer = const_cast<IndexBuffer*>(pIndexBuffer);
}

//----------------------------------------------------------------------------
void Renderer::Disable(const IndexBuffer* pIndexBuffer)
{
	WIRE_ASSERT(mspIndexBuffer == pIndexBuffer /* This IB is not enabled */);
	Disable(pIndexBuffer, mIndexBufferMap);
	mspIndexBuffer = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Set(const IndexBuffer* pIndexBuffer)
{
	Set(pIndexBuffer, mspIndexBuffer);
}

//----------------------------------------------------------------------------
void Renderer::Update(const IndexBuffer* pIndexBuffer)
{
	Update(pIndexBuffer, mIndexBufferMap);
}

//----------------------------------------------------------------------------
void Renderer::Update(const IndexBuffer* pIndexBuffer, UInt count,
	UInt offset)
{
	Update(pIndexBuffer, mIndexBufferMap, count, offset);
}

//----------------------------------------------------------------------------
PdrIndexBuffer* Renderer::GetResource(const IndexBuffer* pIndexBuffer)
{
	return GetResource(pIndexBuffer, mIndexBufferMap);
}

//----------------------------------------------------------------------------
void Renderer::Bind(const VertexBuffer* pVertexBuffer)
{
	Bind(pVertexBuffer, mVertexBufferMap, &mStatistics.mVBOCount,
		&mStatistics.mVBOsSize);
}

//----------------------------------------------------------------------------
void Renderer::Unbind(const VertexBuffer* pVertexBuffer)
{
	Unbind(pVertexBuffer, mVertexBufferMap, &mStatistics.mVBOCount,
		&mStatistics.mVBOsSize);
}

//----------------------------------------------------------------------------
void Renderer::Enable(const VertexBuffer* pVertexBuffer, UInt streamIndex)
{
	/* Disable the previous VB first. */
	WIRE_ASSERT(mVertexBuffers[streamIndex] == NULL);

	WIRE_ASSERT(pVertexBuffer);
	const UInt vertexSize = pVertexBuffer->GetAttributes().GetVertexSize();
	PdrVertexBuffer** pValue = mVertexBufferMap.Find(pVertexBuffer);

	if (!pValue)
	{
		Bind(pVertexBuffer);
		pValue = mVertexBufferMap.Find(pVertexBuffer);
		WIRE_ASSERT(pValue);
	}

	WIRE_ASSERT(*pValue);
	(*pValue)->Enable(this, vertexSize, streamIndex);

	mVertexBuffers[streamIndex] = const_cast<VertexBuffer*>(pVertexBuffer);
}

//----------------------------------------------------------------------------
void Renderer::Disable(const VertexBuffer* pVertexBuffer, UInt streamIndex)
{
	/* This VB is not enabled */
	WIRE_ASSERT(mVertexBuffers[streamIndex] == pVertexBuffer);
	Disable(pVertexBuffer, streamIndex, mVertexBufferMap);
	mVertexBuffers[streamIndex] = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Set(const VertexBuffer* pVertexBuffer, UInt streamIndex)
{
	Set(pVertexBuffer, streamIndex, mVertexBuffers);
}

//----------------------------------------------------------------------------
void Renderer::Update(const VertexBuffer* pVertexBuffer)
{
	Update(pVertexBuffer, mVertexBufferMap);
}

//----------------------------------------------------------------------------
void Renderer::Update(const VertexBuffer* pVertexBuffer, UInt count,
	UInt offset)
{
	Update(pVertexBuffer, mVertexBufferMap, count, offset);
}

//----------------------------------------------------------------------------
PdrVertexBuffer* Renderer::GetResource(const VertexBuffer* pVertexBuffer)
{
	return GetResource(pVertexBuffer, mVertexBufferMap);
}

//----------------------------------------------------------------------------
void Renderer::Bind(const Image2D* pImage)
{
	Bool performedBind = Bind(pImage, mImage2DMap, &mStatistics.mTextureCount,
		&mStatistics.mTexturesSize);

	if (performedBind)
	{
		if (pImage->GetUsage() == Buffer::UT_STATIC_DISCARD_ON_BIND)
		{
			const_cast<Image2D*>(pImage)->Discard();
		}
	}
}

//----------------------------------------------------------------------------
void Renderer::Unbind(const Image2D* pImage)
{
	Unbind(pImage, mImage2DMap, &mStatistics.mTextureCount,	&mStatistics.
		mTexturesSize);
}

//----------------------------------------------------------------------------
void Renderer::Enable(const Texture2D* pTexture, UInt unit)
{
	WIRE_ASSERT(mTexture2Ds[unit] == NULL /* Disable previous first */);
	WIRE_ASSERT(pTexture);
	const Image2D* pImage = pTexture->GetImage();
	WIRE_ASSERT(pImage);

	PdrTexture2D** pValue = mImage2DMap.Find(pImage);

	if (!pValue)
	{
		Bind(pImage);
		pValue = mImage2DMap.Find(pImage);
		WIRE_ASSERT(pValue);
	}

	WIRE_ASSERT(*pValue);
	(*pValue)->Enable(this, pTexture, unit);

	mTexture2Ds[unit] = const_cast<Texture2D*>(pTexture);
}

//----------------------------------------------------------------------------
void Renderer::Disable(const Texture2D* pTexture, UInt unit)
{
	WIRE_ASSERT(mTexture2Ds[unit] == pTexture /* This Tex is not enabled */);
	WIRE_ASSERT(pTexture);
	const Image2D* pImage = pTexture->GetImage();
	Disable(pImage, unit, mImage2DMap);
	mTexture2Ds[unit] = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Set(const Texture2D* pTexture, UInt unit)
{
	Set(pTexture, unit, mTexture2Ds);
}

//----------------------------------------------------------------------------
PdrTexture2D* Renderer::GetResource(const Image2D* pImage)
{
	return GetResource(pImage, mImage2DMap);
}

//----------------------------------------------------------------------------
void Renderer::Bind(const Shader* pShader)
{
	Bind(pShader, mShaderMap, &mStatistics.mShaderCount, NULL);
}

//----------------------------------------------------------------------------
void Renderer::Unbind(const Shader* pShader)
{
	Unbind(pShader, mShaderMap, &mStatistics.mShaderCount, NULL);
}

//----------------------------------------------------------------------------
void Renderer::Enable(const Shader* pShader, ShaderPtr& rInUse)
{
	WIRE_ASSERT(rInUse == NULL /* Disable the previous shader first. */);
	Enable(pShader, mShaderMap);
	rInUse = const_cast<Shader*>(pShader);
}

//----------------------------------------------------------------------------
void Renderer::Disable(const Shader* pShader, ShaderPtr& rInUse)
{
	WIRE_ASSERT(rInUse == pShader /* This shader is not enabled */);
	Disable(pShader, mShaderMap);
	rInUse = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Set(const Shader* pShader, ShaderPtr& rInUse)
{
	if (rInUse != pShader)
	{
		if (rInUse)
		{
			Disable(rInUse, rInUse);
		}

		Enable(pShader, rInUse);
	}
}

//----------------------------------------------------------------------------
PdrShader* Renderer::GetResource(const Shader* pShader)
{
	return GetResource(pShader, mShaderMap);
}

//----------------------------------------------------------------------------
void Renderer::Bind(const RenderTarget* pRenderTarget)
{
	Bind(pRenderTarget, mRenderTargetMap, &mStatistics.mRenderTargetCount,
		&mStatistics.mRenderTargetSize);
}

//----------------------------------------------------------------------------
void Renderer::Unbind(const RenderTarget* pRenderTarget)
{
	Unbind(pRenderTarget, mRenderTargetMap, &mStatistics.mRenderTargetCount,
		&mStatistics.mRenderTargetSize);
}

//----------------------------------------------------------------------------
void Renderer::Enable(const RenderTarget* pRenderTarget)
{
	WIRE_ASSERT(mspRenderTarget == NULL /* Disable the previous RT first. */);
	Enable(pRenderTarget, mRenderTargetMap);
	mspRenderTarget = const_cast<RenderTarget*>(pRenderTarget);
}

//----------------------------------------------------------------------------
void Renderer::Disable(const RenderTarget* pRenderTarget)
{
	WIRE_ASSERT(mspRenderTarget == pRenderTarget /* This RT is not enabled */);
	Disable(pRenderTarget, mRenderTargetMap);
	mspRenderTarget = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Set(const RenderTarget* pRenderTarget)
{
	Set(pRenderTarget, mspRenderTarget);
}

//----------------------------------------------------------------------------
Image2D* Renderer::ReadColor(UInt i, const RenderTarget* pRenderTarget)
{
	PdrRenderTarget* pPdrRenderTarget = GetResource(pRenderTarget);
	if (pPdrRenderTarget)
	{
		return pPdrRenderTarget->ReadColor(i, this);
	}

	return NULL;
}

//----------------------------------------------------------------------------
PdrRenderTarget* Renderer::GetResource(const RenderTarget* pRenderTarget)
{
	return GetResource(pRenderTarget, mRenderTargetMap);
}

//----------------------------------------------------------------------------
void Renderer::Bind(const VertexBuffers& rVertexBuffers)
{
	const UInt key = GetVertexFormatKey(rVertexBuffers);
	PdrVertexFormat** pValue = mVertexFormatMap.Find(key);

	if (!pValue)
	{
		PdrVertexFormat* pPdrVertexFormat = WIRE_NEW PdrVertexFormat(this,
			rVertexBuffers);
		mVertexFormatMap.Insert(key, pPdrVertexFormat);
		mStatistics.mVertexFormatCount++;
	}
}

//----------------------------------------------------------------------------
UInt Renderer::GetVertexFormatKey(const VertexBuffers& rVertexBuffers)
{
	WIRE_ASSERT(rVertexBuffers.GetQuantity() < 256);

	const UInt streamCount = rVertexBuffers.GetQuantity();
	TArray<UInt>& rBoundKeys = mVertexFormatKeys[streamCount];

	for (UInt i = 0; i < rBoundKeys.GetQuantity(); i +=streamCount)
	{
		Bool isIdentical = true;
		for (UInt j = 0; j < streamCount; j++)
		{
			if (rBoundKeys[i+j] != rVertexBuffers[j]->GetAttributes().
				GetKey())
			{
				isIdentical = false;
				break;
			}
		}

		if (isIdentical)
		{
			return (streamCount << 24) + i;
		}
	}

	rBoundKeys.SetGrowBy(streamCount);
	for (UInt i = 0; i < streamCount; i++)
	{
		rBoundKeys.Append(rVertexBuffers[i]->GetAttributes().GetKey());
	}

	return (streamCount << 24) + (rBoundKeys.GetQuantity()-streamCount);
}

//----------------------------------------------------------------------------
void Renderer::Enable(const VertexBuffers& rVertexBuffers)
{
	const UInt key = GetVertexFormatKey(rVertexBuffers);
	PdrVertexFormat** pValue = mVertexFormatMap.Find(key);

	if (!pValue)
	{
		Bind(rVertexBuffers);
		pValue = mVertexFormatMap.Find(key);
		WIRE_ASSERT(pValue);
	}

	WIRE_ASSERT(*pValue);
	(*pValue)->Enable(this);

	mVertexFormatKey = key;
}

//----------------------------------------------------------------------------
void Renderer::Disable(const VertexBuffers& rVertexBuffers)
{
	const UInt key = GetVertexFormatKey(rVertexBuffers);
	Disable(key, mVertexFormatMap);
	mVertexFormatKey = 0;
}

//----------------------------------------------------------------------------
void Renderer::Set(const VertexBuffers& rVertexBuffers)
{
	const UInt key = GetVertexFormatKey(rVertexBuffers);
	if (mVertexFormatKey != key)
	{
		if (mVertexFormatKey != 0)
		{
			Disable(mVertexFormatKey, mVertexFormatMap);
		}

		Enable(rVertexBuffers);
	}
}

//----------------------------------------------------------------------------
PdrVertexFormat* Renderer::GetResource(const VertexBuffers&	rVertexBuffers)
{
	const UInt key = GetVertexFormatKey(rVertexBuffers);
	PdrVertexFormat** pValue = mVertexFormatMap.Find(key);

	if (pValue)
	{
		return *pValue;
	}

	return NULL;
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
void Renderer::DestroyAll(THashTable<const Resource*, PdrResource*>& rMap)
{
	typedef THashTable<const Resource*, PdrResource*> Map;

	while (rMap.GetQuantity() > 0)
	{
		typename Map::Iterator it(&rMap);
		const Resource* pKey = NULL;
		it.GetFirst(&pKey);
		WIRE_ASSERT(pKey);
		Unbind(pKey);
	}

	WIRE_ASSERT(rMap.GetQuantity() == 0);
}

//----------------------------------------------------------------------------
void Renderer::DestroyAll(VertexFormatMap& rVertexFormatMap)
{
	while (rVertexFormatMap.GetQuantity() > 0)
	{
		VertexFormatMap::Iterator it(&rVertexFormatMap);
		UInt key;
		PdrVertexFormat** pValue = it.GetFirst(&key);

		if (pValue)
		{
			WIRE_DELETE *pValue;
			rVertexFormatMap.Remove(key);
		}
	}

	mStatistics.mVertexFormatCount = 0;
	WIRE_ASSERT(rVertexFormatMap.GetQuantity() == 0);
}

//----------------------------------------------------------------------------
void Renderer::Enable(const Material* pMaterial, const Lights* pLights)
{
	if (!pMaterial || !pMaterial->HasShaders() || mShaderVersion == 0)
	{
		Enable(pLights);
	}

	if (!pMaterial)
	{
		return;
	}

	if (pMaterial->GetVertexShader())
	{
		Enable(pMaterial->GetVertexShader(), mspVertexShader);
	}

	if (pMaterial->GetPixelShader())
	{
		Enable(pMaterial->GetPixelShader(), mspPixelShader);
	}
	else
	{
		for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
		{
			EnableTextureStage(pMaterial->GetBlendMode(i), i,
				pMaterial->GetTexture(pMaterial->GetSamplerIndex(i))->
				GetImage()->HasAlpha());
		}
	}

	for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
	{
		Enable(pMaterial->GetTexture(pMaterial->GetSamplerIndex(i)), i);
	}

	mspMaterial = const_cast<Material*>(pMaterial);
	SetShaderVariables();
}

//----------------------------------------------------------------------------
void Renderer::Disable(const Material* pMaterial, const Lights* pLights)
{
	if (!pMaterial || !pMaterial->HasShaders() || mShaderVersion == 0)
	{
		Disable(pLights);
	}

	WIRE_ASSERT(mspMaterial == pMaterial);
	if (!pMaterial)
	{
		return;
	}

	if (pMaterial->GetVertexShader())
	{
		Disable(pMaterial->GetVertexShader(), mspVertexShader);
	}

	if (pMaterial->GetPixelShader())
	{
		Disable(pMaterial->GetPixelShader(), mspPixelShader);
	}
	else
	{
		for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
		{
			DisableTextureStage(i);
		}
	}

	for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
	{
		Disable(pMaterial->GetTexture(pMaterial->GetSamplerIndex(i)), i);
	}

	mspMaterial = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Set(const Material* pMaterial, const Lights* pLights)
{
	if (!pMaterial || !pMaterial->HasShaders() || mShaderVersion == 0)
	{
		Set(pLights);
	}

	if (mspMaterial == pMaterial)
	{
		if (mspMaterial)
		{
			SetShaderVariables();
		}

		return;
	}

	UInt currentTextureQuantity = 0;
	Shader* pCurrentPixelShader = NULL;
	Shader* pCurrentVertexShader = NULL;
	if (mspMaterial)
	{
		pCurrentVertexShader = mspMaterial->GetVertexShader();
		currentTextureQuantity = mspMaterial->GetTextureQuantity();
		pCurrentPixelShader = mspMaterial->GetPixelShader();
		WIRE_ASSERT(mspPixelShader == pCurrentPixelShader);
		if (!mspPixelShader)
		{
			for (UInt i = 0; i < mspMaterial->GetTextureQuantity(); i++)
			{
				DisableTextureStage(i);
			}
		}
	}

	UInt newTextureQuantity = 0;
	Shader* pNewPixelShader = NULL;
	Shader* pNewVertexShader = NULL;
	if (pMaterial)
	{
		pNewVertexShader = pMaterial->GetVertexShader();
		if (pNewVertexShader)
		{
			Set(pNewVertexShader, mspVertexShader);
		}

		newTextureQuantity = pMaterial->GetTextureQuantity();
		pNewPixelShader = pMaterial->GetPixelShader();
		if (pNewPixelShader)
		{
			Set(pNewPixelShader, mspPixelShader);
		}
		else
		{
			for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
			{
				EnableTextureStage(pMaterial->GetBlendMode(i), i,
					pMaterial->GetTexture(i)->GetImage()->HasAlpha());
			}
		}

		for (UInt i = 0; i < pMaterial->GetTextureQuantity(); i++)
		{
			Set(pMaterial->GetTexture(pMaterial->GetSamplerIndex(i)), i);
		}
	}

	for (UInt i = newTextureQuantity; i < currentTextureQuantity; i++)
	{
		Disable(mspMaterial->GetTexture(mspMaterial->GetSamplerIndex(i)), i);
	}

	if (pCurrentVertexShader && !pNewVertexShader)
	{
		Disable(pCurrentVertexShader, mspVertexShader);
	}

	if (pCurrentPixelShader && !pNewPixelShader)
	{
		Disable(pCurrentPixelShader, mspPixelShader);
	}

	mspMaterial = const_cast<Material*>(pMaterial);
	SetShaderVariables();
}

//----------------------------------------------------------------------------
void Renderer::Enable(const Mesh* pMesh)
{
	WIRE_ASSERT(pMesh);

	Enable(pMesh->GetIndexBuffer());
	Enable(pMesh->GetVertexBuffers());

	for (UInt i = 0; i < pMesh->GetVertexBuffers().GetQuantity(); i++)
	{
		Enable(pMesh->GetVertexBuffer(i), i);
	}

	mspMesh = const_cast<Mesh*>(pMesh);
}

//----------------------------------------------------------------------------
void Renderer::Disable(const Mesh* pMesh)
{
	WIRE_ASSERT(pMesh);

	for (UInt i = 0; i < pMesh->GetVertexBuffers().GetQuantity(); i++)
	{
		Disable(pMesh->GetVertexBuffer(i), i);
	}

	Disable(pMesh->GetVertexBuffers());
	Disable(pMesh->GetIndexBuffer());

	mspMesh = NULL;
}

//----------------------------------------------------------------------------
void Renderer::Set(const Mesh* pMesh)
{
	WIRE_ASSERT(pMesh);

	if (mspMesh == pMesh)
	{
		return;
	}

	Set(pMesh->GetIndexBuffer());
	Set(pMesh->GetVertexBuffers());

	for (UInt i = 0; i < pMesh->GetVertexBuffers().GetQuantity(); i++)
	{
		Set(pMesh->GetVertexBuffer(i), i);
	}

	UInt count = mspMesh ? mspMesh->GetVertexBuffers().GetQuantity() : 0;
	for (UInt i = pMesh->GetVertexBuffers().GetQuantity(); i < count; i++)
	{
		Disable(mspMesh->GetVertexBuffer(i), i);
	}

	mspMesh = const_cast<Mesh*>(pMesh);
}

//----------------------------------------------------------------------------
void Renderer::Draw(const RenderObject* pRenderObject, const Transformation&
	rTransformation, Bool restoreState)
{
	// assert that this is really a RenderObject, not a cast Effect
	WIRE_ASSERT(DynamicCast<RenderObject>(StaticCast<Object>(pRenderObject)));

	const Mesh* pMesh = pRenderObject->GetMesh();
	WIRE_ASSERT(pMesh && ((pMesh->GetStartIndex() +	pMesh->GetIndexCount()) <=
		pMesh->GetIndexBuffer()->GetQuantity()));

	const Material* pMaterial = pRenderObject->GetMaterial();
	Shader* pVertexShader = pMaterial ?	pMaterial->GetVertexShader() : NULL;
	SetTransformation(rTransformation, pMesh->HasNormal(), pVertexShader);

	if (restoreState)
	{
		Enable(pRenderObject->GetStates());
		Enable(pMesh);
		Enable(pMaterial, pRenderObject->GetLights());

		DrawElements(pMesh->GetVertexCount(), pMesh->GetIndexCount(),
			pMesh->GetStartIndex(), pMesh->GetStartVertex());

		Disable(pMaterial, pRenderObject->GetLights());
		Disable(pMesh);
		Disable(pRenderObject->GetStates());
	}
	else
	{
		Set(pRenderObject->GetStates());
		Set(pMesh);
		Set(pMaterial, pRenderObject->GetLights());

		DrawElements(pMesh->GetVertexCount(), pMesh->GetIndexCount(),
			pMesh->GetStartIndex(), pMesh->GetStartVertex());
	}
}

//----------------------------------------------------------------------------
void Renderer::Draw(VisibleSet* pVisibleSet)
{
	// NOTE: The stack of 2-tuples is limited to having MAX_GLOBAL_EFFECTS
	// elements. This should be plenty, because the chances of having that
	// many global effects in the same path is small (that is a *lot* of
	// effects to apply in one frame). If it needs to be larger for your
	// applications, increase the maximum size.
	UInt indexStack[Effect::MAX_SIMULTANEOUS_EFFECTS][2]; // startIndex, finalIndex
	UInt top = 0;							// stack is initially empty
	indexStack[0][0] = 0;
	indexStack[0][1] = 0;

	const UInt visibleQuantity = pVisibleSet->GetQuantity();
	Object** pVisible;
	Transformation** pTransformations;
	pVisibleSet->GetSet(pVisible, pTransformations);
	RenderObject** pRenderObjects = reinterpret_cast<RenderObject**>(pVisible);

	for (UInt i = 0; i < visibleQuantity; i++)
	{
		if (pVisible[i])
		{
			if (pTransformations[i] == NULL)
			{
				WIRE_ASSERT(DynamicCast<Effect>(pVisible[i]));
				if (top == 0)
				{
					Draw(pRenderObjects, pTransformations, indexStack[0][0],
						indexStack[0][1]);
				}

				// Begin the scope of an effect.
				top++;
				WIRE_ASSERT(top < Effect::MAX_SIMULTANEOUS_EFFECTS);
				indexStack[top][0] = i;
				indexStack[top][1] = i;
			}
			else
			{
				WIRE_ASSERT(DynamicCast<RenderObject>(pVisible[i]));
				// Found a leaf object.
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
			Effect* pEffect = StaticCast<Effect>(pVisible[min]);
			pEffect->Draw(this, pVisible, pTransformations, min+1, max, false);

			if (--top > 0)
			{
				indexStack[top][1] = max + 1;
			}
			else
			{
				indexStack[0][0] = max + 2;
				indexStack[0][1] = max + 2;
			}
		}
	}

	Draw(pRenderObjects, pTransformations, indexStack[0][0], indexStack[0][1]);
}

//----------------------------------------------------------------------------
void Renderer::Draw(TArray<VisibleSet*>& rVisibleSets)
{
	for (UInt i = 0; i < rVisibleSets.GetQuantity(); i++)
	{
		Draw(rVisibleSets[i]);
	}
}

//----------------------------------------------------------------------------
void Renderer::Draw(RenderObject* const pVisible[], Transformation* const
	pTransformations[], UInt min, UInt max)
{
	if (min == max)
	{
		return;
	}

	if (!UsesBatching())
	{
		for (UInt i = min; i < max; i++)
		{
			Draw(pVisible[i], *(pTransformations[i]));
		}

		return;
	}

	while (min < max)
	{
		UInt idx = min;
		const UInt maxStreams = mBatchedVertexBuffers.GetQuantity();

		const Transformation* pAT = pTransformations[idx];
		if (!pAT)
		{
			min++;
			continue;		// skip the start/end marker of an effect
		}

		WIRE_ASSERT(DynamicCast<RenderObject>((Object*)(pVisible[idx])));
		const RenderObject* pA = pVisible[idx];
		Bool hasIdenticalVBOs = pAT->IsIdentity();
		const Mesh* pMeshA = pA->GetMesh();
		WIRE_ASSERT(pMeshA);
		UInt vA = GetVertexFormatKey(pMeshA->GetVertexBuffers());

		for (; idx < max-1; idx++)
		{
			const Transformation* pBT = pTransformations[idx+1];
			if (!pBT)
			{
				continue;	// skip the start/end marker of an effect
			}

			WIRE_ASSERT(DynamicCast<RenderObject>((Object*)(pVisible[idx+1])));
			const RenderObject* pB = StaticCast<RenderObject>(pVisible[idx+1]);
			Bool hadIdenticalVBOs = hasIdenticalVBOs && pBT->IsIdentity();

			const Mesh* pMeshB = pB->GetMesh();
			WIRE_ASSERT(pMeshB);

			if (pA->GetMaterial() != pB->GetMaterial() ||
				pA->GetStateSetID() != pB->GetStateSetID())
			{
				break;
			}

			const VertexBuffers& rVBOsB = pMeshB->GetVertexBuffers();
			UInt vB = GetVertexFormatKey(rVBOsB);
			if ((vA != vB))
			{
				break;
			}

			for (UInt i = 0; i < rVBOsB.GetQuantity(); i++)
			{
				const VertexBufferPtr* pVBOsA = pMeshA->GetVertexBuffers().
					GetArray();
				if (rVBOsB[i] != pVBOsA[i])
				{
					hasIdenticalVBOs = false;
					break;
				}
			}

			if (hadIdenticalVBOs && !hasIdenticalVBOs && (idx > min))
			{
				hasIdenticalVBOs = true;
				break;
			}

			if (!hasIdenticalVBOs && rVBOsB.GetQuantity() > maxStreams)
			{
				break;
			}

  			pA = pB;
  			vA = vB;
		}

		if (idx > min)
		{
			Set(pA->GetStates());
			Set(pA->GetMaterial(), pA->GetLights());
			Set(pMeshA->GetVertexBuffers());

			if (hasIdenticalVBOs)
			{
				const UInt vboCount = pMeshA->GetVertexBuffers().GetQuantity();
				for (UInt i = 0; i < vboCount; i++)
				{
					Set(pMeshA->GetVertexBuffer(i), i);
				}

				for (UInt i = vboCount; i < mVertexBuffers.GetQuantity(); i++)
				{
					if (mVertexBuffers[i])
					{
						Disable(mVertexBuffers[i], i);
					}
				}

				DrawStaticBatches(pVisible, pTransformations, min, idx+1);
			}
			else
			{
				DrawDynamicBatches(pVisible, pTransformations, min, idx+1);
			}
		}
		else
		{
			Draw(pVisible[min], *(pTransformations[min]));
		}

		min = idx+1;
	}
}

//----------------------------------------------------------------------------
void Renderer::DrawStaticBatches(RenderObject* const pVisible[],
	Transformation*	const pTransformations[], UInt min, UInt max)
{
	PdrIndexBuffer* const pIBPdr = mBatchedIndexBuffer;
	void* pIBRaw = pIBPdr->Lock(Buffer::LM_WRITE_ONLY);

	UInt maxIndex = 0;
	UShort minIndex = System::MAX_USHORT;
	UInt batchedIndexCount = 0;

	for (UInt i = min; i < max; i++)
	{
		if (!pTransformations[i])
		{
			continue; // skip the start/end marker of an effect
		}

		RenderObject* pRenderObject = pVisible[i];
		Transformation& rTransformation = *(pTransformations[i]);
		Mesh* const pMesh = pRenderObject->GetMesh();

		if (pMesh->GetIndexCount() > mStaticBatchingMaxIndexCount)
		{
			Draw(pRenderObject, rTransformation);
			continue;
		}

		UInt curMaxIndex = pMesh->GetVertexCount()+pMesh->GetStartVertex()-1;
		maxIndex = maxIndex < curMaxIndex ? curMaxIndex : maxIndex;
		UShort curMinIndex = pMesh->GetStartVertex();
		minIndex = minIndex > curMinIndex ? curMinIndex : minIndex;

		const UInt ibSize = pMesh->GetIndexCount() * sizeof(UShort);

		Bool exceedsBuffer = (ibSize+batchedIndexCount*sizeof(UShort)) >
			mBatchedIndexBuffer->GetBufferSize();

		if (exceedsBuffer)
		{
			if (batchedIndexCount == 0)
			{
				Draw(pRenderObject, rTransformation);
				maxIndex = 0;
				minIndex = System::MAX_USHORT;
				continue;
			}

			pIBPdr->Unlock();
			WIRE_ASSERT(maxIndex < 65535);

			SetTransformation(Transformation::IDENTITY, pVisible[min]->
				GetMesh()->HasNormal(), mspVertexShader);
			DrawBatch(pIBPdr, maxIndex-minIndex+1, batchedIndexCount, minIndex);
			pIBRaw = pIBPdr->Lock(Buffer::LM_WRITE_ONLY);

			maxIndex = 0;
			minIndex = System::MAX_USHORT;
			batchedIndexCount = 0;
			i--;
			continue;
		}

		IndexBuffer* const pIndexBuffer = pMesh->GetIndexBuffer();
		pIndexBuffer->Copy(static_cast<UShort*>(pIBRaw), 0,
			pMesh->GetIndexCount(), pMesh->GetStartIndex());
		pIBRaw = reinterpret_cast<void*>(ibSize + reinterpret_cast<UChar*>(
			pIBRaw));

		mStatistics.mBatchedStatic++;
		batchedIndexCount += pMesh->GetIndexCount();
	}

	pIBPdr->Unlock();

	if (batchedIndexCount > 0)
	{
		WIRE_ASSERT(maxIndex < 65535);
		SetTransformation(Transformation::IDENTITY, pVisible[min]->
			GetMesh()->HasNormal(), mspVertexShader);
		DrawBatch(pIBPdr, maxIndex-minIndex+1, batchedIndexCount, minIndex);
	}
}

//----------------------------------------------------------------------------
void Renderer::DrawDynamicBatches(RenderObject* const pVisible[], 
	Transformation*	const pTransformations[], UInt min, UInt max)
{
	PdrIndexBuffer* const pIBPdr = mBatchedIndexBuffer;
	void* pIBRaw = pIBPdr->Lock(Buffer::LM_WRITE_ONLY);

	const UInt vbCount = pVisible[min]->GetMesh()->GetVertexBuffers().
		GetQuantity();
	for (UInt i = 0; i < vbCount; i++)
	{
		mRawBatchedVertexBuffers[i] = mBatchedVertexBuffers[i]->Lock(
			Buffer::LM_WRITE_ONLY);
	}

	UShort batchedVertexCount = 0;
	UInt batchedIndexCount = 0;

	for (UInt i = min; i < max; i++)
	{
		if (!pTransformations[i])
		{
			continue; // skip the start/end marker of an effect
		}

		WIRE_ASSERT(DynamicCast<RenderObject>((Object*)(pVisible[i])));
		Transformation& rTransformation = *(pTransformations[i]);
		RenderObject* pRenderObject = pVisible[i];
		Mesh* const pMesh = pRenderObject->GetMesh();

		WIRE_ASSERT(vbCount <= mBatchedVertexBuffers.GetQuantity());
		const UInt vertexCount = pMesh->GetVertexCount();
		const UShort minIndex = pMesh->GetStartVertex();

		if (vertexCount > mDynamicBatchingMaxVertexCount ||
			pMesh->GetIndexCount() > mDynamicBatchingMaxIndexCount)
		{
			Draw(pRenderObject, rTransformation);
			continue;
		}

		const UInt ibSize = pMesh->GetIndexCount() * sizeof(UShort);

		Bool exceedsBuffer = (ibSize+batchedIndexCount*sizeof(UShort)) >
			mBatchedIndexBuffer->GetBufferSize() ||
			(batchedVertexCount + vertexCount > 0xFFFF);
		for (UInt j = 0; (j < vbCount) && !exceedsBuffer; j++)
		{
			VertexBuffer* const pVertexBuffer = pMesh->GetVertexBuffer(j);
			UInt vertexSize = pVertexBuffer->GetAttributes().GetVertexSize();
			UInt vbSize = (vertexCount+batchedVertexCount) * vertexSize;
			exceedsBuffer = exceedsBuffer ||
				vbSize > mBatchedVertexBuffers[j]->GetBufferSize();
		}

		if (exceedsBuffer)
		{
			if (batchedIndexCount == 0)
			{
				Draw(pRenderObject, rTransformation);
				continue;
			}

			for (UInt j = 0; j < vbCount; j++)
			{
				mBatchedVertexBuffers[j]->Unlock();
			}

			pIBPdr->Unlock();

			DrawDynamicBatch(pVisible[min]->GetMesh(), pIBPdr,
				mBatchedVertexBuffers, batchedVertexCount, batchedIndexCount);

			pIBRaw = pIBPdr->Lock(Buffer::LM_WRITE_ONLY);
			for (UInt j = 0; j < vbCount; j++)
			{
				mRawBatchedVertexBuffers[j] = mBatchedVertexBuffers[j]->Lock(
					Buffer::LM_WRITE_ONLY);
			}

			batchedVertexCount = 0;
			batchedIndexCount = 0;
			i--;
			continue;
		}

		IndexBuffer* const pIndexBuffer = pMesh->GetIndexBuffer();
		const UShort offset = batchedVertexCount - minIndex;
		pIndexBuffer->Copy(static_cast<UShort*>(pIBRaw), offset, pMesh->
			GetIndexCount(), pMesh->GetStartIndex());
		pIBRaw = reinterpret_cast<void*>(ibSize + reinterpret_cast<UChar*>(
			pIBRaw));

		for (UInt j = 0; j < vbCount; j++)
		{
			VertexBuffer* const pVertexBuffer = pMesh->GetVertexBuffer(j);
			pVertexBuffer->ApplyForward(rTransformation, static_cast<Float*>(
				mRawBatchedVertexBuffers[j]), vertexCount, minIndex);

			UInt vertexSize = pVertexBuffer->GetAttributes().GetVertexSize();
			UInt vbSize = vertexCount * vertexSize;
			mRawBatchedVertexBuffers[j] = reinterpret_cast<void*>(vbSize +
				reinterpret_cast<UChar*>(mRawBatchedVertexBuffers[j]));
		}

		if (rTransformation.IsIdentity())
		{
			mStatistics.mBatchedDynamic++;
		}
		else
		{
			mStatistics.mBatchedDynamicTransformed++;
		}

		batchedVertexCount = batchedVertexCount + static_cast<UShort>(
			vertexCount);
		batchedIndexCount += pMesh->GetIndexCount();
	}

	pIBPdr->Unlock();
	for (UInt i = 0; i < vbCount; i++)
	{
		mBatchedVertexBuffers[i]->Unlock();
	}

	if (batchedIndexCount > 0)
	{
		DrawDynamicBatch(pVisible[min]->GetMesh(), pIBPdr,
			mBatchedVertexBuffers, batchedVertexCount, batchedIndexCount);
	}
}

//----------------------------------------------------------------------------
void Renderer::DrawDynamicBatch(const Mesh* pMesh, PdrIndexBuffer* const pIBPdr,
	TArray<PdrVertexBuffer*>& rVBsPdr, UInt vertexCount, UInt indexCount)
{
	for (UInt i = 0; i < mVertexBuffers.GetQuantity(); i++)
	{
		if (mVertexBuffers[i])
		{
			Disable(mVertexBuffers[i], i);
		}
	}

	const VertexBuffers& rVertexBuffers = pMesh->GetVertexBuffers();
	for (UInt i = 0; i < rVertexBuffers.GetQuantity(); i++)
	{
		UInt vertexSize = rVertexBuffers[i]->GetAttributes().GetVertexSize();
		rVBsPdr[i]->Enable(this, vertexSize, i);

		const UInt vbSize = vertexCount * vertexSize;
		mStatistics.mBatchedVBOTotalData += vbSize;
		if (mStatistics.mBatchedVBOLargestBatch < vbSize)
		{
			mStatistics.mBatchedVBOLargestBatch = vbSize;
		}
	}

	SetTransformation(mIdentityNonRS, pMesh->HasNormal(), mspVertexShader);
	DrawBatch(pIBPdr, vertexCount, indexCount, 0);
	for (UInt i = 0; i < rVertexBuffers.GetQuantity(); i++)
	{
		rVBsPdr[i]->Disable(this, i);
	}

	if (mStatistics.mBatchedVBOMaxLargestBatch < mStatistics.mBatchedVBOLargestBatch)
	{
		mStatistics.mBatchedVBOMaxLargestBatch = mStatistics.mBatchedVBOLargestBatch;
	}
}

//----------------------------------------------------------------------------
void Renderer::DrawBatch(PdrIndexBuffer* const pIBPdr, UInt vertexCount,
	UInt indexCount, UShort minIndex)
{
	mspMesh = NULL;

	if (mspIndexBuffer)
	{
		Disable(mspIndexBuffer);
		mspIndexBuffer = NULL;
	}

	pIBPdr->Enable(this);

	if (mspVertexShader)
	{
		PdrShader* pPdrShader = GetResource(mspVertexShader);
		if (pPdrShader)
		{
			pPdrShader->SetBuiltInVariables(this);
		}
	}

	if (mspPixelShader)
	{
		PdrShader* pPdrShader = GetResource(mspPixelShader);
		if (pPdrShader)
		{
			pPdrShader->SetBuiltInVariables(this);
		}
	}

	DrawElements(vertexCount, indexCount, 0, minIndex);
	pIBPdr->Disable(this);

	mStatistics.mBatchCount++;
	if (mStatistics.mBatchCountMax < mStatistics.mBatchCount)
	{
		mStatistics.mBatchCountMax = mStatistics.mBatchCount;
	}

	const UInt ibSize = indexCount*sizeof(UShort);
	mStatistics.mBatchedIBOTotalData += ibSize;
	if (mStatistics.mBatchedIBOLargestBatch < ibSize)
	{
		mStatistics.mBatchedIBOLargestBatch = ibSize;
	}

	if (mStatistics.mBatchedIBOMaxLargestBatch < mStatistics.mBatchedIBOLargestBatch)
	{
		mStatistics.mBatchedIBOMaxLargestBatch = mStatistics.mBatchedIBOLargestBatch;
	}
}

//----------------------------------------------------------------------------
void Renderer::Enable(const StatePtr spStates[])
{
	State* pState = spStates[State::ALPHA];
	if (pState)
	{
		SetState(StaticCast<StateAlpha>(pState));
	}

	pState = spStates[State::CULL];
	if (pState)
	{
		SetState(StaticCast<StateCull>(pState));
	}

	pState = spStates[State::FOG];
	if (pState)
	{
		SetState(StaticCast<StateFog>(pState));
	}

	pState = spStates[State::MATERIAL];
	if (pState)
	{
		SetState(StaticCast<StateMaterial>(pState));
	}

	pState = spStates[State::WIREFRAME];
	if (pState)
	{
		SetState(StaticCast<StateWireframe>(pState));
	}

	pState = spStates[State::ZBUFFER];
	if (pState)
	{
		SetState(StaticCast<StateZBuffer>(pState));
	}
}

//----------------------------------------------------------------------------
void Renderer::Disable(const StatePtr spStates[])
{
	State* pState;

	if (spStates[State::ALPHA])
	{
		pState = State::Default[State::ALPHA];
		SetState(StaticCast<StateAlpha>(pState));
	}

	if (spStates[State::CULL])
	{
		pState = State::Default[State::CULL];
		SetState(StaticCast<StateCull>(pState));
	}

	if (spStates[State::FOG])
	{
		pState = State::Default[State::FOG];
		SetState(StaticCast<StateFog>(pState));
	}

	if (spStates[State::MATERIAL])
	{
		pState = State::Default[State::MATERIAL];
		SetState(StaticCast<StateMaterial>(pState));
	}

	if (spStates[State::WIREFRAME])
	{
		pState = State::Default[State::WIREFRAME];
		SetState(StaticCast<StateWireframe>(pState));
	}

	if (spStates[State::ZBUFFER])
	{
		pState = State::Default[State::ZBUFFER];
		SetState(StaticCast<StateZBuffer>(pState));
	}
}

//----------------------------------------------------------------------------
void Renderer::Set(const StatePtr spStates[])
{
	State* pState = spStates[State::ALPHA];
	if (pState && pState != mspStates[State::ALPHA])
	{
		SetState(StaticCast<StateAlpha>(pState));
	}

	pState = spStates[State::CULL];
	if (pState && pState != mspStates[State::CULL])
	{
		SetState(StaticCast<StateCull>(pState));
	}

	pState = spStates[State::FOG];
	if (pState && pState != mspStates[State::FOG])
	{
		SetState(StaticCast<StateFog>(pState));
	}

	pState = spStates[State::MATERIAL];
	if (pState && pState != mspStates[State::MATERIAL])
	{
		SetState(StaticCast<StateMaterial>(pState));
	}

	pState = spStates[State::WIREFRAME];
	if (pState && pState != mspStates[State::WIREFRAME])
	{
		SetState(StaticCast<StateWireframe>(pState));
	}

	pState = spStates[State::ZBUFFER];
	if (pState && pState != mspStates[State::ZBUFFER])
	{
		SetState(StaticCast<StateZBuffer>(pState));
	}
}

//----------------------------------------------------------------------------
void Renderer::Enable(const Lights* pLights)
{
	if (!pLights)
	{
		return;
	}

	const Lights& rLights = *pLights;
	UInt lightCount = rLights.GetQuantity();
	if (lightCount == 0)
	{
		return;
	}

 	if (lightCount > mLights.GetQuantity())
 	{
 		lightCount = mLights.GetQuantity();
 	}
	
	ColorRGB ambient = ColorRGB::BLACK;
	for (UInt i = 0; i < lightCount; i++)
	{
		ambient += rLights[i]->Ambient;
		SetLight(rLights[i], i);
	}

	ambient.Saturate();
	EnableLighting(ambient);
}

//----------------------------------------------------------------------------
void Renderer::Disable(const Lights* pLights)
{
	if (!pLights)
	{
		return;
	}

	const Lights& rLights = *pLights;

	UInt lightCount = rLights.GetQuantity();
	if (lightCount == 0)
	{
		return;
	}

	DisableLighting();

	if (lightCount > mLights.GetQuantity())
	{
		lightCount = mLights.GetQuantity();
	}

	for (UInt i = 0; i < lightCount; i++)
	{
		SetLight(NULL, i);
	}
}

//----------------------------------------------------------------------------
void Renderer::Set(const Lights* pLights)
{
	if (!pLights)
	{
		return;
	}

	const Lights& rLights = *pLights;
	UInt lightCount = rLights.GetQuantity();
	if (lightCount > mLights.GetQuantity())
	{
		lightCount = mLights.GetQuantity();
	}

	ColorRGB ambient = ColorRGB::BLACK;
	UInt unit = 0;
	for (; unit < lightCount; unit++)
	{
		if (mLights[unit] != rLights[unit])
		{
			SetLight(rLights[unit], unit);
		}

		ambient.R() = MathF::Max(ambient.R(), rLights[unit]->Ambient.R());
		ambient.G() = MathF::Max(ambient.G(), rLights[unit]->Ambient.G());
		ambient.B() = MathF::Max(ambient.B(), rLights[unit]->Ambient.B());
	}

	for (; unit < mLights.GetQuantity(); unit++)
	{
		if (mLights[unit])
		{
			SetLight(NULL, unit);
		}
	}

	if (lightCount > 0)
	{
		EnableLighting(ambient);
	}
	else
	{
		DisableLighting();
	}
}

//----------------------------------------------------------------------------
void Renderer::CreateBatchingBuffers(UInt iboSize, UInt vboSize,
	UInt maxVertexStreams)
{
	DestroyBatchingBuffers();

	if (!mSupportsBatching || iboSize == 0)
	{
		return;
	}

	mBatchedIndexBuffer = WIRE_NEW PdrIndexBuffer(this, iboSize, Buffer::
		UT_DYNAMIC_WRITE_ONLY);
	mStatistics.mIBOCount++;
	mStatistics.mIBOsSize += mBatchedIndexBuffer->GetBufferSize();
	mStatistics.mBatchIBOSize = mBatchedIndexBuffer->GetBufferSize();

	if (maxVertexStreams > mVertexBuffers.GetQuantity())
	{
		maxVertexStreams = mVertexBuffers.GetQuantity();
	}

	if (vboSize > 0 && maxVertexStreams > 0)
	{
		mBatchedVertexBuffers.SetQuantity(maxVertexStreams);
		mRawBatchedVertexBuffers.SetQuantity(maxVertexStreams);
		for (UInt i = 0; i < maxVertexStreams; i++)
		{
			mBatchedVertexBuffers[i] =  WIRE_NEW PdrVertexBuffer(this,
				vboSize, Buffer::UT_DYNAMIC_WRITE_ONLY);
			const UInt bufferSize = mBatchedVertexBuffers[i]->GetBufferSize();
			mStatistics.mVBOCount++;
			mStatistics.mVBOsSize += bufferSize;
			mStatistics.mBatchVBOCount++;
			mStatistics.mBatchVBOsSize += bufferSize;
		}
	}
}

//----------------------------------------------------------------------------
void Renderer::DestroyBatchingBuffers()
{
	if (mBatchedIndexBuffer)
	{
		mStatistics.mIBOCount--;
		mStatistics.mIBOsSize -= mBatchedIndexBuffer->GetBufferSize();
		mStatistics.mBatchIBOSize = 0;
		WIRE_DELETE mBatchedIndexBuffer;
		mBatchedIndexBuffer = NULL;
	}

	for (UInt i = 0; i < mBatchedVertexBuffers.GetQuantity(); i++)
	{
		const UInt bufferSize = mBatchedVertexBuffers[i]->GetBufferSize();
		mStatistics.mVBOCount--;
		mStatistics.mVBOsSize -= bufferSize;
		mStatistics.mBatchVBOCount--;
		mStatistics.mBatchVBOsSize -= bufferSize;
		WIRE_DELETE mBatchedVertexBuffers[i];
		mBatchedVertexBuffers[i] = NULL;
	}

	mBatchedVertexBuffers.SetQuantity(0);
	mRawBatchedVertexBuffers.SetQuantity(0);
}

//----------------------------------------------------------------------------
void Renderer::InsertInImage2DMap(const Image2D* pImage, PdrTexture2D*
	pPdrTexture)
{
	WIRE_ASSERT(mImage2DMap.Find(pImage) == NULL);
	mImage2DMap.Insert(pImage, pPdrTexture);
}
