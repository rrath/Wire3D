// Sample4 - Multitexturing
// This sample demonstrates how to blend individual textures together in
// a single rendering pass using multi-texturing.

#include "Sample4.h"

using namespace Wire;

WIRE_APPLICATION(Sample4);

//----------------------------------------------------------------------------
Bool Sample4::OnInitialize()
{
	if (!Parent::OnInitialize())
	{
		return false;
	}

	// create a cube with vertex colors using a texture effect with 2 textures
	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(CreateTexture());
	pMaterial->AddTexture(CreateTexture2());

	mspCube = StandardMesh::CreateCube14(3, pMaterial->GetTextureQuantity());
	mspCube->SetMaterial(pMaterial);

	// camera and viewing frustum setup
	Vector3F cameraLocation(0.0F, 0.0F, -7.0F);
	Vector3F viewDirection(0.0F, 0.0F, 1.0F);
	Vector3F up(0.0F, 1.0F, 0.0F);
	Vector3F right = viewDirection.Cross(up);
	mspCamera = WIRE_NEW Camera;
	mspCamera->SetFrame(cameraLocation, viewDirection, up, right);
	mspCamera->SetFrustum(45, GetWidthF() / GetHeightF() , 0.1F, 300.0F);

	// initializing working variables
	mAngle = 0.0F;
	mLastTime = System::GetTime();

	return true;
}

//----------------------------------------------------------------------------
void Sample4::OnIdle()
{
	Double time = System::GetTime();
	Double elapsedTime = time - mLastTime;
	mLastTime = time;

	// rotate the cube based on the time passed	
	mAngle += static_cast<Float>(elapsedTime) * 0.5F;
	mAngle = MathF::FMod(mAngle, MathF::TWO_PI);
	Vector3F axis(Vector3F(MathF::Sin(mAngle), MathF::Cos(mAngle), 0.3F));
	Matrix34F rotation(axis, mAngle);
	Transformation transformation;
	transformation.SetRotate(rotation);

	// define some blending modes for the cube to cycle through
	static const BlendMode modes[] =
	{
		// texture0 passes the previous stage (i.e. vertex color),
		// so does texture1, thus only the vertex color is visible
		BlendMode(Material::BM_PASS, Material::BM_PASS),

		// texture0 replaces the vertex color, texture1 passes the
		// the previous stage, thus only texture0 is visible
		BlendMode(Material::BM_REPLACE, Material::BM_PASS),

		// texture0 blends with the vertex color
		BlendMode(Material::BM_BLEND, Material::BM_PASS),

		// texture0 replaces the vertex color, texture1 is applied as a decal
		// over the previous stage (i.e. texture0)
		BlendMode(Material::BM_REPLACE, Material::BM_DECAL),

		// and so on... check WireMaterial.h for the formulas
		BlendMode(Material::BM_REPLACE, Material::BM_BLEND),
		BlendMode(Material::BM_MODULATE, Material::BM_BLEND),
		BlendMode(Material::BM_BLEND, Material::BM_BLEND)
	};

	// we change the blending mode every second
	UInt modeQuantity = sizeof(modes) / sizeof(BlendMode);
	UInt modeIndex = static_cast<UInt>(MathF::FMod(static_cast<Float>(time),
		static_cast<Float>(modeQuantity)));
	WIRE_ASSERT(modeIndex < modeQuantity);
	Material* pMaterial = mspCube->GetMaterial();
	if (pMaterial)
	{
		pMaterial->SetBlendMode(modes[modeIndex].Mode0, 0);
		pMaterial->SetBlendMode(modes[modeIndex].Mode1, 1);
	}

	GetRenderer()->ClearBuffers();
	GetRenderer()->PreDraw(mspCamera);
	GetRenderer()->Draw(mspCube, transformation);
	GetRenderer()->PostDraw();
	GetRenderer()->DisplayBackBuffer();
}

//----------------------------------------------------------------------------
Texture2D* Sample4::CreateTexture()
{
	const UInt width = 512;
	const UInt height = 128;

	UChar* pMap = WIRE_NEW UChar[width * height];
	UInt seed = 7;
	for (UInt x = 0; x < width; x++)
	{
		pMap[x] = seed;
	}

	for (UInt i = width; i < (width * height)-1; i++)
	{
		seed *= 0x06255;
		seed = (seed >> 7) | (seed << (32-7));
		UInt val = (7 & seed) - 1;
		UChar texel = (pMap[i-width] + pMap[i-width+1]) >> 1;
		pMap[i] = val + texel;
	}

	Image2D::FormatMode format = Image2D::FM_RGB565;
	const UInt bytesPerPixel = Image2D::GetBytesPerPixel(format);

	UChar* pData = WIRE_NEW UChar[width * height * bytesPerPixel];
	UChar* pDst = pData;

	for (UInt y = 0; y < height; y++)
	{
		UInt temp;
		for (UInt x = 0; x < width; x++)
		{
			UChar t = pMap[y*width+x];
			UChar* pTemp = reinterpret_cast<UChar*>(&temp);
			*pTemp++ = t;
			*pTemp++ = t;
			*pTemp++ = t;

			Image2D::RGB888ToRGB565(pTemp - 3, pDst);
			pDst += 2;
		}
	}

	WIRE_DELETE[] pMap;

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pData);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	return pTexture;
}

//----------------------------------------------------------------------------
Texture2D* Sample4::CreateTexture2()
{
	// create a 32bit checker board texture with alpha channel
	const UInt width = 64;
	const UInt height = 64;

	Image2D::FormatMode format = Image2D::FM_RGBA8888;
	const UInt bytesPerPixel = Image2D::GetBytesPerPixel(format);

	UChar* pData = WIRE_NEW UChar[width * height * bytesPerPixel];
	UChar* pDst = pData;

	for (UInt y = 0; y < height; y++)
	{
		Bool flip = true;
		if ((y & 0x4) > 0)
		{
			flip = false;
		}

		for (UInt x = 0; x < width; x++)
		{
			UChar t = flip ? 0x40 : 0x80;
			if ((x & 0x4) > 0)
			{
				t = flip ? 0x80 : 0x40;
			}

			*pDst++ = t;
			*pDst++ = t;
			*pDst++ = t;
			*pDst++ = 0x40;
		}
	}

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pData);
	Texture2D* pTexture = WIRE_NEW Texture2D(pImage);
	pTexture->SetFilterType(Texture2D::FT_NEAREST);
	return pTexture;
}
