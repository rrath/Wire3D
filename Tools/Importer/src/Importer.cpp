#include "Importer.h"

#include "PicoPNG.h"

#ifndef NO_FREETYPE2_LIB
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

#include "WireEffect.h"
#include "WireImage2D.h"
#include "WireLight.h"
#include "WireMesh.h"
#include "WireNodeCamera.h"
#include "WireNodeLight.h"
#include "WireQuaternion.h"
#include "WireStandardMesh.h"
#include "WireStateAlpha.h"
#include "WireStateCull.h"
#include "WireStateFog.h"
#include "WireStateMaterial.h"
#include "WireStateWireframe.h"
#include "WireStateZBuffer.h"
#include "WireTStack.h"
#include "WireVertexAttributes.h"

using namespace Wire;

//----------------------------------------------------------------------------
Importer::Importer(const Char* pPath, Options* pOptions)
	:
	mpPath(pPath),
	mStaticSpatials(0, 100)
{
	mpOptions = pOptions ? pOptions : &mDefaultOptions;
}

//----------------------------------------------------------------------------
#ifndef NO_BULLET_PHYSICS_LIB
Node* Importer::LoadSceneFromXml(const Char* pFilename, PhysicsWorld* pPhysicsWorld)
{
	mspPhysicsWorld = pPhysicsWorld;
#else
Node* Importer::LoadSceneFromXml(const Char* pFilename)
{
#endif
	ResetStatistics();

	String path = String(mpPath) + String(pFilename);
	Int xmlSize;
	Char* pXml = Load(static_cast<const Char*>(path), xmlSize);
	if (!pXml)
	{
		return NULL;
	}

	Char* pXmlNullTerminated = WIRE_NEW Char[xmlSize+1];
	System::Memcpy(pXmlNullTerminated, xmlSize, pXml, xmlSize);
	pXmlNullTerminated[xmlSize] = 0;
	WIRE_DELETE[] pXml;

	rapidxml::xml_document<> doc;    // character type defaults to char
	doc.parse<0>(pXmlNullTerminated);
	Node* pRoot = WIRE_NEW Node;
	pRoot->SetName(pFilename);

	if (doc.first_node())
	{
		for (rapidxml::xml_node<>* pChild = doc.first_node(); pChild;
			pChild = pChild->next_sibling())
		{
			Traverse(pChild, pRoot);
		}
	}

	WIRE_DELETE[] pXmlNullTerminated;

	mMaterials.RemoveAll();
	mMaterialStates.RemoveAll();
	mMeshes.RemoveAll();
	mTextures.RemoveAll();
	mLights.RemoveAll();

	pRoot->UpdateGS(-MathD::MAX_REAL, true, false);
	pRoot->UpdateRS();

	if (mStaticSpatials.GetQuantity() > 0)
	{
		for (UInt i = 0; i < mStaticSpatials.GetQuantity(); i++)
		{
			WIRE_ASSERT(mStaticSpatials[i]);
			WIRE_ASSERT(mStaticSpatials[i]->WorldIsCurrent);
			Node* pStaticNode = DynamicCast<Node>(mStaticSpatials[i]);
			if (pStaticNode)
			{
				if (pStaticNode->GetQuantity() == 0)
				{
					mStaticSpatials[i]->WorldBoundIsCurrent = true;
				}
			}
		}
	}

	mStaticSpatials.RemoveAll();

	if (mpOptions->PrepareSceneForBatching)
	{
		pRoot->PrepareForStaticBatching(false, true);
	}

	return pRoot;
}

//----------------------------------------------------------------------------
Image2D* Importer::LoadPNG(const Char* pFilename, Bool filterMipmaps,
	Buffer::UsageType usage, UInt mipmapCount)
{
	Int pngSize;
	UChar* pPNG = reinterpret_cast<UChar*>(Load(pFilename, pngSize));
	if (!pPNG)
	{
		return NULL;
	}

	Image2D* pImage2D = DecodePNG(pPNG, pngSize, filterMipmaps, usage,
		mipmapCount);
	WIRE_DELETE[] pPNG;

	return pImage2D;
}

//----------------------------------------------------------------------------
Texture2D* Importer::LoadTexture2D(const Char* pFilename, Bool filterMipmaps,
	Buffer::UsageType usage, UInt mipmapCount)
{
	Image2D* pImage = LoadPNG(pFilename, filterMipmaps, usage, mipmapCount);
	if (!pImage)
	{
		return NULL;
	}

	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	WIRE_ASSERT(pTexture);
	return pTexture;
}

//----------------------------------------------------------------------------
Image2D* Importer::DecodePNG(const UChar* pPNG, size_t pngSize,
	Bool filterMipmaps, Buffer::UsageType usage, UInt mipmapCount)
{
	std::vector<UChar> rawImage;
	ULong width;
	ULong height;
	PicoPNG::decodePNG(rawImage, width, height, pPNG, pngSize, false);

	Bool hasAlpha = (rawImage.size() / (height*width)) == 4;
	Image2D::FormatMode format = hasAlpha ? Image2D::FM_RGBA8888 :
		Image2D::FM_RGB888;

	size_t size = Image2D::GetBytesPerPixel(format) * width*height;
	UChar* pDst = WIRE_NEW UChar[size];
	System::Memcpy(pDst, size, &(rawImage[0]), size);

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pDst,
		filterMipmaps, usage, mipmapCount);

	return pImage;
}

//----------------------------------------------------------------------------
RenderText* Importer::CreateText(const Char* pFilename, UInt width, UInt height,
	UInt maxLength)
{
#ifndef NO_FREETYPE2_LIB
	// Init FreeType lib and font
	Int fileSize;
	UChar* pMemFile = reinterpret_cast<UChar*>(Load(pFilename, fileSize));
	if (!pMemFile)
	{
		return NULL;
	}

	FT_Library library;
	
	FT_Error error = FT_Init_FreeType(&library);
	if (error != 0)
	{
		WIRE_ASSERT(false /* Could not initialize freetype lib */);
		WIRE_DELETE[] pMemFile;
		return NULL;
	}

	FT_Face face;
	error = FT_New_Memory_Face(library, pMemFile, fileSize, 0, &face);
	if (error)
	{
		if (error == FT_Err_Unknown_File_Format)
		{
			WIRE_ASSERT(false /* Font format is not supported */);
		}
		else
		{
			WIRE_ASSERT(false /* Could not create face from memory */);
		}

		FT_Done_FreeType(library);
		WIRE_DELETE[] pMemFile;
		return NULL;
	}

	error = FT_Set_Pixel_Sizes(face, width, height);
	if (error)
	{
		// Usually, an error occurs with a fixed-size font format (like FNT or
		// PCF) when trying to set the pixel size to a value that is
		// not listed in the face->fixed_sizes array
		WIRE_ASSERT(false /* Could not set pixel size */);
		FT_Done_Face(face);
		FT_Done_FreeType(library);
		WIRE_DELETE[] pMemFile;
		return NULL;
	}

	// Calculate the required font texture size from the font size
	const UInt totalCharCount = 128;
	UInt texWidth = 32;
	UInt texHeight = 32;
	FT_GlyphSlot slot = face->glyph;

	Bool fitsInTexture = false;
	while (!fitsInTexture)
	{
		if (texWidth > 1024 || texHeight > 1024)
		{
			return NULL;
		}

		UInt wc = 0;
		Int penX = 0;
		Int penY = height;
		fitsInTexture = true;

		for (UInt i = 0; i < totalCharCount; i++)
		{
			FT_UInt glyphIndex = FT_Get_Char_Index(face, wc++);
			if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER))
			{
				continue;
			}

			Int advance = slot->advance.x >> 6;
			advance = advance > slot->bitmap.width ? advance : slot->bitmap.width;
			if (penX+advance >= static_cast<Int>(texWidth))
			{
				penX = 0;
				penY += height;
				if (penY+height >= texHeight)
				{
					if (texWidth > texHeight)
					{
						texHeight = texHeight << 1;
					}
					else
					{
						texWidth = texWidth << 1;
					}

					fitsInTexture = false;
					break;
				}
			}

			penX += advance;
		}
	}

	UChar* const pDst = WIRE_NEW UChar[texWidth * texHeight * 4];
	System::Memset(pDst, 0, texWidth * texHeight * 4);
	TArray<Vector2F> uvs(totalCharCount*4);
	TArray<Vector4F> charSizes(totalCharCount);

	Int penY = height;
	Int penX = 0;
	for (UInt wc = 0; wc < totalCharCount; wc++)
	{
		FT_UInt glyphIndex = FT_Get_Char_Index(face, wc);
		if (FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER))
		{
			uvs.Append(Vector2F(0, 0));
			uvs.Append(Vector2F(1, 0));
			uvs.Append(Vector2F(1, 1));
			uvs.Append(Vector2F(0, 1));
			Float fWidth = static_cast<Float>(width);
			Float fHeight = static_cast<Float>(height);
			charSizes.Append(Vector4F(fWidth, fHeight, fWidth, 0));
			continue;
		}

		Int advance = slot->advance.x >> 6;
		advance = advance > slot->bitmap.width ? advance : slot->bitmap.width;
		if (penX+advance >= static_cast<Int>(texWidth))
		{
			penX = 0;
			penY += height;
			if (penY+height >= texHeight)
			{
				if (texWidth > texHeight)
				{
					texHeight = texHeight << 1;
				}
				else
				{
					texWidth = texWidth << 1;
				}

				fitsInTexture = false;
				break;
			}
		}

		Int offset = penX + slot->bitmap_left;
		Int top = penY - slot->bitmap_top;
		FT_Bitmap& rBitmap = slot->bitmap;

		Float u0 = static_cast<Float>(offset)/texWidth;
		Float v0 = static_cast<Float>(top)/texHeight;
		Float u1 = static_cast<Float>(offset+rBitmap.width)/texWidth;
		Float v1 = static_cast<Float>(top+rBitmap.rows)/texHeight;
		uvs.Append(Vector2F(u0, v0));
		uvs.Append(Vector2F(u1, v0));
		uvs.Append(Vector2F(u1, v1));
		uvs.Append(Vector2F(u0, v1));

		Float cWidth = static_cast<Float>(rBitmap.width);
		Float cHeight = static_cast<Float>(rBitmap.rows);
		Float cStride = static_cast<Float>(slot->advance.x >> 6);
		if (wc == '.' || wc == ',' || wc == ':' || wc == ';')
		{
			cStride = static_cast<Float>(slot->bitmap.width+1);
		}

		Float cOffsetY = static_cast<Float>(slot->bitmap.rows - slot->bitmap_top);
		charSizes.Append(Vector4F(cWidth, cHeight, cStride, cOffsetY));

		Int q = 0;
		for (Int j = top; j < (top + rBitmap.rows); j++, q++)
		{
			Int p = 0;
			for (Int i = offset; i < (offset + rBitmap.width); i++, p++)
			{
				UChar pixel = rBitmap.buffer[q*rBitmap.width + p];
				WIRE_ASSERT(((j*texWidth + i) < (texWidth * texHeight)) && ((j*texWidth + i) >= 0));
				pDst[(j*texWidth + i)*4] = 0xFF;
				pDst[(j*texWidth + i)*4+1] = 0xFF;
				pDst[(j*texWidth + i)*4+2] = 0xFF;
				pDst[(j*texWidth + i)*4+3] = pixel;
			}
		}

		penX += advance;
		WIRE_ASSERT((slot->advance.y >> 6) <= static_cast<Int>(height));
	}

	Image2D* pImage = WIRE_NEW Image2D(Image2D::FM_RGBA8888, texWidth,
		texHeight, pDst, false, Buffer::UT_STATIC, 1);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	pTexture->SetFilterType(Texture2D::FT_NEAREST);
 	RenderText* pText = WIRE_NEW RenderText(height, pTexture, uvs, charSizes, maxLength);

	FT_Done_Face(face);
	FT_Done_FreeType(library);
	WIRE_DELETE[] pMemFile;
#else
	RenderText* pText = StandardMesh::CreateText(maxLength);
#endif

	return pText;
}

//----------------------------------------------------------------------------
Char* Importer::Load(const Char* pFilename, Int& rSize)
{
	Char* pBuffer;
	Bool hasSucceeded;
	hasSucceeded = System::Load(pFilename, pBuffer, rSize);
	if (!hasSucceeded)
	{
		WIRE_ASSERT(false /* Could not load file */);
		return NULL;
	}

	return pBuffer;
}

//----------------------------------------------------------------------------
Float* Importer::Load32(const Char* pFilename, Int& rSize, Bool isBigEndian)
{
	WIRE_ASSERT(pFilename);
	if (!pFilename)
	{
		return NULL;
	}

	String path = String(mpPath) + String(pFilename);
	Char* pBuffer = Load(static_cast<const Char*>(path), rSize);
	if (!pBuffer)
	{
		return NULL;
	}

	Float* pBuffer32 = reinterpret_cast<Float*>(pBuffer);
	if (System::IsBigEndian() != isBigEndian)
	{
		if (!pBuffer32 || (rSize & 3) != 0)
		{
			WIRE_ASSERT(false);
			return NULL;
		}

		for (Int i = 0; i < rSize; i+=4)
		{
			UChar i0 = pBuffer[i+0];
			pBuffer[i+0] = pBuffer[i+3];
			pBuffer[i+3] = i0;
			i0 = pBuffer[i+1];
			pBuffer[i+1] = pBuffer[i+2];
			pBuffer[i+2] = i0;
		}
	}

	return pBuffer32;
}

//----------------------------------------------------------------------------
UShort* Importer::Load16(const Char* pFilename, Int& rSize, Bool isBigEndian)
{
	WIRE_ASSERT(pFilename);
	if (!pFilename)
	{
		return NULL;
	}

	String path = String(mpPath) + String(pFilename);
	Char* pBuffer = Load(static_cast<const Char*>(path), rSize);
	if (!pBuffer)
	{
		return NULL;
	}

	UShort* pBuffer16 = reinterpret_cast<UShort*>(pBuffer);
	if (System::IsBigEndian() != isBigEndian)
	{
		if (!pBuffer16 || (rSize & 1) != 0)
		{
			WIRE_ASSERT(false);
			return NULL;
		}

		for (Int i = 0; i < rSize; i+=2)
		{
			UChar i0 = pBuffer[i+0];
			pBuffer[i+0] = pBuffer[i+1];
			pBuffer[i+1] = i0;
		}
	}

	return pBuffer16;
}

//----------------------------------------------------------------------------
void Importer::Free32(Float* pFloats)
{
	if (!pFloats)
	{
		return;
	}

	UChar* pUChar = reinterpret_cast<UChar*>(pFloats);
	WIRE_DELETE[] pUChar;
}

//----------------------------------------------------------------------------
const Importer::Statistics* Importer::GetStatistics()
{
	return &mStatistics;
}

//----------------------------------------------------------------------------
void Importer::ResetStatistics()
{
	mStatistics.RenderObjectCount = 0;
	mStatistics.NodeCount = 0;
	mStatistics.TextureCount = 0;
	mStatistics.MaterialCount = 0;
	mStatistics.MeshCount = 0;
	mStatistics.VertexBufferCount = 0;
	mStatistics.IndexBufferCount = 0;
	mStatistics.ColliderCount = 0;
	mStatistics.RigidBodyCount = 0;
}

//----------------------------------------------------------------------------
void Importer::Traverse(rapidxml::xml_node<>* pXmlNode, Node* pParent)
{
	if (Is("Assets", pXmlNode->name()))
	{
		ParseAssets(pXmlNode);
		return;
	}

	if (Is("Skybox", pXmlNode->name()))
	{
		NodeSkybox* pSkybox = ParseSkybox(pXmlNode);
		if (pSkybox)
		{
			pParent->AttachChild(pSkybox);
		}

		return;
	}

	if (Is("Text", pXmlNode->name()))
	{
		Node* pText = ParseText(pXmlNode);
		if (pText)
		{
			pParent->AttachChild(pText);
		}

		return;
	}

	WIRE_ASSERT(Is("Node", pXmlNode->name()));
	Node* pNode = ParseNode(pXmlNode, pParent);

	if (pXmlNode->first_node())
	{
		for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
			pChild = pChild->next_sibling())
		{
			if (Is("Node", pChild->name()) || Is("Text", pChild->name()) ||
				Is("Skybox", pChild->name()))
			{
				Traverse(pChild, pNode);
			}
		}
	}
}

//----------------------------------------------------------------------------
Char* Importer::GetValue(rapidxml::xml_node<>* pXmlNode, const Char* pName)
{
	for (rapidxml::xml_attribute<>* attr = pXmlNode->first_attribute();	attr;
		attr = attr->next_attribute())
	{
		if (Is(pName, attr->name()))
		{
			return attr->value();
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------
Bool Importer::HasValue(rapidxml::xml_node<>* pXmlNode, const Char* pName)
{
	return GetValue(pXmlNode, pName) != NULL;
}

//----------------------------------------------------------------------------
Bool Importer::IsTrue(const Char* pSrc, rapidxml::xml_node<>* pXmlNode)
{
	for (rapidxml::xml_attribute<>* attr = pXmlNode->first_attribute();	attr;
		attr = attr->next_attribute())
	{
		if (Is(pSrc, attr->name()))
		{
			Char* pValue = attr->value();
			if (pValue)
			{
				if (*pValue == '1')
				{
					return true;
				}
			}
		}
	}

	return false;
}

//----------------------------------------------------------------------------
Buffer::UsageType Importer::GetUsageType(rapidxml::xml_node<>* pXmlNode)
{
	for (rapidxml::xml_attribute<>* attr = pXmlNode->first_attribute();	attr;
		attr = attr->next_attribute())
	{
		if (Is("Usage", attr->name()))
		{
			Char* pValue = attr->value();
			if (pValue)
			{
				if (Is("STATIC", pValue))
				{
					return Buffer::UT_STATIC;
				}
				else if (Is("DYNAMIC", pValue))
				{
					return Buffer::UT_DYNAMIC;
				}
				else if (Is("DYNAMIC_WRITE_ONLY", pValue))
				{
					return  Buffer::UT_DYNAMIC_WRITE_ONLY;
				}
				else if (Is("STATIC_DISCARD_ON_BIND", pValue))
				{
					return  Buffer::UT_STATIC_DISCARD_ON_BIND;
				}
			}
		}
	}

	return Buffer::UT_STATIC;
}

//----------------------------------------------------------------------------
Float Importer::GetFloat(rapidxml::xml_node<>* pXmlNode, const Char* pName)
{
	Float f = 0;
	GetFloat(pXmlNode, pName, f);
	return f;
}

//----------------------------------------------------------------------------
void Importer::GetFloat(rapidxml::xml_node<>* pXmlNode, const Char* pName,
	Float& rFloat)
{
	Char* pFloat = GetValue(pXmlNode, pName);
	if (pFloat)
	{
		Int n;
		n = sscanf(pFloat, "%f", &rFloat);
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
	}
}

//----------------------------------------------------------------------------
void Importer::GetHex(rapidxml::xml_node<>* pXmlNode, const Char* pName,
	UInt& rHex)
{
	Char* pHex = GetValue(pXmlNode, pName);
	if (pHex)
	{
		Int n;
		n = sscanf(pHex, "%x", &rHex);
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
	}
}

//----------------------------------------------------------------------------
UInt Importer::GetUInt(rapidxml::xml_node<>* pXmlNode, const Char* pName)
{
	Char* pUInt = GetValue(pXmlNode, pName);
	UInt i = 0;
	if (pUInt)
	{
		Int n;
		n = sscanf(pUInt, "%d", &i);
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
	}

	return i;
}

//----------------------------------------------------------------------------
Bool Importer::GetBool(rapidxml::xml_node<>* pXmlNode, const Char* pName)
{
	Bool b = false;
	GetBool(pXmlNode, pName, b);
	return b;
}

//----------------------------------------------------------------------------
void Importer::GetBool(rapidxml::xml_node<>* pXmlNode, const Char* pName,
	Bool& rBool)
{
	Char* pBool = GetValue(pXmlNode, pName);
	UInt b = 0;
	if (pBool)
	{
		Int n;
		n = sscanf(pBool, "%d", &b);
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
		rBool = (b != 0);
	}
}

//----------------------------------------------------------------------------
Vector3F Importer::GetVector3(rapidxml::xml_node<>* pXmlNode, const Char*
	pName)
{
	Vector3F v = Vector3F::ZERO;
	Char* pCol = GetValue(pXmlNode, pName);
	if (pCol)
	{
		Int n;
		n = sscanf(pCol, "%f, %f, %f", &v.X(), &v.Y(), &v.Z());
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 3);
	}

	return v;
}

//----------------------------------------------------------------------------
ColorRGB Importer::GetColorRGB(rapidxml::xml_node<>* pXmlNode, const Char*
	pName, Bool& rHasValue)
{
	ColorRGB c = ColorRGB::WHITE;
	rHasValue = false;
	Char* pCol = GetValue(pXmlNode, pName);
	if (pCol)
	{
		Int n;
		n = sscanf(pCol, "%f, %f, %f", &c.R(), &c.G(), &c.B());
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 3);
		rHasValue = true;
	}

	return c;
}

//----------------------------------------------------------------------------
ColorRGBA Importer::GetColorRGBA(rapidxml::xml_node<>* pXmlNode, const Char*
	pName, Bool& rHasValue)
{
	ColorRGBA c = ColorRGBA::WHITE;
	rHasValue = false;
	Char* pCol = GetValue(pXmlNode, pName);
	if (pCol)
	{
		Int n;
		n = sscanf(pCol, "%f, %f, %f, %f", &c.R(), &c.G(), &c.B(), &c.A());
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 4);
		rHasValue = true;
	}

	return c;
}

//----------------------------------------------------------------------------
void Importer::UpdateWorldTransformation(Spatial* pSpatial)
{
	WIRE_ASSERT(pSpatial);
	if (pSpatial->WorldIsCurrent)
	{
		return;
	}

	Spatial* pParent = pSpatial->GetParent();
	Spatial* pRoot = pSpatial;
	while (pParent)
	{
		pRoot = pParent;
		pParent = pParent->GetParent();
	}

	pRoot->UpdateGS(0, true, false);
}

//----------------------------------------------------------------------------
void Importer::ParseAssets(rapidxml::xml_node<>* pXmlNode)
{
	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
			pChild = pChild->next_sibling())
		{
			if (Is("Meshes", pChild->name()))
			{
				for (rapidxml::xml_node<>* pMesh = pChild->first_node();
					pMesh; pMesh = pMesh->next_sibling())
				{
					ParseMesh(pMesh);
				}
			}
			else if (Is("Materials", pChild->name()))
			{
				for (rapidxml::xml_node<>* pMaterial = pChild->first_node();
					pMaterial; pMaterial = pMaterial->next_sibling())
				{
					ParseMaterial(pMaterial);
				}
			}
			else if (Is("Lights", pChild->name()))
			{
				for (rapidxml::xml_node<>* pLight = pChild->first_node();
					pLight; pLight = pLight->next_sibling())
				{
					ParseLight(pLight);
				}
			}
		}
	}
}

#ifndef NO_BULLET_PHYSICS_LIB
//----------------------------------------------------------------------------
btTransform Importer::GetBtTransform(Spatial* pSpatial, const Vector3F& rCenter)
{
	WIRE_ASSERT(pSpatial);

	UpdateWorldTransformation(pSpatial);
	Vector3F center = rCenter;
	Transformation& rWorld = pSpatial->World;
	center.X() *= rWorld.GetScale().X();
	center.Y() *= rWorld.GetScale().Y();
	center.Z() *= rWorld.GetScale().Z();
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(PhysicsWorld::Convert(rWorld.GetTranslate() + center));
	transform.setBasis(PhysicsWorld::Convert(rWorld.GetRotate()));
	return transform;
}

//----------------------------------------------------------------------------
void Importer::AddRigidBodyController(Spatial* pSpatial, btCollisionShape*
	pCollisionShape, RigidBodyInfo& rInfo, const Vector3F& rCenter,
	VertexBuffer* pVBRef, IndexBuffer* pIBRef)
{
	btTransform transform = GetBtTransform(pSpatial, rCenter);

	// radius of sphere was already scaled by maximum component of local scale
	if (pCollisionShape->getShapeType() != SPHERE_SHAPE_PROXYTYPE)
	{
		pCollisionShape->setLocalScaling(PhysicsWorld::Convert(pSpatial->World.GetScale()));
	}

	// rigid body is dynamic if and only if mass is non zero
	btVector3 localInertia(0, 0, 0);
	if (rInfo.Mass != 0.0F && pCollisionShape->getShapeType() != EMPTY_SHAPE_PROXYTYPE)
	{
		pCollisionShape->calculateLocalInertia(rInfo.Mass, localInertia);
	}

	btDefaultMotionState* pMotionState = WIRE_NEW btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyInformation(rInfo.Mass,
		pMotionState, pCollisionShape, localInertia);
	btRigidBody* pRigidBody = WIRE_NEW btRigidBody(rigidBodyInformation);
	mStatistics.RigidBodyCount++;

	if (rInfo.IsKinematic)
	{
		pRigidBody->setCollisionFlags(pRigidBody->getCollisionFlags() |
			btCollisionObject::CF_KINEMATIC_OBJECT);
	}

//	pRigidBody->setContactProcessingThreshold(BT_LARGE_FLOAT);

// Only do CCD if  motion in one timestep (1/60) exceeds 
//	pRigidBody->setCcdMotionThreshold(0.5F);

//Experimental: better estimation of CCD Time of Impact.
//	pRigidBody->setCcdSweptSphereRadius(0.2F);

	if (rInfo.Mass != 0.0F)
	{
		pRigidBody->setDamping(rInfo.Damping, rInfo.AngularDamping);
	}

	mspPhysicsWorld->AddCollisionShape(pCollisionShape, pVBRef, pIBRef);

	RigidBodyController* pController = WIRE_NEW RigidBodyController(
		mspPhysicsWorld, pRigidBody);
	pSpatial->AttachController(pController);

	if (rInfo.IsEnabled)
	{
		pController->SetEnabled(true);
	}
}

//----------------------------------------------------------------------------
void Importer::ParseRigidBodyInfo(RigidBodyInfo& rInfo, rapidxml::xml_node<>* pXmlNode)
{
	rInfo.Mass = GetFloat(pXmlNode, "Mass");
	rInfo.IsKinematic = GetBool(pXmlNode, "Kinematic");
	rInfo.Damping = GetFloat(pXmlNode, "Drag");
	rInfo.AngularDamping = GetFloat(pXmlNode, "AngularDrag");
}

//----------------------------------------------------------------------------
void Importer::ParseRigidBody(rapidxml::xml_node<>* pXmlNode, Spatial* pSpatial)
{
	// Parse a rigid body that does NOT have a collider.
	// Rigid bodies with colliders are treated in ParseCollider().

	if (!mspPhysicsWorld)
	{
		return;
	}

	rapidxml::xml_node<>* pXmlParent = pXmlNode->parent();
	for (rapidxml::xml_node<>* pChild = pXmlParent->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		if (Is("Collider", pChild->name()))
		{
			return;
		}
	}

	// TODO: Make sure to re-use shapes among rigid bodies whenever possible!
	btCollisionShape* pCollisionShape = WIRE_NEW btEmptyShape;

	RigidBodyInfo info;
	ParseRigidBodyInfo(info, pXmlNode);
	AddRigidBodyController(pSpatial, pCollisionShape, info);
}

//----------------------------------------------------------------------------
void Importer::ParseCollider(rapidxml::xml_node<>* pXmlNode, Spatial* pSpatial)
{
	if (!mspPhysicsWorld)
	{
		return;
	}

	Char* pShapeName = GetValue(pXmlNode, "Shape");
	WIRE_ASSERT(pShapeName);

	// Rigid body parameters
	RigidBodyInfo info;
	rapidxml::xml_node<>* pXmlParent = pXmlNode->parent();
	for (rapidxml::xml_node<>* pChild = pXmlParent->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		if (Is("RigidBody", pChild->name()))
		{
			ParseRigidBodyInfo(info, pChild);
		}
	}

	GetBool(pXmlNode, "Enabled", info.IsEnabled);

	// TODO: Make sure to re-use shapes among rigid bodies whenever possible!
	btCollisionShape* pCollisionShape = NULL;
	Vector3F center = Vector3F::ZERO;
	VertexBuffer* pMeshVB = NULL;
	IndexBuffer* pMeshIB = NULL;

	if (Is("Mesh", pShapeName))
	{
		Char* pMeshName = GetValue(pXmlNode, "Mesh");
		WIRE_ASSERT(pMeshName);

		TArray<MeshPtr>* pValue = mMeshes.Find(pMeshName);
		WIRE_ASSERT(pValue);
		WIRE_ASSERT(0 < pValue->GetQuantity());
		Mesh* pMesh = (*pValue)[0];
		Bool isConvex = GetBool(pXmlNode, "Convex");

		// Bullet recommends to use less than 100 vertices in a convex mesh
		WIRE_ASSERT(pMesh->GetVertexCount() < 100);

		btTriangleIndexVertexArray* pTriangleIndexVertexArray =
			PhysicsWorld::Convert(pMesh, false);
 		pMeshVB = pMesh->GetPositionBuffer();
 		pMeshIB = pMesh->GetIndexBuffer();

		if (pTriangleIndexVertexArray)
		{
			if (info.Mass != 0)
			{
				if (isConvex)
				{
					btConvexHullShape* pHull = WIRE_NEW btConvexHullShape();
					pCollisionShape = pHull;

					for (UInt i = 0; i < pMeshVB->GetQuantity(); i++)
					{
						Vector3F v = pMeshVB->Position3(i);
						Bool isDuplicate = false;
						for (UInt j = 0; j < i; j++)
						{
							if (v == pMeshVB->Position3(j))
							{
								isDuplicate = true;
								break;
							}
						}

						if (!isDuplicate)
						{
							pHull->addPoint(PhysicsWorld::Convert(v));
						}
					}
				}
				else
				{
					pCollisionShape = WIRE_NEW btConvexTriangleMeshShape(
						pTriangleIndexVertexArray, true);

// 					btGImpactMeshShape* pGImpactMeshShape = WIRE_NEW
// 						btGImpactMeshShape(pTriangleIndexVertexArray);
// 					pGImpactMeshShape->updateBound();
// 					pCollisionShape = pGImpactMeshShape;
// 
// 					btCollisionDispatcher * dispatcher = static_cast<btCollisionDispatcher *>(mspPhysicsWorld->Get()->getDispatcher());
// 					btGImpactCollisionAlgorithm::registerAlgorithm(dispatcher);
				}
			}
			else
			{
				pCollisionShape = WIRE_NEW btBvhTriangleMeshShape(
					pTriangleIndexVertexArray, true);
			}
		}
	}
	else
	{
		center = GetVector3(pXmlNode, "Center");

		if (Is("Character", pShapeName))
		{
			Float height = GetFloat(pXmlNode, "Height");
			Float radius = GetFloat(pXmlNode, "Radius");
			Float slope = GetFloat(pXmlNode, "Slope");
			Float step = GetFloat(pXmlNode, "Step");

			btConvexShape* pConvexShape = WIRE_NEW btCapsuleShape(radius, height);
			pConvexShape->setLocalScaling(PhysicsWorld::Convert(pSpatial->Local.GetScale()));
			mspPhysicsWorld->AddCollisionShape(pConvexShape);

			btPairCachingGhostObject* pGhostObject = WIRE_NEW btPairCachingGhostObject();
			pGhostObject->setCollisionShape(pConvexShape);
			pGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
			pGhostObject->setWorldTransform(GetBtTransform(pSpatial, center));

			btKinematicCharacterController* pCharacter = WIRE_NEW
				btKinematicCharacterController(pGhostObject, pConvexShape, step);
			pCharacter->setMaxSlope(slope * MathF::DEG_TO_RAD);

			CharacterController* pController = WIRE_NEW CharacterController(
				mspPhysicsWorld, pGhostObject, pCharacter);
			pController->SetEnabled(true);
			pSpatial->AttachController(pController);
			return;
		}
		else if (Is("Box", pShapeName))
		{
			Vector3F size = GetVector3(pXmlNode, "Size");
			pCollisionShape = WIRE_NEW btBoxShape(PhysicsWorld::Convert(size * 0.5f));
		}
		else if (Is("Sphere", pShapeName))
		{
			Float radius = GetFloat(pXmlNode, "Radius");
			const Vector3F& rLocalScale = pSpatial->Local.GetScale();
			Float maxScale = MathF::Max(rLocalScale.X(), rLocalScale.Y());
			maxScale = MathF::Max(rLocalScale.Z(), maxScale);
			pCollisionShape = WIRE_NEW btSphereShape(radius*maxScale);
		}
		else if (Is("Capsule", pShapeName))
		{
			Float radius = GetFloat(pXmlNode, "Radius");
			Float height = GetFloat(pXmlNode, "Height");
			Char* pDirection = GetValue(pXmlNode, "Direction");
			WIRE_ASSERT(pDirection);
			switch (*pDirection)
			{
			case 'X' : 
				pCollisionShape = WIRE_NEW btCapsuleShapeX(radius, height);
				break;
			case 'Y' : 
				pCollisionShape = WIRE_NEW btCapsuleShape(radius, height);
				break;
			case 'Z' : 
				pCollisionShape = WIRE_NEW btCapsuleShapeZ(radius, height);
				break;
			default:
				WIRE_ASSERT(false);
			}
		}
		else 
		{
			WIRE_ASSERT(false /* Collider shape not supported yet! */);
		}
	}

	if (!pCollisionShape)
	{
		WIRE_ASSERT(false /* unsupported collision shape */);
		return;
	}

	AddRigidBodyController(pSpatial, pCollisionShape, info, center,
		pMeshVB, pMeshIB);

	mStatistics.ColliderCount++;
}
#endif

//----------------------------------------------------------------------------
Light* Importer::ParseLight(rapidxml::xml_node<>* pXmlNode)
{
	Char* pName = GetValue(pXmlNode, "Name");
	if (pName)
	{
		if (mpOptions->AssetsWithEqualNamesAreIdentical)
		{
			LightPtr* pValue = mLights.Find(pName);
			if (pValue)
			{
				WIRE_ASSERT(*pValue);
				return *pValue;
			}
		}
	}

	Char* pType = GetValue(pXmlNode, "Type");
	WIRE_ASSERT(pType);
	Light* pLight = WIRE_NEW Light;
	Light::LightType lt = Light::LT_POINT;

	Float range = GetFloat(pXmlNode, "Range");

	if (Is("Point", pType))
	{
		lt = Light::LT_POINT;
	}
	else if (Is("Directional", pType))
	{
		lt = Light::LT_DIRECTIONAL;
	}
	else if (Is("Spot", pType))
	{
		lt = Light::LT_SPOT;
		if (HasValue(pXmlNode, "Angle"))
		{
			pLight->Angle = GetFloat(pXmlNode, "Angle");
		}

		if (HasValue(pXmlNode, "Exp"))
		{
			pLight->Exponent = GetFloat(pXmlNode, "Exp");
		}
	}

	Char* pCol = GetValue(pXmlNode, "Direction");
	if (pCol)
	{
		Vector3F& rDir = pLight->Direction;
		Int n;
		n = sscanf(pCol, "%f, %f, %f", &rDir.X(), &rDir.Y(), &rDir.Z());
		WIRE_ASSERT_NO_SIDEEFFECTS(n == 3);
	}

	pLight->Type = lt;

	if (range > 0.0F)
	{
		pLight->Quadric = (1.0F/(0.01F*range*range));
	}

	Bool hasValue;
	ColorRGB ambient = GetColorRGB(pXmlNode, "Ambient", hasValue); 
	if (hasValue)
	{
		pLight->Ambient = ambient; 
	}

	ColorRGB color = GetColorRGB(pXmlNode, "Color", hasValue);
	if (hasValue)
	{
		pLight->Color = color;
	}

	UInt mask = static_cast<UInt>(~0);
	GetHex(pXmlNode, "Mask", mask);
	UInt64 layerMask = mask;
	if (!GetBool(pXmlNode, "Baked"))
	{
		layerMask |= 0x100000000;
	}

	pLight->LayerMask = layerMask;

	Bool enabled = true;
	GetBool(pXmlNode, "Enabled", enabled);
	pLight->Enabled = enabled;

	if (pName)
	{
		mLights.Insert(pName, pLight);
	}

	return pLight;
}

//----------------------------------------------------------------------------
void Importer::ParseCamera(rapidxml::xml_node<>* pXmlNode, Spatial* pSpatial)
{
	if (DynamicCast<Node>(pSpatial) == NULL)
	{
		return;
	}

	Vector3F cameraLocation(0, 0, 0);
	Vector3F viewDirection(0.0F, 0.0F, 1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	Vector3F right = viewDirection.Cross(up);

	Float fov = GetFloat(pXmlNode, "Fov");
	Float near = GetFloat(pXmlNode, "Near");
	Float far = GetFloat(pXmlNode, "Far");
	Camera* pCam = WIRE_NEW Camera(fov != 0.0F);
	if (fov != 0.0F)
	{
		pCam->SetFrame(cameraLocation, viewDirection, up, right);
	}

	Float leftV = 0;
	Float rightV = 1;
	Float topV = 1;
	Float bottomV = 0;
	GetFloat(pXmlNode, "Left", leftV);
	GetFloat(pXmlNode, "Right", rightV);
	GetFloat(pXmlNode, "Top", topV);
	GetFloat(pXmlNode, "Bottom", bottomV);

	Float aspectRatio = 640.0F / 480.0F;
	Float div = topV - bottomV;
	WIRE_ASSERT(div != 0);
	aspectRatio *= (rightV-leftV) / div;
	pCam->SetFrustum(fov, aspectRatio, near, far);
	pCam->SetViewport(leftV, rightV, topV, bottomV);

	UInt mask = static_cast<UInt>(~0);
	GetHex(pXmlNode, "Mask", mask);
	UInt64 cullingMask = mask;
	pCam->SetLayerMask(cullingMask);

	NodeCamera* pCameraNode = WIRE_NEW NodeCamera(pCam);
	Bool isEnabled = true;
	GetBool(pXmlNode, "Enabled", isEnabled);
	pCameraNode->SetEnabled(isEnabled);
	StaticCast<Node>(pSpatial)->AttachChild(pCameraNode);
}

//----------------------------------------------------------------------------
void Importer::ParseTransformation(rapidxml::xml_node<>* pXmlNode, Spatial*
	pSpatial)
{
	Vector3F t = Vector3F::ZERO;
	QuaternionF r = QuaternionF::IDENTITY;
	Vector3F s = Vector3F::ONE;
	
	for (rapidxml::xml_attribute<>* attr = pXmlNode->first_attribute();	attr;
		attr = attr->next_attribute())
	{
		if (Is("Pos", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%f, %f, %f", &t.X(), &t.Y(), &t.Z());
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 3);
		}
		else if (Is("Rot", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%f, %f, %f, %f", &r.W(), &r.X(),
				&r.Y(), &r.Z());
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 4);
		}
		else if (Is("Scale", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%f, %f, %f", &s.X(), &s.Y(), &s.Z());
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 3);
		}
		else if (Is("Static", attr->name()))
		{
			Int n;
			Int st;
			n = sscanf(attr->value(), "%d", &st);
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
			if (st != 0)
			{
				mStaticSpatials.Append(pSpatial);
				pSpatial->WorldIsCurrent = true;
			}
		}
	}

	Transformation& rTrafo = pSpatial->Local;
	if ((t.X() != 0) || (t.Y() != 0) || (t.Z() != 0))
	{
		rTrafo.SetTranslate(t);
	}

	if ((r.W() != 1) || (r.X() != 0) || (r.Y() != 0) || (r.Z() != 0))
	{
		Matrix3F m;
		r.ToRotationMatrix(m);
		rTrafo.SetRotate(m);
	}

	if ((s.X() != 1.0F) || (s.Y() != 1.0F) || (s.Z() != 1.0F))
	{
		if ((s.X() == s.Y()) && (s.X() == s.Z()))
		{
			rTrafo.SetUniformScale(s.X());
		}
		else
		{
			rTrafo.SetScale(s);
		}
	}

	if (pSpatial->WorldIsCurrent)
	{
		pSpatial->World = pSpatial->Local;
	}
}

//---------------------------------------------------------------------------
Node* Importer::ParseNode(rapidxml::xml_node<>* pXmlNode, Node* pParent)
{
	RenderObject* pRenderObject = ParseRenderObject(pXmlNode);
	Node* pNode = WIRE_NEW Node(pRenderObject);
	pParent->AttachChild(pNode);

	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		if (Is("LightNode", pChild->name()))
		{
			// attach as dedicated child node due to the light node using
			// the light's parameters as its own local transformation
			ParseComponents(pChild, pNode);
			WIRE_ASSERT(pNode->GetLightQuantity() == 1);
			Light* pLight = pNode->GetLight();
			NodeLight* pLightNode = WIRE_NEW NodeLight;
			mStatistics.NodeCount++;
			pLightNode->Set(pLight);
			pNode->DetachLight(pLight);
			pNode->AttachChild(pLightNode);
			break;
		}
	}

	mStatistics.NodeCount++;

	Char* pName = GetValue(pXmlNode, "Name");
	if (pName)
	{
		pNode->SetName(pName);
	}

	UInt layer = GetUInt(pXmlNode, "Layer");
	WIRE_ASSERT(layer < 32);
	pNode->SetLayerMask(1 << layer);

	ParseTransformationAndComponents(pXmlNode, pNode);

	if (pRenderObject && pRenderObject->GetMaterial())
	{
		TArray<StatePtr>* pStateList = mMaterialStates.Find(pRenderObject->
			GetMaterial());
		if (pStateList)
		{
			for (UInt i = 0; i < pStateList->GetQuantity(); i++)
			{
				pNode->AttachState((*pStateList)[i]);
			}
		}
	}
	
	return pNode;
}

//----------------------------------------------------------------------------
RenderObject* Importer::ParseRenderObject(rapidxml::xml_node<>* pXmlNode)
{
	Mesh* pMesh = NULL;
	Material* pMaterial = NULL;

	if (pXmlNode->first_node())
	{
		for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
			pChild = pChild->next_sibling())
		{
			if (Is("Mesh", pChild->name()))
			{
				pMesh = ParseMesh(pChild);
			}
			else if (Is("Material", pChild->name()))
			{
				pMaterial = ParseMaterial(pChild);
			}
		}
	}

	if (!pMesh)
	{
		return NULL;
	}

	if (pMesh->HasColor())
	{
		if (pMaterial && pMaterial->GetTextureQuantity() > 0)
		{
			pMaterial->SetBlendMode(Material::BM_MODULATE);
		}
	}

	RenderObject* pRenderObject = WIRE_NEW RenderObject(pMesh, pMaterial);
	mStatistics.RenderObjectCount++;
	return pRenderObject;
}

//----------------------------------------------------------------------------
Node* Importer::ParseText(rapidxml::xml_node<>* pXmlNode)
{
	Int width = 8;
	Int height = 8;
	Int maxLength = -1;

	for (rapidxml::xml_attribute<>* attr = pXmlNode->first_attribute();	attr;
		attr = attr->next_attribute())
	{
		if (Is("Width", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%d", &width);
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
		}
		else if (Is("Height", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%d", &height);
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
		}
		else if (Is("MaxLength", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%d", &maxLength);
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);
		}
	}

	Char* pString = GetValue(pXmlNode, "String");
	if (maxLength < 0)
	{
		maxLength = System::Strlen(pString);
	}

	RenderText* pText = NULL;
	Char* pFontName = GetValue(pXmlNode, "Font");
	if (pFontName)
	{
		String path = String(mpPath) + String(pFontName);
		pText = CreateText(path, width, height, maxLength);
	}
	else
	{
		pText = StandardMesh::CreateText(maxLength);
	}

	if (!pText)
	{
		WIRE_ASSERT(false);
		return NULL;
	}

	Bool hasValue;
	ColorRGBA color = GetColorRGBA(pXmlNode, "Color", hasValue);
	if (hasValue)
	{
		pText->SetColor(color);
	}

	if (pString)
	{
		Bool isOk = pText->Set(pString);
		WIRE_ASSERT_NO_SIDEEFFECTS(isOk /* Wire::Text::Set() failed */);
	}

	Node* pTextNode = WIRE_NEW Node(pText);
	mStatistics.RenderObjectCount++;

	Char* pName = GetValue(pXmlNode, "Name");
	if (pName)
	{
		pTextNode->SetName(pName);
	}

	ParseTransformationAndComponents(pXmlNode, pTextNode);

	return pTextNode;
}

//----------------------------------------------------------------------------
NodeSkybox* Importer::ParseSkybox(rapidxml::xml_node<>* pXmlNode)
{
	if (!pXmlNode->first_node())
	{
		return NULL;
	}

	Texture2D* pPosZ = ParseSkyboxTexture("PosZ", pXmlNode);
	Texture2D* pNegZ = ParseSkyboxTexture("NegZ", pXmlNode);
	Texture2D* pPosX = ParseSkyboxTexture("PosX", pXmlNode);
	Texture2D* pNegX = ParseSkyboxTexture("NegX", pXmlNode);
	Texture2D* pPosY = ParseSkyboxTexture("PosY", pXmlNode);
	Texture2D* pNegY = ParseSkyboxTexture("NegY", pXmlNode);

	NodeSkybox* pSkyBox = WIRE_NEW NodeSkybox(pPosZ, pNegZ, pPosX, pNegX,
		pPosY, pNegY);

	ParseComponents(pXmlNode, pSkyBox);

	return pSkyBox;
}

//----------------------------------------------------------------------------
Texture2D* Importer::ParseSkyboxTexture(const Char* pName,
	rapidxml::xml_node<>* pXmlNode)
{
	Texture2D* pTexture = NULL;
	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		if (Is(pName, pChild->name()))
		{
			for (rapidxml::xml_node<>* pGrandChild = pChild->first_node();
				pGrandChild; pGrandChild = pGrandChild->next_sibling())
			{
				if (Is("Texture", pGrandChild->name()))
				{
					Material::BlendMode bm = Material::BM_REPLACE;
					return ParseTexture(pGrandChild, bm);
				}
			}
		}
	}

	return pTexture;
}

//----------------------------------------------------------------------------
void Importer::ParseComponent(rapidxml::xml_node<>* pXmlNode, Spatial*
	pSpatial)
{
	State* pState = ParseRenderStates(pXmlNode);
	if (pState)
	{
		pSpatial->AttachState(pState);
	}
	else if (Is("Camera", pXmlNode->name()))
	{
		ParseCamera(pXmlNode, pSpatial);
	}
	else if (Is("Light", pXmlNode->name()))
	{
		Light* pLight =	ParseLight(pXmlNode);
		if (pLight)
		{
			pSpatial->AttachLight(pLight);
		}
	}
#ifndef NO_BULLET_PHYSICS_LIB
	else if (Is("Collider", pXmlNode->name()))
	{
		ParseCollider(pXmlNode, pSpatial);
	}
	else if (Is("RigidBody", pXmlNode->name()))
	{
		ParseRigidBody(pXmlNode, pSpatial);
	}
#endif
}

//----------------------------------------------------------------------------
void Importer::ParseComponents(rapidxml::xml_node<>* pXmlNode, Spatial*
	pSpatial)
{
	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		ParseComponent(pChild, pSpatial);
	}
}

//----------------------------------------------------------------------------
void Importer::ParseTransformationAndComponents(rapidxml::xml_node<>* pXmlNode,
	Spatial* pSpatial)
{
	ParseTransformation(pXmlNode, pSpatial);
	ParseComponents(pXmlNode, pSpatial);
}

//----------------------------------------------------------------------------
State* Importer::ParseRenderStates(rapidxml::xml_node<>* pXmlNode)
{
	if (Is("MaterialState", pXmlNode->name()))
	{
		StateMaterial* pMaterialState = WIRE_NEW StateMaterial;
		Bool hasValue;	
		ColorRGBA ambient = GetColorRGBA(pXmlNode, "Ambient", hasValue);
		if (hasValue)
		{
			pMaterialState->Ambient = ambient;
		}

		return pMaterialState;
	}
	else if (Is("AlphaState", pXmlNode->name()))
	{
		StateAlpha* pAlphaState = WIRE_NEW StateAlpha;
		pAlphaState->BlendEnabled = GetBool(pXmlNode, "Enabled");

		Char* pSrcBlend = GetValue(pXmlNode, "SrcBlendMode");
		if (pSrcBlend)
		{
			if (Is("ZERO", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_ZERO;
			}
			else if (Is("ONE", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_ONE;
			}
			else if (Is("DST_COLOR", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_DST_COLOR;
			}
			else if (Is("ONE_MINUS_DST_COLOR", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_ONE_MINUS_DST_COLOR;
			}
			else if (Is("SRC_ALPHA", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_SRC_ALPHA;
			}
			else if (Is("ONE_MINUS_SRC_ALPHA", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_ONE_MINUS_SRC_ALPHA;
			}
			else if (Is("DST_ALPHA", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_DST_ALPHA;
			}
			else if (Is("ONE_MINUS_DST_ALPHA", pSrcBlend))
			{
				pAlphaState->SrcBlend = StateAlpha::SBM_ONE_MINUS_DST_ALPHA;
			}
		}

		Char* pDstBlend = GetValue(pXmlNode, "DstBlendMode");
		if (pDstBlend)
		{
			if (Is("ZERO", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_ZERO;
			}
			else if (Is("ONE", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_ONE;
			}
			else if (Is("SRC_COLOR", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_SRC_COLOR;
			}
			else if (Is("ONE_MINUS_SRC_COLOR", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_ONE_MINUS_SRC_COLOR;
			}
			else if (Is("SRC_ALPHA", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_SRC_ALPHA;
			}
			else if (Is("ONE_MINUS_SRC_ALPHA", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_ONE_MINUS_SRC_ALPHA;
			}
			else if (Is("DST_ALPHA", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_DST_ALPHA;
			}
			else if (Is("ONE_MINUS_DST_ALPHA", pDstBlend))
			{
				pAlphaState->DstBlend = StateAlpha::DBM_ONE_MINUS_DST_ALPHA;
			}
		}

		return pAlphaState;
	}
	else if (Is("ZBufferState", pXmlNode->name()))
	{
		StateZBuffer* pZBufferState = WIRE_NEW StateZBuffer;
		pZBufferState->Enabled = GetBool(pXmlNode, "Enabled");
		pZBufferState->Writable = GetBool(pXmlNode, "Writable");

		Char* pCmpFunc = GetValue(pXmlNode, "CmpFunc");
		if (pCmpFunc)
		{
			if (Is("NEVER", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_NEVER;
			}
			else if (Is("LESS", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_LESS;
			}
			else if (Is("EQUAL", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_EQUAL;
			}
			else if (Is("LEQUAL", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_LEQUAL;
			}
			else if (Is("GREATER", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_GREATER;
			}
			else if (Is("NOTEQUAL", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_NOTEQUAL;
			}
			else if (Is("GEQUAL", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_GEQUAL;
			}
			else if (Is("ALWAYS", pCmpFunc))
			{
				pZBufferState->Compare = StateZBuffer::CF_ALWAYS;
			}
		}

		return pZBufferState;
	}
	else if (Is("CullState", pXmlNode->name()))
	{
		StateCull* pCullState = WIRE_NEW StateCull;
		pCullState->Enabled = GetBool(pXmlNode, "Enabled");

		Char* pMode = GetValue(pXmlNode, "Mode");
		if (pMode)
		{
			if (Is("OFF", pMode))
			{
				pCullState->CullFace = StateCull::CM_OFF;
			}
			else if (Is("BACK", pMode))
			{
				pCullState->CullFace = StateCull::CM_BACK;
			}
			else if (Is("FRONT", pMode))
			{
				pCullState->CullFace = StateCull::CM_FRONT;
			}
		}

		return pCullState;
	}
	else if (Is("WireframeState", pXmlNode->name()))
	{
		StateWireframe* pWireframeState = WIRE_NEW StateWireframe;
		pWireframeState->Enabled = GetBool(pXmlNode, "Enabled");

		return pWireframeState;
	}
	else if (Is("FogState", pXmlNode->name()))
	{
		StateFog* pFogState = WIRE_NEW StateFog;
		pFogState->Enabled = GetBool(pXmlNode, "Enabled");
		Bool hasValue;
		ColorRGB color = GetColorRGB(pXmlNode, "Color", hasValue);
		if (hasValue)
		{
			pFogState->Color = color;
		}

		pFogState->Start = GetFloat(pXmlNode, "Start");
		pFogState->End = GetFloat(pXmlNode, "End");

		Char* pFunc = GetValue(pXmlNode, "Func");
		if (pFunc)
		{
			if (Is("LINEAR", pFunc))
			{
				pFogState->DensityFunc = StateFog::DF_LINEAR;
			}
			else if (Is("EXP", pFunc))
			{
				pFogState->DensityFunc = StateFog::DF_EXP;
			}
			else if (Is("EXPSQR", pFunc))
			{
				pFogState->DensityFunc = StateFog::DF_EXPSQR;
			}
		}

		return pFogState;
	}

	return NULL;
}

//----------------------------------------------------------------------------
Mesh* Importer::ParseMesh(rapidxml::xml_node<>* pXmlNode)
{
	Char* pName = GetValue(pXmlNode, "Name");
	if (!pName)
	{
		WIRE_ASSERT(false /* Mesh has no name */);
		return NULL;
	}

	UInt subMeshIndex = GetUInt(pXmlNode, "SubMesh");

	if (mpOptions->AssetsWithEqualNamesAreIdentical)
	{
		TArray<MeshPtr>* pValue = mMeshes.Find(pName);
		if (pValue)
		{
			WIRE_ASSERT(subMeshIndex < pValue->GetQuantity());
			return (*pValue)[subMeshIndex];
		}
	}

	TArray<VertexBufferPtr> vertexBuffers;
	if (mpOptions->CreateInterleavedVertexBuffers)
	{
		VertexBuffer* pVertexBuffer = ParseVertexBuffer(pXmlNode);
		vertexBuffers.Append(pVertexBuffer);
	}
	else
	{
		ParseVertexBuffers(pXmlNode, vertexBuffers);
	}

	IndexBuffer* pIndexBuffer = ParseIndexBuffer(pXmlNode);

	Mesh* pMesh = WIRE_NEW Mesh(vertexBuffers, pIndexBuffer);
	mStatistics.MeshCount++;
	TArray<MeshPtr> subMeshes;
	subMeshes.Append(pMesh);

	for (rapidxml::xml_node<>* pSubNode = pXmlNode->first_node();
		pSubNode; pSubNode = pSubNode->next_sibling())
	{
		if (Is("SubMeshes", pSubNode->name()))
		{
			for (rapidxml::xml_node<>* pChild = pSubNode->first_node();
				pChild;	pChild = pChild->next_sibling())
			{
				WIRE_ASSERT(HasValue(pChild, "Index"));
				UInt i = GetUInt(pChild, "Index");
				WIRE_ASSERT(HasValue(pChild, "StartIndex"));
				UInt startIndex = GetUInt(pChild, "StartIndex");
				WIRE_ASSERT(HasValue(pChild, "IndexCount"));
				UInt indexCount = GetUInt(pChild, "IndexCount");

				Mesh* pSubMesh = WIRE_NEW Mesh(vertexBuffers, pIndexBuffer,
					startIndex, indexCount);
				subMeshes.SetElement(i, pSubMesh);

				if (i > 0)
				{
					mStatistics.MeshCount++;
				}
			}
		}
	}

	mMeshes.Insert(pName, subMeshes);
	return subMeshes[subMeshIndex];
}

//----------------------------------------------------------------------------
VertexBuffer* Importer::ParseVertexBuffer(rapidxml::xml_node<>* pXmlNode)
{
	Char* pVerticesFileName = NULL;
	Char* pNormalsFileName = NULL;
	Char* pColorsFileName = NULL;
	TArray<Char*> uvSetNames(8,8);
	Bool isVertexBufferBigEndian = true;
	Bool isNormalsBigEndian = true;
	Bool isColorsBigEndian = true;
	Bool isColors32bit = false;
	TArray<Bool> uvBigEndian(8,8);
	Buffer::UsageType vertexBufferUsage = Buffer::UT_STATIC;

	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		if (Is("Vertices", pChild->name()))
		{
			WIRE_ASSERT(!pVerticesFileName);
			pVerticesFileName = GetValue(pChild, "Name");
			isVertexBufferBigEndian = !IsTrue("LittleEndian", pChild);
			vertexBufferUsage = GetUsageType(pChild);
		}
		else if (Is("Normals", pChild->name()))
		{
			WIRE_ASSERT(!pNormalsFileName);
			pNormalsFileName = GetValue(pChild, "Name");
			isNormalsBigEndian = !IsTrue("LittleEndian", pChild);
		}
		else if (Is("Colors", pChild->name()))
		{
			WIRE_ASSERT(!pColorsFileName);
			pColorsFileName = GetValue(pChild, "Name");
			isColorsBigEndian = !IsTrue("LittleEndian", pChild);
			isColors32bit = IsTrue("32bit", pChild);
		}
		else if (System::Strncmp("Uv", pChild->name(), 2) == 0)
		{
			uvSetNames.Append(GetValue(pChild, "Name"));
			uvBigEndian.Append(!IsTrue("LittleEndian", pChild));
		}
	}

	if (!pVerticesFileName)
	{
		WIRE_ASSERT(false /* Mesh has no vertices */);
		return NULL;
	}

	VertexBuffer* pVertexBuffer = LoadVertexBufferFromFiles(pVerticesFileName,
		isVertexBufferBigEndian, vertexBufferUsage, pNormalsFileName,
		isNormalsBigEndian, pColorsFileName, isColorsBigEndian, isColors32bit,
		uvSetNames, uvBigEndian);

	return pVertexBuffer;
}

//----------------------------------------------------------------------------
void Importer::ParseVertexBuffers(rapidxml::xml_node<>* pXmlNode,
	TArray<VertexBufferPtr>& rVertexBuffers)
{
	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		VertexAttributes va;
		if (Is("Vertices", pChild->name()))
		{
			va.SetPositionChannels(3);
			VertexBuffer* pVertexBuffer = LoadVertexBuffer(pChild, va);
			rVertexBuffers.Append(pVertexBuffer);
		}
		else if (Is("Normals", pChild->name()))
		{
			va.SetNormalChannels(3);
			VertexBuffer* pVertexBuffer = LoadVertexBuffer(pChild, va);
			rVertexBuffers.Append(pVertexBuffer);
		}
		else if (Is("Colors", pChild->name()))
		{
			va.SetColorChannels(4);
			VertexBuffer* pVertexBuffer = LoadVertexBuffer(pChild, va);
			rVertexBuffers.Append(pVertexBuffer);
		}
		else if (System::Strncmp("Uv", pChild->name(), 2) == 0)
		{
			va.SetTCoordChannels(2);
			VertexBuffer* pVertexBuffer = LoadVertexBuffer(pChild, va);
			rVertexBuffers.Append(pVertexBuffer);
		}
	}

	WIRE_ASSERT(rVertexBuffers.GetQuantity() > 0);
}

//----------------------------------------------------------------------------
IndexBuffer* Importer::ParseIndexBuffer(rapidxml::xml_node<>* pXmlNode)
{
	for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
		pChild = pChild->next_sibling())
	{
		if (Is("Indices", pChild->name()))
		{
			IndexBuffer* pIndexBuffer = LoadIndexBufferFromFile(GetValue(
				pChild, "Name"), !IsTrue("LittleEndian", pChild),
				GetUsageType(pChild), IsTrue("16bit", pChild));
			return pIndexBuffer;
		}
	}

	WIRE_ASSERT(false);
	return NULL;
}

//----------------------------------------------------------------------------
Material* Importer::ParseMaterial(rapidxml::xml_node<>* pXmlNode)
{
	Char* pName = GetValue(pXmlNode, "Name");
	if (!pName)
	{
		WIRE_ASSERT(false /* Material has no name */);
		return NULL;
	}

	if (mpOptions->AssetsWithEqualNamesAreIdentical)
	{
		MaterialPtr* pValue = mMaterials.Find(pName);
		if (pValue)
		{
			WIRE_ASSERT(*pValue);
			return *pValue;
		}
	}

	Material* pMaterial = WIRE_NEW Material;
	if (pXmlNode->first_node())
	{
		for (rapidxml::xml_node<>* pChild = pXmlNode->first_node(); pChild;
			pChild = pChild->next_sibling())
		{
			if (Is("Texture", pChild->name()))
			{
				Material::BlendMode bm = Material::BM_QUANTITY;
				Texture2D* pTex = ParseTexture(pChild, bm);

				if (bm == Material::BM_QUANTITY)
				{
					bm = pMaterial->GetTextureQuantity() > 0 ?
						Material::BM_MODULATE : Material::BM_REPLACE;
				}

				pMaterial->AddTexture(pTex, bm);
			}
			else
			{
				State* pState = ParseRenderStates(pChild);
				if (pState)
				{
					TArray<StatePtr>* pStateList = mMaterialStates.Find(pMaterial);
					if (pStateList)
					{
						for (UInt i = 0; i < pStateList->GetQuantity(); i++)
						{
							if ((*pStateList)[i]->GetStateType() == pState->
								GetStateType())
							{
								WIRE_ASSERT(false /* multiple States of the same type encountered */);
							}
						}

						pStateList->Append(pState);
					}
					else
					{
						TArray<StatePtr> stateList;
						stateList.Append(pState);
						mMaterialStates.Insert(pMaterial, stateList);
					}
				}
			}
		}
	}

	mMaterials.Insert(pName, pMaterial);
	mStatistics.MaterialCount++;

	return pMaterial;
}

//----------------------------------------------------------------------------
Texture2D* Importer::ParseTexture(rapidxml::xml_node<>* pXmlNode,
	Material::BlendMode& rBlendMode)
{
	Char* pName = GetValue(pXmlNode, "Name");
	if (!pName)
	{
		WIRE_ASSERT(false /* No Texture filename found */);
		return NULL;
	}

	for (rapidxml::xml_attribute<>* attr = pXmlNode->first_attribute();	attr;
		attr = attr->next_attribute())
	{
		if (Is("BlendMode", attr->name()))
		{
			if (Is("Replace", attr->value()))
			{
				rBlendMode = Material::BM_REPLACE;
				break;
			}
			else if (Is("Modulate", attr->value()))
			{
				rBlendMode = Material::BM_MODULATE;
				break;
			}
			else if (Is("Pass", attr->value()))
			{
				rBlendMode = Material::BM_PASS;
				break;
			}
			else if (Is("Blend", attr->value()))
			{
				rBlendMode = Material::BM_BLEND;
				break;
			}
			else if (Is("Decal", attr->value()))
			{
				rBlendMode = Material::BM_DECAL;
				break;
			}
			else if (Is("Add", attr->value()))
			{
				rBlendMode = Material::BM_ADD;
				break;
			}
		}
	}

	if (mpOptions->AssetsWithEqualNamesAreIdentical)
	{
		Texture2DPtr* pValue = mTextures.Find(pName);
		if (pValue)
		{
			WIRE_ASSERT(*pValue);
			return *pValue;
		}
	}

	UInt mipmapCount = 1;
	Texture2D::FilterType filter = Texture2D::FT_LINEAR_LINEAR;
	Texture2D::WrapType warp = Texture2D::WT_CLAMP;
	UInt anisoLevel = 0;
	Buffer::UsageType usage = GetUsageType(pXmlNode);

	for (rapidxml::xml_attribute<>* attr = pXmlNode->first_attribute();	attr;
		attr = attr->next_attribute())
	{
		if (Is("Mipmaps", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%d", &mipmapCount);
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);	
		}
		else if (Is("FilterMode", attr->name()))
		{
			if (Is("Point", attr->value()))
			{
				filter = Texture2D::FT_NEAREST_NEAREST;
			}
			else if (Is("Bilinear", attr->value()))
			{
				filter = Texture2D::FT_LINEAR_NEAREST;
			}
			else if (Is("Trilinear", attr->value()))
			{
				filter = Texture2D::FT_LINEAR_LINEAR;
			}
		}
		else if (Is("AnisoLevel", attr->name()))
		{
			Int n;
			n = sscanf(attr->value(), "%d", &anisoLevel);
			WIRE_ASSERT_NO_SIDEEFFECTS(n == 1);	
		}
		else if (Is("WrapMode", attr->name()))
		{
			if (Is("Repeat", attr->value()))
			{
				warp = Texture2D::WT_REPEAT;
			}
			else if (Is("Clamp", attr->value()))
			{
				warp = Texture2D::WT_CLAMP;
			}
		}
	}

	String path = String(mpPath) + String(pName);
	Image2D* pImage = LoadPNG(static_cast<const Char*>(path),
		(mipmapCount > 1), usage, mipmapCount);
	if (!pImage)
	{
		return NULL;
	}

	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	mStatistics.TextureCount++;
	if (mipmapCount == 1)
	{
		if (filter == Texture2D::FT_NEAREST_NEAREST)
		{
			filter = Texture2D::FT_NEAREST;
		}
		else
		{
			filter = Texture2D::FT_LINEAR;
		}
	}

	pTexture->SetFilterType(filter);
	pTexture->SetWrapType(0, warp);
	pTexture->SetWrapType(1, warp);
	pTexture->SetAnisotropyValue(static_cast<Float>(anisoLevel));

	mTextures.Insert(pName, pTexture);

	return pTexture;
}

//----------------------------------------------------------------------------
Bool Importer::Is(const Char* pSrc, const Char* pDst)
{
	return (System::Strcmp(pSrc, pDst) == 0);
}

//----------------------------------------------------------------------------
VertexBuffer* Importer::LoadVertexBuffer(rapidxml::xml_node<>* pXmlNode,
	VertexAttributes& rVA)
{
	if (rVA.HasPosition())
	{
		WIRE_ASSERT(rVA.GetChannelQuantity() == rVA.GetPositionChannels());
	}

	if (rVA.HasNormal())
	{
		WIRE_ASSERT(rVA.GetChannelQuantity() ==	rVA.GetNormalChannels());
	}

	if (rVA.HasTCoord())
	{
		WIRE_ASSERT(rVA.GetChannelQuantity() == rVA.GetTCoordChannels());
	}

	Char* pFileName = GetValue(pXmlNode, "Name");
	Bool isBigEndian = !IsTrue("LittleEndian", pXmlNode);
	Buffer::UsageType usage = GetUsageType(pXmlNode);

	Int size;
	Float* const pChannel = Load32(pFileName, size, isBigEndian);
	
	VertexBuffer* pVertexBuffer = NULL;
	mStatistics.VertexBufferCount++;

	if (!rVA.HasColor())
	{
		const UInt count = size/(rVA.GetChannelQuantity()*sizeof(Float));
		pVertexBuffer = WIRE_NEW VertexBuffer(pChannel, rVA, count, usage);
	}
	else
	{
		if (IsTrue("32bit", pXmlNode))
		{
			const UInt count = size/(rVA.GetChannelQuantity()*sizeof(UInt));
			if (WIRE_RGBA_R != 0 || WIRE_RGBA_G != 1 || WIRE_RGBA_B != 2 ||
				WIRE_RGBA_A != 3)
			{
				WIRE_ASSERT(sizeof(UInt) == 4);
				UChar* pRGBA = reinterpret_cast<UChar*>(pChannel);
				if (System::IsBigEndian())
				{
					for (UInt i = 0; i < count; i++)
					{
						UChar r = pRGBA[i*4];
						UChar g = pRGBA[i*4+1];
						UChar b = pRGBA[i*4+2];
						UChar a = pRGBA[i*4+3];
						pRGBA[i*4 + WIRE_RGBA_R] = r;
						pRGBA[i*4 + WIRE_RGBA_G] = g;
						pRGBA[i*4 + WIRE_RGBA_B] = b;
						pRGBA[i*4 + WIRE_RGBA_A] = a;
					}
				}
				else
				{
					for (UInt i = 0; i < count; i++)
					{
						UChar r = pRGBA[i*4+3];
						UChar g = pRGBA[i*4+2];
						UChar b = pRGBA[i*4+1];
						UChar a = pRGBA[i*4];
						pRGBA[i*4 + WIRE_RGBA_R] = r;
						pRGBA[i*4 + WIRE_RGBA_G] = g;
						pRGBA[i*4 + WIRE_RGBA_B] = b;
						pRGBA[i*4 + WIRE_RGBA_A] = a;
					}
				}
			}

			pVertexBuffer = WIRE_NEW VertexBuffer(pChannel, rVA, count, usage);
		}
		else
		{
			const UInt count = size/(4*sizeof(Float));
			pVertexBuffer = WIRE_NEW VertexBuffer(rVA, count, usage);

			Float* pTempChannel = pChannel;
			for (UInt i = 0; i < count; i++)
			{
				WIRE_ASSERT(rVA.GetChannelQuantity() == 1);

				ColorRGBA c;
				c.R() = *pTempChannel++;
				c.G() = *pTempChannel++;
				c.B() = *pTempChannel++;
				c.A() = *pTempChannel++;
				pVertexBuffer->Color4(i) = c;
			}

			Free32(pChannel);
		}
	}

	return pVertexBuffer;
}

//----------------------------------------------------------------------------
VertexBuffer* Importer::LoadVertexBufferFromFiles(Char* pFileName, Bool
	isVertexBufferBigEndian, Buffer::UsageType vertexBufferUsage, 
	Char* pNormalsName, Bool isNormalsBigEndian, Char* pColorsName, Bool
	isColorsBigEndian, Bool isColors32Bit, TArray<Char*>& rUvSetNames,
	TArray<Bool>& rUvBigEndian)
{
	VertexAttributes vertexAttributes;
	vertexAttributes.SetPositionChannels(3);
	Int verticesSize;
	Float* pVertices = Load32(pFileName, verticesSize, isVertexBufferBigEndian);

	Int normalsSize;
	Float* pNormals = NULL;
	if (pNormalsName)
	{
		vertexAttributes.SetNormalChannels(3);
		pNormals = Load32(pNormalsName, normalsSize, isNormalsBigEndian);
		if (verticesSize != normalsSize)
		{
			WIRE_ASSERT(false /* Vertices and normals do not match */);
			Free32(pVertices);
			return NULL;
		}
	}

	Int colorsSize;
	Float* pColors = NULL;
	Char* pColors32 = NULL;
	if (pColorsName)
	{
		vertexAttributes.SetColorChannels(4);
		Int vbSize = verticesSize/(3*sizeof(Float));
		if (isColors32Bit)
		{
			String path = String(mpPath) + String(pColorsName);
			pColors32 = Load(static_cast<const Char*>(path), colorsSize);
		}
		else
		{
			pColors = Load32(pColorsName, colorsSize, isColorsBigEndian);
		}

		if (vbSize != (colorsSize/(4*sizeof(Float))))
		{
			if (isColors32Bit && (vbSize != (colorsSize/4)))
			{
				WIRE_ASSERT(false /* Vertices and colors do not match */);
				Free32(pNormals);
				Free32(pVertices);
				return NULL;
			}
		}
	}

	TArray<Float*> uvSets(rUvSetNames.GetQuantity());
	TArray<Int> uvSetSizes(rUvSetNames.GetQuantity());
	for (UInt i = 0; i < rUvSetNames.GetQuantity(); i++)
	{
		uvSetSizes.Append(0);
		uvSets.Append(Load32(rUvSetNames[i], uvSetSizes[i], rUvBigEndian[i]));
		vertexAttributes.SetTCoordChannels(2, i);
		if (verticesSize / (3 * sizeof(Float)) != uvSetSizes[i] / (2 * sizeof(Float)))
		{
			WIRE_ASSERT(false /* vertices and uv sets do not match */);
			for (UInt j = 0; j <= i; j++)
			{
				Free32(uvSets[j]);
			}

			WIRE_DELETE[] pColors32;
			Free32(pColors);
			Free32(pNormals);
			Free32(pVertices);
			return NULL;
		}	
	}

	VertexBuffer* pVertexBuffer = WIRE_NEW VertexBuffer(vertexAttributes,
		verticesSize / (3 * sizeof(Float)), vertexBufferUsage);
	mStatistics.VertexBufferCount++;

	Float* pTempVertices = pVertices;
	Float* pTempNormals = pNormals;
	Float* pTempColors = pColors;
	Char* pTempColors32 = pColors32;
	for (UInt i = 0; i < (verticesSize/(3*sizeof(Float))); i++)
	{
		if (pVertices)
		{
			Vector3F v;
			v.X() = *pTempVertices++;
			v.Y() = *pTempVertices++;
			v.Z() = *pTempVertices++;
			pVertexBuffer->Position3(i) = v;
		}

		if (pNormals)
		{
			Vector3F n;
			n.X() = *pTempNormals++;
			n.Y() = *pTempNormals++;
			n.Z() = *pTempNormals++;
			pVertexBuffer->Normal3(i) = n;
		}

		if (pColors32)
		{
			Color32 c;
			c.R() = *pTempColors32++;
			c.G() = *pTempColors32++;
			c.B() = *pTempColors32++;
			c.A() = *pTempColors32++;
			pVertexBuffer->Color4(i) = c;
		}
		else if (pColors)
		{
			ColorRGBA c;
			c.R() = *pTempColors++;
			c.G() = *pTempColors++;
			c.B() = *pTempColors++;
			c.A() = *pTempColors++;
			pVertexBuffer->Color4(i) = c;
		}

		for (UInt j = 0; j < uvSets.GetQuantity(); j++)
		{
			Vector2F v(uvSets[j][i*2], uvSets[j][i*2+1]);
			pVertexBuffer->TCoord2(i, j) = v;
		}
	}

	for (UInt i = 0; i < uvSets.GetQuantity(); i++)
	{
		Free32(uvSets[i]);
	}

	WIRE_DELETE[] pColors32;
	Free32(pColors);
	Free32(pNormals);
	Free32(pVertices);

	return pVertexBuffer;
}

//----------------------------------------------------------------------------
IndexBuffer* Importer::LoadIndexBufferFromFile(Char* pFileName, Bool
	isIndexBufferBigEndian, Buffer::UsageType indexBufferUsage, Bool is16Bit)
{
	Int indicesSize;

	if (is16Bit)
	{
		UShort* pIndices = Load16(pFileName, indicesSize,
			isIndexBufferBigEndian);
		if (!pIndices)
		{
			return NULL;
		}

		IndexBuffer* pIndexBuffer = WIRE_NEW IndexBuffer(pIndices,
			indicesSize/sizeof(UShort), indexBufferUsage);
		mStatistics.IndexBufferCount++;
		return pIndexBuffer;
	}

	UInt* pIndices = reinterpret_cast<UInt*>(Load32(pFileName, indicesSize,
		isIndexBufferBigEndian));
	if (!pIndices)
	{
		return NULL;
	}

	IndexBuffer* pIndexBuffer = WIRE_NEW IndexBuffer(indicesSize/sizeof(UInt),
		indexBufferUsage);
	mStatistics.IndexBufferCount++;
	for (UInt i = 0; i < indicesSize/sizeof(UInt); i++)
	{
		WIRE_ASSERT(pIndices[i] < 65536);
		(*pIndexBuffer)[i] = static_cast<UShort>(pIndices[i]);
	}

	UChar* pUChar = reinterpret_cast<UChar*>(pIndices);
	WIRE_DELETE[] pUChar;

	return pIndexBuffer;
}
