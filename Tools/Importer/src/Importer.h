#pragma once
#ifndef IMPORTER_H
#define IMPORTER_H

#include "rapidxml.hpp"
#include "WireCamera.h"
#include "WireColorRGBA.h"
#include "WireMaterial.h"
#include "WireMesh.h"
#include "WireNode.h"
#include "WireNodeSkybox.h"
#include "WireRenderText.h"
#include "WireString.h"
#include "WireVector2.h"
#include "WireVertexBuffer.h"
#include "WireIndexBuffer.h"

#ifndef NO_BULLET_PHYSICS_LIB
#include "PhysicsWorld.h"
#endif

class Importer
{

public:
	struct Options
	{
		Options()
			:
			AssetsWithEqualNamesAreIdentical(true),
			PrepareSceneForBatching(false),
			CreateInterleavedVertexBuffers(false)
			{}
			 
		Bool AssetsWithEqualNamesAreIdentical;
		Bool PrepareSceneForBatching;
		Bool CreateInterleavedVertexBuffers;
	};

	struct SceneOptions
	{
		SceneOptions()
			:
			Gravity(0, -9.81F, 0)
			{}

		Wire::Vector3F Gravity;
	};

	struct Statistics
	{
		UInt RenderObjectCount;
		UInt NodeCount;
		UInt TextureCount;
		UInt MaterialCount;
		UInt MeshCount;
		UInt VertexBufferCount;
		UInt IndexBufferCount;
		UInt ColliderCount;	 // btCollisionShape excluding btEmptyShape
		UInt RigidBodyCount; // btRigidBody
	};

	Importer(const Char* pPath = "", Options* pOptions = NULL);

#ifndef NO_BULLET_PHYSICS_LIB
	Wire::Node* LoadSceneFromXml(const Char* pFilename, PhysicsWorld* pPhysicsWorld = NULL);
#else
	Wire::Node* LoadSceneFromXml(const Char* pFilename);
#endif

	static Wire::Image2D* DecodePNG(const UChar* pPngInMem, size_t pngSize, Bool filterMipmaps,
		Wire::Buffer::UsageType usage = Wire::Buffer::UT_STATIC, UInt mipmapCount = 0);
	static Wire::Image2D* LoadPNG(const Char* pFilename, Bool filterMipmaps,
		Wire::Buffer::UsageType usage = Wire::Buffer::UT_STATIC, UInt mipmapCount = 0);
	static Wire::Texture2D* LoadTexture2D(const Char* pFilename, Bool filterMipmaps,
		Wire::Buffer::UsageType usage = Wire::Buffer::UT_STATIC, UInt mipmapCount = 0);
	static Wire::RenderText* CreateText(const Char* pFilename, UInt width,
		UInt height, UInt maxLength = 4000);

	const Statistics* GetStatistics();
	void ApplySceneOptions(PhysicsWorld* pPhysicsWorld = NULL);

private:
	static Char* Load(const Char* pFilename, Int& rSize);

	Float* Load32(const Char* pFilename, Int& rSize, Bool isBigEndian);
	UShort* Load16(const Char* pFilename, Int& rSize, Bool isBigEndian);
	void Free32(Float* pFloats);

	void Traverse(rapidxml::xml_node<>* pXmlNode, Wire::Node* pParent);
	Char* GetValue(rapidxml::xml_node<>* pXmlNode, const Char* pName);
	Bool HasValue(rapidxml::xml_node<>* pXmlNode, const Char* pName);
	Float GetFloat(rapidxml::xml_node<>* pXmlNode, const Char* pName);
	void GetFloat(rapidxml::xml_node<>* pXmlNode, const Char* pName, Float& rFloat);
	void GetHex(rapidxml::xml_node<>* pXmlNode, const Char* pName, UInt& rHex);
	Int GetInt(rapidxml::xml_node<>* pXmlNode, const Char* pName);
	UInt GetUInt(rapidxml::xml_node<>* pXmlNode, const Char* pName);
	Bool GetBool(rapidxml::xml_node<>* pXmlNode, const Char* pName);
	void GetBool(rapidxml::xml_node<>* pXmlNode, const Char* pName, Bool& rBool);
	Wire::Vector3F GetVector3(rapidxml::xml_node<>* pXmlNode, const Char* pName);
	void GetVector3(rapidxml::xml_node<>* pXmlNode, const Char* pName, Wire::Vector3F& rV);
	Wire::ColorRGB GetColorRGB(rapidxml::xml_node<>* pXmlNode,
		const Char* pName, Bool& rHasValue);
	Wire::ColorRGBA GetColorRGBA(rapidxml::xml_node<>* pXmlNode,
		const Char* pName, Bool& rHasValue);
	Bool IsTrue(const Char* pName, rapidxml::xml_node<>* pXmlNode);
	Wire::Buffer::UsageType GetUsageType(rapidxml::xml_node<>* pXmlNode);
	Bool Is(const Char*, const Char*);

	void ParseAssets(rapidxml::xml_node<>* pXmlNode);
	void ParseOptions(rapidxml::xml_node<>* pXmlNode);
	Wire::Node* ParseNode(rapidxml::xml_node<>* pXmlNode, Wire::Node* pParent);
	Wire::RenderObject* ParseRenderObject(rapidxml::xml_node<>* pXmlNode);
	Wire::Node* ParseText(rapidxml::xml_node<>* pXmlNode);
	Wire::NodeSkybox* ParseSkybox(rapidxml::xml_node<>* pXmlNode);
	Wire::Texture2D* ParseSkyboxTexture(const Char* pName, rapidxml::xml_node<>* pXmlNode);
	Wire::Mesh* ParseMesh(rapidxml::xml_node<>* pXmlNode);
	Wire::VertexBuffer* ParseVertexBuffer(rapidxml::xml_node<>* pXmlNode);
	void ParseVertexBuffers(rapidxml::xml_node<>* pXmlNode, Wire::TArray<
		Wire::VertexBufferPtr>& rVertexBuffers);
	Wire::IndexBuffer* ParseIndexBuffer(rapidxml::xml_node<>* pXmlNode);
	Wire::Material* ParseMaterial(rapidxml::xml_node<>* pXmlNode);
	Wire::Texture2D* ParseTexture(rapidxml::xml_node<>* pXmlNode, Wire::Material::BlendMode& blendMode);
	void ParseTransformation(rapidxml::xml_node<>* pXmlNode, Wire::Spatial* pSpatial);
	void ParseComponent(rapidxml::xml_node<>* pXmlNode, Wire::Spatial* pSpatial);
	void ParseComponents(rapidxml::xml_node<>* pXmlNode, Wire::Spatial* pSpatial);
	void ParseCamera(rapidxml::xml_node<>* pXmlNode, Wire::Spatial* pSpatial);
	Wire::Light* ParseLight(rapidxml::xml_node<>* pXmlNode);
	Wire::State* ParseRenderStates(rapidxml::xml_node<>* pXmlNode);
	void ParseTransformationAndComponents(rapidxml::xml_node<>* pXmlNode,
		Wire::Spatial* pSpatial);

	void UpdateWorldTransformation(Wire::Spatial* pSpatial);
	void ResetStatistics();

	Wire::IndexBuffer* LoadIndexBufferFromFile(Char* pFileName, Bool isIndexBufferBigEndian,
		Wire::Buffer::UsageType indexBufferUsage, Bool is16Bit);
	Wire::VertexBuffer* LoadVertexBufferFromFiles(Char* pFileName, Bool isVertexBufferBigEndian,
		Wire::Buffer::UsageType vertexBufferUsage, Char* pNormalsName, Bool isNormalsBigEndian,
		Char* pColorsName, Bool isColorsBigEndian, Bool isColors32Bit,
		Wire::TArray<Char*>& rUvSetNames, Wire::TArray<Bool>& rUvBigEndian);
	Wire::VertexBuffer* LoadVertexBuffer(rapidxml::xml_node<>* pXmlNode,
		Wire::VertexAttributes& rAttributes);

	const Char* mpPath;

	Wire::THashTable<Wire::String, Wire::MaterialPtr> mMaterials;
	Wire::THashTable<Wire::Material*, Wire::TArray<Wire::StatePtr> > mMaterialStates;
	Wire::THashTable<Wire::String, Wire::TArray<Wire::MeshPtr> > mMeshes;
	Wire::THashTable<Wire::String, Wire::Texture2DPtr> mTextures; // TODO: use Image2D
	Wire::THashTable<Wire::String, Wire::LightPtr> mLights;
	Wire::TArray<Wire::SpatialPtr> mStaticSpatials;

	Statistics mStatistics;
	Options mDefaultOptions;
	Options* mpOptions;
	SceneOptions mSceneOptions;

#ifndef NO_BULLET_PHYSICS_LIB
	struct RigidBodyInfo
	{
		RigidBodyInfo()	: Mass(0), Damping(0), AngularDamping(0.5F), IsKinematic(false),
			IsEnabled(true) {}

		Float Mass;
		Float Damping;
		Float AngularDamping;
		Bool IsKinematic;
		Bool IsEnabled;
	};

	void ParseCollider(rapidxml::xml_node<>* pXmlNode, Wire::Spatial* pSpatial);
	void ParseRigidBody(rapidxml::xml_node<>* pXmlNode, Wire::Spatial* pSpatial);
	void ParseRigidBodyInfo(RigidBodyInfo& rInfo, rapidxml::xml_node<>* pXmlNode);
	void AddRigidBodyController(Wire::Spatial* pSpatial, btCollisionShape* pCollisionShape,
		RigidBodyInfo& rInfo, const Wire::Vector3F& rCenter = Wire::Vector3F::ZERO,
		Wire::VertexBuffer* pVBRef = NULL, Wire::IndexBuffer* pIBRef = NULL);
	btTransform GetBtTransform(Wire::Spatial* pSpatial, const Wire::Vector3F& rCenter);

	PhysicsWorldPtr mspPhysicsWorld;
#endif
};

#endif
