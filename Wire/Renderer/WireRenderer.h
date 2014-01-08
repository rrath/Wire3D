// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRERENDERER_H
#define WIRERENDERER_H

#include "WireColorRGBA.h"
#include "WireMaterial.h"
#include "WireRendererStatistics.h"
#include "WireSmartPointer.h"
#include "WireStateAlpha.h"
#include "WireStateCull.h"
#include "WireStateFog.h"
#include "WireStateMaterial.h"
#include "WireStateWireframe.h"
#include "WireStateZBuffer.h"
#include "WireTHashTable.h"
#include "WireTransformation.h"

namespace Wire
{

class Camera;
class IndexBuffer;
class Light;
class Mesh;
class RenderObject;
class RenderTarget;
class Texture2D;
class VertexAttributes;
class VertexBuffer;
class VisibleSet;
// Opaque classes resolving to the platform-dependent resource (pdr) classes.
class PdrIndexBuffer;
class PdrRendererData;
class PdrRendererInput;
class PdrRenderTarget;
class PdrShader;
class PdrTexture2D;
class PdrVertexFormat;
class PdrVertexBuffer;

class Renderer
{
public:
	// draw a single object
	void Draw(const RenderObject* pRenderObject, const Transformation&
		rTransformation, Bool restoreState = false);
	// draw array of objects [min,max) (using batching if enabled)
	void Draw(RenderObject* const pVisible[], Transformation* const
		pTransformations[], UInt min, UInt max);
	// draw set(s) of objects supporting Wire::Effect
	void Draw(VisibleSet* rVisibleSet);
	void Draw(TArray<VisibleSet*>& rVisibleSets);

	// Support for predraw and postdraw semantics.
	Bool PreDraw(Camera* pCamera = NULL);
	void PostDraw();

	// Batching
	void CreateBatchingBuffers(UInt iboSize, UInt vboSize = 0,
		UInt maxVertexStreams = 5);
	inline Bool SupportsBatching() const;
	inline Bool UsesBatching() const;
	inline UInt GetStaticBatchingThreshold() const;
	inline void SetStaticBatchingThreshold(UInt maxTriangles);
	inline void GetDynamicBatchingThreshold(UInt& rMaxVertices,
		UInt& rMaxTriangles) const;
	inline void SetDynamicBatchingThreshold(UInt maxVertices,
		UInt maxTriangles);

	// Texture sampler functions
	inline Float GetMaxAnisotropy() const;
	inline UInt GetMaxTextureStages() const;
	inline UInt GetMaxTextureWidth() const;
	inline UInt GetMaxTextureHeight() const;

	inline UInt GetMaxVertexStreams() const;

	// PS major/minor, VS major/minor version number
	inline UInt GetShaderVersion() const;

	inline UInt GetMaxSimultaneousRenderTargets() const;

	// maximum fixed function lights supported by hardware
	inline UInt GetMaxLights() const;

	// Resource management.
	//
	// Bind:  Create a resource corresponding to the input object. The
	//    renderer maintains a mapping between the object and the resource.
	//    In most cases, video memory is allocated and a copy is made from
	//    the corresponding system memory of the object. If Enable is called
	//    before Bind, the renderer silently creates a resource rather than
	//    cause an exception.
	//
	// BindAll:  Create the resources corresponding to the input object for
	//    all renderers in the system.
	//
	// Unbind:  Destroy the resource corresponding to the input object. The
	//    renderer removes the object-resource pair from the mapping. The
	//    object maintains its system memory copy and properties, so it can
	//    always be bound again.
	//
	// UnbindAll:  Destroy the resources corresponding to the input object
	//    for all renderers that created a resource from the object. This
	//    function is called in the destructors for the objects.
	//
	// Enable:  The resource is made active during the current draw call but
	//    before the DrawElements call is made.
	//
	// Disable:  The resource is made inactive during the current draw call
	//    but after the DrawElements call is made.
	//
	// Set:  The resource is only enabled, if it is inactive. This is used
	//    instead of Enable/Disable pairs to avoid redundant resource
	//    activation. However changing activated input objects between Set
	//    calls do not propagate their change.
	//
	// Update:  Lock the video memory of the resource, copy the system memory
	//    contents to it, and then unlock the video memory. This is the
	//    recommended way for updating resources (update the system memory and
	//    then call Update).

	template <typename Resource>
	static void UnbindAll(const Resource* pResource);

	// Index buffer management
	void Bind(const IndexBuffer* pIndexBuffer);
	void Unbind(const IndexBuffer* pIndexBuffer);
	void Enable(const IndexBuffer* pIndexBuffer);
	void Disable(const IndexBuffer* pIndexBuffer);
	void Set(const IndexBuffer* pIndexBuffer);
	void Update(const IndexBuffer* pIndexBuffer);
	void Update(const IndexBuffer* pIndexBuffer, UInt count, UInt offset = 0);

	// Vertex buffer management
	void Bind(const VertexBuffer* pVertexBuffer);
	void Unbind(const VertexBuffer* pVertexBuffer);
	void Enable(const VertexBuffer* pVertexBuffer, UInt streamIndex);
	void Disable(const VertexBuffer* pVertexBuffer, UInt streamIndex);
	void Set(const VertexBuffer* pIndexBuffer, UInt streamIndex);
	void Update(const VertexBuffer* pVertexBuffer);
	void Update(const VertexBuffer* pVertexBuffer, UInt count,
		UInt offset = 0);

	// Vertex format management
	typedef TArray<Pointer<VertexBuffer> > VertexBuffers;
	void Bind(const VertexBuffers& rVertexBuffers);
	void Enable(const VertexBuffers& rVertexBuffers);
	void Disable(const VertexBuffers& rVertexBuffers);
	void Set(const VertexBuffers& rVertexBuffers);

	// 2D texture management
	void Bind(const Image2D* pImage);
	void Unbind(const Image2D* pImage);
	void Enable(const Texture2D* pTexture, UInt unit = 0);
	void Disable(const Texture2D* pTexture, UInt unit = 0);
	void Set(const Texture2D* pTexture, UInt unit = 0);

	// Shader management
	void Bind(const Shader* pShader);
	void Unbind(const Shader* pShader);
	void Enable(const Shader* pShader, Pointer<Shader>& rInUse);
	void Disable(const Shader* pShader, Pointer<Shader>& rInUse);
	void Set(const Shader* pShader, Pointer<Shader>& rInUse);

	// render target management
	void Bind(const RenderTarget* pRenderTarget);
	void Unbind(const RenderTarget* pRenderTarget);
	void Enable(const RenderTarget* pRenderTarget);
	void Disable(const RenderTarget* pRenderTarget);
	void Set(const RenderTarget* pRenderTarget);
	// The render target must be disabled when ReadColor is called.
	// It returns a copy of the render target color video memory.
	Image2D* ReadColor(UInt i, const RenderTarget* pRenderTarget);


	// Release all references to non renderer owned resources (disables all
	// actively used resources in the renderer).
	void ReleaseResources();

	// Access to the current camera state.
	void SetCamera(Camera* pCamera);
	Camera* GetCamera() const;

	// Apply camera changes to platform specific renderer.
	void OnFrameChange();
	void OnViewportChange();

	// Handle window resize.
	void Resize(UInt width, UInt height);

	// Backbuffer functions
	inline UInt GetWidth() const;
	inline UInt GetHeight() const;
	inline const ColorRGBA& GetClearColor() const;
	void SetClearColor(const ColorRGBA& rClearColor);
	void ClearBuffers();
	void DisplayBackBuffer();

	// Immediate render state handling
	void SetState(StateAlpha* pState);
	void SetState(StateCull* pState);
	void SetState(StateFog* pState);
	void SetState(StateMaterial* pState);
	void SetState(StateWireframe* pState);
	void SetState(StateZBuffer* pState);
	inline const StateAlpha* GetStateAlpha() const;
	inline const StateCull* GetStateCull() const;
	inline const StateFog* GetStateFog() const;
	inline const StateMaterial* GetStateMaterial() const;
	inline const StateWireframe* GetStateWireframe() const;
	inline const StateZBuffer* GetStateZBuffer() const;
	// Renderer render state handling
	void Set(const StatePtr spStates[]);
	void Enable(const StatePtr spStates[]);
	void Disable(const StatePtr spStates[]);

	// Material management
	typedef TArray<Pointer<Light> > Lights;
	void Enable(const Material* pMaterial, const Lights* pLights = NULL);
	void Disable(const Material* pMaterial, const Lights* pLights = NULL);
	void Set(const Material* pMaterial, const Lights* pLights = NULL);

	// Mesh management
	void Bind(const Mesh* pMesh);
	void Unbind(const Mesh* pMesh);
	void Enable(const Mesh* pMesh);
	void Disable(const Mesh* pMesh);
	void Set(const Mesh* pMesh);

	// Fixed function immediate light state handling
	void SetLight(const Light* pLight, UInt unit = 0);
	void EnableLighting(const ColorRGB& rAmbient = ColorRGB::BLACK);
	void DisableLighting();

	// Renderer light state handling
	void Enable(const Lights* pLights);
	void Disable(const Lights* pLights);
	void Set(const Lights* pLights);

	// Bind/Unbind all resources of a RenderObject
	void Bind(const RenderObject* pRenderObject);
	void Unbind(const RenderObject* pRenderObject);

	// Draw call and bound resources' memory statistics
	inline RendererStatistics* GetStatistics();

	// The platform-dependent constructor must call this function first before
	// doing any of its own work.
	void Initialize(UInt width, UInt height);

	// The platform-dependent destructor must call this function after doing
	// any of its own work.
	void Terminate();

	Renderer(PdrRendererInput& rInput, UInt width, UInt height,
		Bool isFullscreen, Bool useVSync);
	~Renderer();

private:
	// Fixed function texture stage management
	void EnableTextureStage(Material::BlendMode blendMode, UInt unit = 0,
		Bool hasAlpha = true);
	void DisableTextureStage(UInt unit = 0);

	void SetShaderVariables();
	void SetShaderVariables(PdrShader* pPdrShader, const ShaderVariables*
		pShaderVars);

	void SetTransformation(const Transformation& rWorld,
		Bool processNormals, Shader* pVertexShader);
	UInt GetVertexFormatKey(const VertexBuffers& rVertexBuffers);

	void DrawStaticBatches(RenderObject* const pVisible[], Transformation*
		const pTransformations[], UInt min, UInt max);
	void DrawDynamicBatches(RenderObject* const pVisible[], Transformation*
		const pTransformations[], UInt min, UInt max);
	void DrawDynamicBatch(const Mesh* pMesh, PdrIndexBuffer* const pIBPdr,
		TArray<PdrVertexBuffer*>& rVBsPdr, UInt vertexCount,
		UInt indexCount);
	void DrawBatch(PdrIndexBuffer* const pIBPdr, UInt vertexCount,
		UInt indexCount, UShort minIndex);

	// The main entry point to drawing in the derived-class renderers
	void DrawElements(UInt vertexCount, UInt indexCount, UInt startIndex,
		UInt startVertex = 0);

	void ResetDevice();

	// Resource management.
	typedef THashTable<const IndexBuffer*, PdrIndexBuffer*> IndexBufferMap;
	typedef THashTable<const VertexBuffer*, PdrVertexBuffer*> VertexBufferMap;
	typedef THashTable<const Image2D*, PdrTexture2D*> Image2DMap;
	typedef THashTable<const Shader*, PdrShader*> ShaderMap;
	typedef THashTable<const RenderTarget*, PdrRenderTarget*> RenderTargetMap;
	typedef THashTable<UInt, PdrVertexFormat*> VertexFormatMap;

	template <typename Resource, typename PdrResource>
	inline Bool Bind(const Resource* pResource, THashTable<const Resource*,
		PdrResource*>& rMap, UInt* pCount, UInt* pSize);

	template <typename Resource, typename PdrResource>
	inline void Unbind(const Resource* pResource, THashTable<const Resource*,
		PdrResource*>& rMap, UInt* pCount, UInt* pSize);

	template <typename Resource, typename PdrResource>
	inline void Enable(const Resource* pResource, THashTable<const Resource*,
		PdrResource*>& rMap);

	template <typename Resource, typename PdrResource>
	inline void Disable(const Resource* pResource, THashTable<const Resource*,
		PdrResource*>& rMap);

	template <typename Resource, typename PdrResource>
	inline void Disable(const Resource* pResource, UInt index, THashTable<
		const Resource*, PdrResource*>& rMap);

	template <typename PdrResource>
	inline void Disable(const UInt key, THashTable<UInt, PdrResource*>& rMap);

	template <typename Resource>
	inline void Set(const Resource* pResource, Pointer<Resource> spInUse);

	template <typename Resource>
	inline void Set(const Resource* pResource, UInt index, TArray<Pointer<
		Resource> >& rInUse);

	template <typename Resource, typename PdrResource>
	inline void Update(const Resource* pResource, THashTable<const Resource*,
		PdrResource*>& rMap);

	template <typename Resource, typename PdrResource>
	inline void Update(const Resource* pResource, THashTable<const Resource*,
		PdrResource*>& rMap, UInt count, UInt offset = 0);

	// Support for destructor. Destroy any remaining resources that the
	// application did not explicitly release.
	template <typename Resource, typename PdrResource>
	void DestroyAll(THashTable<const Resource*, PdrResource*>& rMap);
	void DestroyAll(VertexFormatMap& rVertexFormatMap);

	void DestroyBatchingBuffers();

	IndexBufferMap mIndexBufferMap;
	VertexBufferMap mVertexBufferMap;
	Image2DMap mImage2DMap;
	ShaderMap mShaderMap;
	RenderTargetMap mRenderTargetMap;
	VertexFormatMap mVertexFormatMap;

	// Objects currently in use by the Renderer
	StatePtr mspStates[State::MAX_STATE_TYPE];
	Pointer<IndexBuffer> mspIndexBuffer;
	Pointer<Material> mspMaterial;
	Pointer<Mesh> mspMesh;
	VertexBuffers mVertexBuffers;
	TArray<TArray<UInt> > mVertexFormatKeys;
	TArray<Pointer<Texture2D> > mTexture2Ds;
	Pointer<Shader> mspPixelShader;
	Pointer<Shader> mspVertexShader;
	Pointer<RenderTarget> mspRenderTarget;
	Lights mLights;
	UInt mVertexFormatKey;

	// The camera for establishing the view frustum
	Pointer<Camera> mspCamera;

	// The color used for clearing the backbuffer
	ColorRGBA mClearColor;

	// Width and height of the backbuffer
	UInt mWidth;
	UInt mHeight;

	// Maximum anisotropy value supported for texture filtering by the device
	Float mMaxAnisotropy;
	UInt mMaxTextureWidth;
	UInt mMaxTextureHeight;

	// PS major/minor, VS major/minor version number
	UInt mShaderVersion;

	UInt mMaxSimultaneousRenderTargets;

	static Renderer* s_pRenderer;

	// Batching
	Transformation mIdentityNonRS; // xformed batching needs to renormalize
	PdrIndexBuffer* mBatchedIndexBuffer;
	TArray<PdrVertexBuffer*> mBatchedVertexBuffers;
	TArray<void*> mRawBatchedVertexBuffers;
	UInt mStaticBatchingMaxIndexCount;
	UInt mDynamicBatchingMaxVertexCount;
	UInt mDynamicBatchingMaxIndexCount;
	Bool mSupportsBatching;

	RendererStatistics mStatistics;

public:
	// The platform-specific data. It is in public scope to allow the
	// renderer resource classes to access it.
	inline PdrRendererData* GetRendererData() const;

	PdrIndexBuffer* GetResource(const IndexBuffer* pIndexBuffer);
	PdrVertexBuffer* GetResource(const VertexBuffer* pVertexBuffer);
	PdrVertexFormat* GetResource(const VertexBuffers& rVertexBuffers);
	PdrTexture2D* GetResource(const Image2D* pTexture);
	PdrShader* GetResource(const Shader* pShader);
	PdrRenderTarget* GetResource(const RenderTarget* pRenderTarget);

	void InsertInImage2DMap(const Image2D* pImage, PdrTexture2D* pPdrTexture); 

private:
	template <typename Resource, typename PdrResource>
	inline PdrResource* GetResource(const Resource* pResource, THashTable<
		const Resource*, PdrResource*>& rMap);

	PdrRendererData* mpData;
};

#include "WireRenderer.inl"

}

#endif
