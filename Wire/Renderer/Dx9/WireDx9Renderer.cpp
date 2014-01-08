// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireDx9IndexBuffer.h"
#include "WireDx9RendererData.h"
#include "WireDx9RendererInput.h"
#include "WireDx9Shader.h"
#include "WireDx9Texture2D.h"
#include "WireDx9VertexBuffer.h"
#include "WireIndexBuffer.h"
#include "WireLight.h"
#include "WireMesh.h"
#include "WireRenderTarget.h"
#include "WireShader.h"

using namespace Wire;

//----------------------------------------------------------------------------
Renderer::Renderer(PdrRendererInput& rInput, UInt width, UInt height,
	Bool isFullscreen, Bool useVSync)
	:
	mVertexBuffers(16, 16),
	mVertexFormatKeys(16, 16),
	mMaxAnisotropy(1.0F),
	mSupportsBatching(true)
{
	Initialize(width, height);

	mpData = WIRE_NEW PdrRendererData(this);
	WIRE_ASSERT(mpData);

	IDirect3D9*& rD3D = mpData->D3D;
	rD3D = Direct3DCreate9(D3D_SDK_VERSION);
	WIRE_ASSERT(rD3D);

	D3DPRESENT_PARAMETERS& rPresent = mpData->Present;
	rPresent.BackBufferWidth = width;
	rPresent.BackBufferHeight = height;
	rPresent.BackBufferCount = 1;
	rPresent.hDeviceWindow = rInput.WindowHandle;
	rPresent.Flags = 0;
	if (useVSync)
	{
		rPresent.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
	{
		rPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	if (isFullscreen)
	{
		rPresent.Windowed = FALSE;
		rPresent.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		rPresent.BackBufferFormat = D3DFMT_X8R8G8B8;
	}
	else
	{
		rPresent.Windowed = TRUE;
		rPresent.FullScreen_RefreshRateInHz = 0;
		rPresent.BackBufferFormat = D3DFMT_UNKNOWN;
	}

	// Request depth and stencil buffers. The parameters are not independent
	// for DirectX. For now, just grab a 24-bit depth buffer and an 8-bit
	// stencil buffer.
	rPresent.EnableAutoDepthStencil = TRUE;
	rPresent.AutoDepthStencilFormat = D3DFMT_D24S8;

	// Request multi sampling.
	rPresent.SwapEffect = D3DSWAPEFFECT_FLIP; 
	rPresent.MultiSampleType = D3DMULTISAMPLE_NONE;
	rPresent.MultiSampleQuality = 0;

	// Query the device for its capabilities.
	D3DCAPS9 deviceCaps;
	HRESULT hr;
	hr = rD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &deviceCaps);
	WIRE_ASSERT(SUCCEEDED(hr));

	mpData->Supports32BitIndices = deviceCaps.MaxVertexIndex > 0xffff ?
		true : false;

	mMaxTextureWidth = deviceCaps.MaxTextureWidth;
	mMaxTextureHeight = deviceCaps.MaxTextureHeight;
	UInt maxStreams = deviceCaps.MaxStreams > 16 ? 16 : deviceCaps.MaxStreams;
	mVertexBuffers.SetQuantity(maxStreams);
	mVertexFormatKeys.SetQuantity(maxStreams);
	mTexture2Ds.SetQuantity(deviceCaps.MaxTextureBlendStages);
	mpData->SamplerStates.SetQuantity(deviceCaps.MaxTextureBlendStages);
	mpData->TextureStageStates.SetQuantity(deviceCaps.MaxTextureBlendStages);
	UInt maxLights = deviceCaps.MaxActiveLights;
	mLights.SetQuantity(maxLights <= 0 ? 8 : maxLights);

	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders
	// in HW then switch to SWVP.
	DWORD behaviorFlags = D3DCREATE_FPU_PRESERVE;
	if((deviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
		deviceCaps.VertexShaderVersion < D3DVS_VERSION(1, 1))
	{
		behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	else
	{
		behaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		// The Renderer does not use any Get* D3D api calls, thus it is
		// capable of using a pure device.
// 		if(deviceCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
// 		{
// 			behaviorFlags |= D3DCREATE_PUREDEVICE;
// 		}
	}

	DWORD psVer = deviceCaps.PixelShaderVersion;
	DWORD vsVer = deviceCaps.VertexShaderVersion;
	mShaderVersion = D3DSHADER_VERSION_MAJOR(psVer) << 24 |
		D3DSHADER_VERSION_MINOR(psVer) << 16 |
		D3DSHADER_VERSION_MAJOR(vsVer) << 8 | D3DSHADER_VERSION_MINOR(vsVer);

	mMaxSimultaneousRenderTargets = deviceCaps.NumSimultaneousRTs;

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;

	hr = -1;

#ifdef WIRE_DEBUG
	// Look for 'NVIDIA PerfHUD' adapter
	// If it is present, override default settings
	for (UINT adapter = 0; adapter < rD3D->GetAdapterCount(); adapter++)
	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT res = rD3D->GetAdapterIdentifier(adapter, 0, &Identifier);
		WIRE_ASSERT(SUCCEEDED(res));
		if (strstr(Identifier.Description,"PerfHUD") != 0)
		{
			// try to create a NVPerfHUD compatible adapter
			hr = rD3D->CreateDevice(
				adapter,
				D3DDEVTYPE_REF,
				rInput.WindowHandle,
				D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE,
				&rPresent,
				&rDevice);
			WIRE_ASSERT(SUCCEEDED(hr));
			break;
		}
	}
#endif

	if (FAILED(hr))
	{
		hr = rD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			rInput.WindowHandle, behaviorFlags, &rPresent, &rDevice);
		WIRE_ASSERT(SUCCEEDED(hr));
	}

	if ((deviceCaps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY ) > 0)
	{
		if (deviceCaps.TextureCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
		{
			mpData->SupportsMinFAniso = true;
		}

		if (deviceCaps.TextureCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
		{
			mpData->SupportsMagFAniso = true;
		}

		mMaxAnisotropy = static_cast<Float>(deviceCaps.MaxAnisotropy);
	}

	// Turn off lighting (DX defaults to lighting on).
	hr = rDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	WIRE_ASSERT(SUCCEEDED(hr));

	hr = rDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	WIRE_ASSERT(SUCCEEDED(hr));
	mpData->UsesRenormalizeNormals = false;

	// Initialize global render state to default settings.
	Set(State::Default);
}

//----------------------------------------------------------------------------
Renderer::~Renderer()
{
	Terminate();

	if (mpData->D3DDevice)
	{
		mpData->D3DDevice->Release();
	}

	if (mpData->D3D)
	{
		mpData->D3D->Release();
	}

	WIRE_DELETE mpData;
}

//----------------------------------------------------------------------------
void Renderer::ClearBuffers()
{
	if (mpData->IsDeviceLost)
	{
		return;
	}

	DWORD clearColor = D3DCOLOR_COLORVALUE(mClearColor.R(),
		mClearColor.G(), mClearColor.B(), mClearColor.A());

	HRESULT hr;
	hr = mpData->D3DDevice->Clear(0, 0,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER /*| D3DCLEAR_STENCIL*/,
		clearColor, 1.0F/*mClearDepth*/, static_cast<DWORD>(0/*mClearStencil*/));
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void Renderer::DisplayBackBuffer()
{
	if (mpData->IsDeviceLost)
	{
		return;
	}

	HRESULT hr;
	hr = mpData->D3DDevice->Present(NULL, NULL, NULL, NULL);

	if (hr == D3DERR_DEVICELOST)
	{
		mpData->IsDeviceLost = true;
	}
	else
	{
		WIRE_ASSERT(SUCCEEDED(hr));
	}
}

//----------------------------------------------------------------------------
void Renderer::Bind(const Mesh*)
{
}

//----------------------------------------------------------------------------
void Renderer::Unbind(const Mesh*)
{
}

//----------------------------------------------------------------------------
Bool Renderer::PreDraw(Camera* pCamera)
{
	SetCamera(pCamera);

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;
	hr = rDevice->TestCooperativeLevel();
    
    switch (hr)
    {
    case D3DERR_DEVICELOST:
		mpData->IsDeviceLost = true;
		return false;

	case D3DERR_DEVICENOTRESET:
		if (!mpData->IsDeviceLost)
		{
			mpData->IsDeviceLost = true;
			return false;
		}

		ResetDevice();
		break;

	case D3D_OK:
		if (mpData->IsDeviceLost)
		{
			ResetDevice();
		}

		break;

	default:
		WIRE_ASSERT(false);
    }

    hr = rDevice->BeginScene();
    WIRE_ASSERT(SUCCEEDED(hr));

	Set(State::Default);

	return true;
}

//----------------------------------------------------------------------------
void Renderer::PostDraw()
{
	HRESULT hr;
	hr = mpData->D3DDevice->EndScene();
	if (!mpData->IsDeviceLost)
	{
		WIRE_ASSERT(SUCCEEDED(hr));
	}

	// Reset state cache (state is not preserved outside Begin/EndScene()),
	// and release smart pointers cached by the Renderer.
	ReleaseResources();

	mspCamera = NULL;
}

//----------------------------------------------------------------------------
void Renderer::SetShaderVariables()
{
	if (mspVertexShader)
	{
		PdrShader* pPdrShader = GetResource(mspVertexShader);
		if (pPdrShader)
		{
			pPdrShader->SetBuiltInVariables(this);
		}

		if (mspMaterial && mspMaterial->GetVertexShaderVariables())
		{
			SetShaderVariables(pPdrShader, mspMaterial->
				GetVertexShaderVariables());
		}
	}

	if (mspPixelShader)
	{
		PdrShader* pPdrShader = GetResource(mspPixelShader);
		if (pPdrShader)
		{
			pPdrShader->SetBuiltInVariables(this);
		}

		if (mspMaterial && mspMaterial->GetPixelShaderVariables())
		{
			SetShaderVariables(pPdrShader, mspMaterial->
				GetPixelShaderVariables());
		}
	}
}

//----------------------------------------------------------------------------
void Renderer::SetShaderVariables(PdrShader* pPdrShader, const
	ShaderVariables* pShaderVars)
{
	WIRE_ASSERT(pPdrShader && pShaderVars);

	const TArray<Matrix4F>& rMatrices = pShaderVars->GetMatrices();
	const TArray<String>& rMatrixNames = pShaderVars->GetMatrixNames();
	for (UInt i = 0; i < rMatrices.GetQuantity(); i++)
	{
		pPdrShader->SetMatrix(this, rMatrixNames[i], &(rMatrices[i]));
	}

	const TArray<Float>& rFloats = pShaderVars->GetFloats();
	const TArray<String>& rFloatNames = pShaderVars->GetFloatNames();
	for (UInt i = 0; i < rFloats.GetQuantity(); i++)
	{
		pPdrShader->SetFloat(this, rFloatNames[i], rFloats[i]);
	}

	const TArray<Vector4F>& rFloat4s = pShaderVars->GetFloat4s();
	const TArray<String>& rFloat4Names = pShaderVars->GetFloat4Names();
	for (UInt i = 0; i < rFloat4s.GetQuantity(); i++)
	{
		pPdrShader->SetFloat4(this, rFloat4Names[i], &(rFloat4s[i]));
	}
}

//----------------------------------------------------------------------------
void Renderer::SetTransformation(const Transformation& rWorld, Bool
	processNormals, Shader* pVertexShader)
{
	processNormals = pVertexShader && mShaderVersion ? false : processNormals;

	Bool needsRenormalization = processNormals;
	if (processNormals && rWorld.IsUniformScale())
	{
		if (rWorld.GetUniformScale() == 1.0F)
		{
			needsRenormalization = false;
		}
	}

	HRESULT hr;
	IDirect3DDevice9*& rDevice = mpData->D3DDevice;

	if (needsRenormalization != mpData->UsesRenormalizeNormals)
	{
		DWORD normalize = needsRenormalization ? TRUE : FALSE;
		hr = rDevice->SetRenderState(D3DRS_NORMALIZENORMALS, normalize);
		WIRE_ASSERT(SUCCEEDED(hr));
		mpData->UsesRenormalizeNormals = needsRenormalization;
	}

	Matrix4F world;
	rWorld.GetHomogeneous(world);

	if (pVertexShader)
	{
		mpData->ModelView = world * mpData->View;
		mpData->ModelViewProjection = mpData->ModelView * mpData->Projection;
	}
	else
	{
		hr = rDevice->SetTransform(D3DTS_WORLD, reinterpret_cast<D3DMATRIX*>(
			&world));
		WIRE_ASSERT(SUCCEEDED(hr));
	}
}

//----------------------------------------------------------------------------
void Renderer::DrawElements(UInt vertexCount, UInt indexCount,
	UInt startIndex, UInt startVertex)
{
	const UInt triangleCount = indexCount/3;
	mStatistics.mDrawCalls++;
	mStatistics.mTriangles += triangleCount;
	mStatistics.mVertices += vertexCount;

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;
	hr = rDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, startVertex,
		vertexCount, startIndex, triangleCount);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void Renderer::SetCamera(Camera* pCamera)
{
	if (!pCamera)
	{
		return;
	}

	mspCamera = pCamera;
	OnFrameChange();
	OnViewportChange();

	const Float n = pCamera->GetDMin();
	const Float f = pCamera->GetDMax();
	const Float b = pCamera->GetUMin();
	const Float t = pCamera->GetUMax();
	const Float l = pCamera->GetRMin();
	const Float r = pCamera->GetRMax();

	const Float w = 1.0F/(r-l);
	const Float h = 1.0F/(t-b);
	const Float d = 1.0F/(f-n);

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	
	if (pCamera->IsPerspective())
	{
		mpData->Projection = Matrix4F(
			2.0F*n*w, 0.0F,     0.0F,  0.0F,
			0.0F,     2.0F*n*h, 0.0F,  0.0F,
			-(r+l)*w, -(t+b)*h, f*d,   1.0F,
			0.0F,     0.0F,    -n*f*d, 0.0F );
	}
	else
	{
		mpData->Projection = Matrix4F(
			2.0F*w,   0.0F,     0.0F, 0.0F,
			0.0F,     2.0F*h,   0.0F, 0.0F,
			0.0F,     0.0F,     d,    0.0F,
			-(l+r)*w, -(t+b)*h, -n*d, 1.0F );

		Matrix4F halfPixelOffset(false);
		halfPixelOffset[3][0] = 0.5F;
		halfPixelOffset[3][1] = 0.5F;
		mpData->Projection = halfPixelOffset * mpData->Projection;
	}

	rDevice->SetTransform(D3DTS_PROJECTION, reinterpret_cast<D3DMATRIX*>(
		&mpData->Projection));
	rDevice->SetTransform(D3DTS_VIEW, reinterpret_cast<D3DMATRIX*>(&mpData->
		View));

	mpData->ViewProjection = mpData->View * mpData->Projection;
}

//----------------------------------------------------------------------------
void Renderer::OnFrameChange()
{
	WIRE_ASSERT(mspCamera);
	Vector3F eye = mspCamera->GetLocation();
	Vector3F rVector = mspCamera->GetRVector();
	Vector3F uVector = mspCamera->GetUVector();
	Vector3F dVector = mspCamera->GetDVector();

	Matrix4F& rViewMatrix = mpData->View;
	rViewMatrix[0][0] = rVector[0];
	rViewMatrix[0][1] = uVector[0];
	rViewMatrix[0][2] = dVector[0];
	rViewMatrix[0][3] = 0.0F;
	rViewMatrix[1][0] = rVector[1];
	rViewMatrix[1][1] = uVector[1];
	rViewMatrix[1][2] = dVector[1];
	rViewMatrix[1][3] = 0.0F;
	rViewMatrix[2][0] = rVector[2];
	rViewMatrix[2][1] = uVector[2];
	rViewMatrix[2][2] = dVector[2];
	rViewMatrix[2][3] = 0.0F;
	rViewMatrix[3][0] = -rVector.Dot(eye);
	rViewMatrix[3][1] = -uVector.Dot(eye);
	rViewMatrix[3][2] = -dVector.Dot(eye);
	rViewMatrix[3][3] = 1.0F;
}

//----------------------------------------------------------------------------
void Renderer::OnViewportChange()
{
	if (!mspCamera)
	{
		return;
	}

	Float left;
	Float right;
	Float top;
	Float bottom;

	mspCamera->GetViewport(left, right, top, bottom);

	// DirectX defines the full-sized viewport to have origin at the upper
	// left corner of the screen. Wire uses the OpenGL convention that
	// 'bottom' specifies the relative distance from the bottom of the
	// screen. DirectX needs a specification of relative distance from the
	// top of the screen, which is 1 - 'top'.
	D3DVIEWPORT9 viewport;
	viewport.X = static_cast<DWORD>(left * mWidth);
	viewport.Y = static_cast<DWORD>((1.0F - top) * mHeight);// See note above.
	viewport.Width = static_cast<DWORD>((right - left) * mWidth);
	viewport.Height = static_cast<DWORD>((top - bottom) * mHeight);
	viewport.MinZ = 0.0F;
	viewport.MaxZ = 1.0F;

	HRESULT hr;
	hr = mpData->D3DDevice->SetViewport(&viewport);
	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void Renderer::SetClearColor(const ColorRGBA& rClearColor)
{
	mClearColor = rClearColor;
}

//----------------------------------------------------------------------------
void Renderer::Resize(UInt width, UInt height)
{
	WIRE_ASSERT(mpData);
	mWidth = width;
	mHeight = height;

	mpData->Present.BackBufferWidth = width;
	mpData->Present.BackBufferHeight = height;

	ResetDevice();
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
void DestroyNonManagedResources(THashTable<const Resource*,
	PdrResource*>& rMap, TArray<const Resource*>& rSave)
{
	rSave.SetMaxQuantity(rMap.GetQuantity(), false);
	const Resource* pKey;
	THashTable<const Resource*, PdrResource*>::Iterator it(&rMap);

	for (PdrResource** pValue = it.GetFirst(&pKey); pValue;
		pValue = it.GetNext(&pKey))
	{
		const Buffer* pBuffer = reinterpret_cast<const Buffer*>(pKey);
		if (PdrRendererData::POOLS[pBuffer->GetUsage()] != D3DPOOL_MANAGED)
		{
			rSave.Append(pKey);
		}
	}

	for (UInt i = 0; i < rSave.GetQuantity(); i++)
	{
		Renderer::UnbindAll(rSave[i]);
	}
}

//----------------------------------------------------------------------------
template <typename Resource, typename PdrResource>
void DestroyResources(THashTable<const Resource*, PdrResource*>& rMap,
	TArray<const Resource*>& rSave)
{
	rSave.SetMaxQuantity(rMap.GetQuantity(), false);
	const Resource* pKey;
	THashTable<const Resource*, PdrResource*>::Iterator it(&rMap);

	for (PdrResource** pValue = it.GetFirst(&pKey); pValue;
		pValue = it.GetNext(&pKey))
	{
		rSave.Append(pKey);
	}

	for (UInt i = 0; i < rSave.GetQuantity(); i++)
	{
		Renderer::UnbindAll(rSave[i]);
	}
}

//----------------------------------------------------------------------------
template <typename Resource>
void RecreateResources(Renderer* pRenderer, TArray<const Resource*>& rSave)
{
	for (UInt i = 0; i < rSave.GetQuantity(); i++)
	{
		pRenderer->Bind(rSave[i]);
	}
}

//----------------------------------------------------------------------------
void Renderer::ResetDevice()
{
	WIRE_ASSERT(mpData);
	TArray<const IndexBuffer*> saveIndexBuffers;
	TArray<const VertexBuffer*> saveVertexBuffers;
	TArray<const Image2D*> saveTexture2Ds;
	TArray<const RenderTarget*> saveRenderTargets;
	DestroyResources(mIndexBufferMap, saveIndexBuffers);
	DestroyResources(mVertexBufferMap, saveVertexBuffers);
	DestroyNonManagedResources(mImage2DMap, saveTexture2Ds);
	DestroyResources(mRenderTargetMap, saveRenderTargets);

	UInt batchingIBOSize = 0;
	if (mBatchedIndexBuffer)
	{
		batchingIBOSize = mBatchedIndexBuffer->GetBufferSize();
	}

	const UInt maxVertexStreamsToBatch = mBatchedVertexBuffers.GetQuantity();
	UInt batchingVBOSize = 0;
	if (mBatchedVertexBuffers.GetQuantity() > 0)
	{
		batchingVBOSize = mBatchedVertexBuffers[0]->GetBufferSize();
	}

	DestroyBatchingBuffers();

	HRESULT hr;
	IDirect3DDevice9* const pDevice = mpData->D3DDevice;
	hr = pDevice->Reset(&mpData->Present);
	WIRE_ASSERT(SUCCEEDED(hr));
	mpData->IsDeviceLost = false;

	CreateBatchingBuffers(batchingIBOSize, batchingVBOSize, maxVertexStreamsToBatch);

	RecreateResources(this, saveIndexBuffers);
	RecreateResources(this, saveVertexBuffers);
	RecreateResources(this, saveTexture2Ds);
	RecreateResources(this, saveRenderTargets);

	OnViewportChange();

	hr = pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	WIRE_ASSERT(SUCCEEDED(hr));

	hr = pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	WIRE_ASSERT(SUCCEEDED(hr));
	mpData->UsesRenormalizeNormals = false;
	mVertexFormatKey = 0;

	for (UInt i = 0; i < mLights.GetQuantity(); i++)
	{
		hr = pDevice->LightEnable(i, FALSE);
		WIRE_ASSERT(SUCCEEDED(hr));
	}

	mpData->AlphaState.IsValid = false;
	mpData->CullState.IsValid = false;
	mpData->FogState.IsValid = false;
	mpData->LightState.IsValid = false;
	mpData->WireframeState.IsValid = false;
	mpData->ZBufferState.IsValid = false;
	for (UInt i = 0; i < mpData->TextureStageStates.GetQuantity(); i++)
	{
		mpData->TextureStageStates[i].IsValid = false;
	}

	for (UInt i = 0; i < mpData->SamplerStates.GetQuantity(); i++)
	{
		mpData->SamplerStates[i].IsValid = false;
	}

	Set(mspStates);
}

//----------------------------------------------------------------------------
const DWORD PdrRendererData::BUFFER_LOCKING[Buffer::LM_QUANTITY] = 
{
	D3DLOCK_READONLY,           // Buffer::LM_READ_ONLY
	D3DLOCK_DISCARD,            // Buffer::LM_WRITE_ONLY
	0                           // Buffer::LM_READ_WRITE
};

//----------------------------------------------------------------------------
const D3DPOOL PdrRendererData::POOLS[Buffer::UT_QUANTITY] =
{
	D3DPOOL_MANAGED,	// Buffer::UT_STATIC
	D3DPOOL_DEFAULT,	// Buffer::UT_DYNAMIC
	D3DPOOL_DEFAULT,	// Buffer::UT_DYNAMIC_WRITE_ONLY
	D3DPOOL_MANAGED		// Buffer::UT_STATIC_DISCARD_ON_BIND
};

//----------------------------------------------------------------------------
const DWORD PdrRendererData::USAGES[Buffer::UT_QUANTITY] =
{
	D3DUSAGE_WRITEONLY,						// Buffer::UT_STATIC
	D3DUSAGE_DYNAMIC,						// Buffer::UT_DYNAMIC
	D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,	// Buffer::UT_DYNAMIC_WRITE_ONLY
	D3DUSAGE_WRITEONLY						// Buffer::UT_STATIC_DISCARD_ON_BIND
};

//----------------------------------------------------------------------------
PdrRendererData::PdrRendererData(Renderer* pRenderer)
	:
	D3D(NULL),
	D3DDevice(NULL),
	Supports32BitIndices(false),
	SupportsMinFAniso(false),
	SupportsMagFAniso(false),
	IsDeviceLost(false),
	UsesRenormalizeNormals(false),
	mpRenderer(pRenderer)
{
}
