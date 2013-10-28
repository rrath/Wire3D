#include "LensflareNode.h"
#include "WireFoundation.h"

using namespace Wire;

// specify the parent of the LensflareNode class
WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(LensflareNode, Node);

// if LensflareNode would use a namespace, we would implement it like this:
//WIRE_IMPLEMENT_RTTI(MyNamespace, LensflareNode, Node);

// Note:
// RTTI is used to determine the type of an object at run-time, e.g.:
//	LensflareNode* pChild = DynamicCast<LensflareNode>(pNode->GetChild(0));
//	if (pChild)
//	...
// If pChild (pNode->GetChild() returns a Spatial*) is a LensflareNode,
// then pChild is a valid pointer, otherwise it is NULL.

// Define the flares using an ID for the texture, flare size, position & color
const Float LensflareNode::s_Scale = 0.015F;
const LensflareNode::FlareDef LensflareNode::s_DefaultDef[] = {
	{ FT_FLARE_5, 4.0F * s_Scale,	1.775F, ColorRGBA(1, 1, 0.8F, 1) },
	{ FT_FLARE_4, 0.25F * s_Scale,	0.7F, ColorRGBA(0.5F, 0.5F, 0.3F, 1) },
	{ FT_FLARE_3, 0.35F * s_Scale,   0.35F, ColorRGBA(0.6F, 0.2F, 0.1F, 1) },
	{ FT_FLARE_4, 0.25F * s_Scale,   -0.2F, ColorRGBA(0.4F, 0.4F, 0.5F, 1) },
	{ FT_FLARE_1, 0.06F * s_Scale, -0.35F, ColorRGBA(0.3F, 1, 1, 1) },
	{ FT_FLARE_4, 0.3F * s_Scale,   -0.45F, ColorRGBA(0.3F, 0.3F, 0.5F, 1) },
	{ FT_FLARE_1, 0.08F * s_Scale,  -0.65F, ColorRGBA(0.3F, 1, 1, 1) },
	{ FT_FLARE_3, 1.1F * s_Scale,   -1.5F, ColorRGBA(0.6F, 0.5F, 0.3F, 1) }
};

//----------------------------------------------------------------------------
LensflareNode::LensflareNode(Spatial* pLightSource)
	:
	mspLightSource(pLightSource)
{
	// Create the flares defined by s_DefaultDef
	CreateFlares();

	// Never let this node be culled by the system, because we determine
	// the visibility of this node and its children (i.e. flares) ourselves.
	Culling = CULL_NEVER;
}

//----------------------------------------------------------------------------
void LensflareNode::GetVisibleSet(Culler& rCuller, Bool noCull)
{
	// This function is called by the Culler (usually after UpdateGS() has
	// transformed local space to world space), so we are dealing with
	// world coordinates here.

	// Determine the visibility of the flares by projecting the
	// 3D light source to 2D space and align them through the center of the
	// screen.
	const Camera* pCam = rCuller.GetCamera();

	Transformation camTrafo;
	Matrix34F matrix(pCam->GetRVector(), pCam->GetUVector(),
		pCam->GetDVector(), pCam->GetLocation());
	camTrafo.SetMatrix(matrix, false);

	const Vector3F& rCamPosition = camTrafo.GetTranslate();
	Vector3F lightPosWorld = mspLightSource->World.GetTranslate();
	Vector3F lightDirection = lightPosWorld - rCamPosition;
	lightDirection.Normalize();

	// viewing frustum left, right, bottom, top, near, far of the camera
	Float l, r, b, t, n, f;
	pCam->GetFrustum(l, r, b, t, n, f);

	Vector4F col0((2*n)/(r-l),	0.0F,			0.0F,			0.0F);
	Vector4F col1(0.0F,			2*n/(t-b),		0.0F,			0.0F);
	Vector4F col2((r+l)/(r-l),	(t+b)/(t-b),	-(f+n)/(f-n),	-1.0F);
	Vector4F col3(0.0F,			0.0F,			-2*(f*n)/(f-n),	0.0F);
	Matrix4F projectionMatrix;
	projectionMatrix.SetColumn(0, col0);
	projectionMatrix.SetColumn(1, col1);
	projectionMatrix.SetColumn(2, col2);
	projectionMatrix.SetColumn(3, col3);

	// transform to camera space
	Vector3F lightPosCamSpace = camTrafo.ApplyInverse(lightPosWorld);

	// transform to screen space
	Vector4F lightPosScreenSpace = projectionMatrix * Vector4F(
		lightPosCamSpace.X(), lightPosCamSpace.Y(), lightPosCamSpace.Z(), 1);

	Float invW = 1.0F / lightPosScreenSpace.W();

	// to screen coordinates [1,1 | -1,-1]
	Vector2F lightPos2D = Vector2F((lightPosScreenSpace.X() * invW * -1),
		(lightPosScreenSpace.Y() * invW * -1));

	// to frustum coordinates [fL - fR | fT - fB]
	lightPos2D[0] *= r;
	lightPos2D[1] *= t;

	Float alpha;
	Float max = 0.9F * t;
	Float min = 0.3F * t;
	Float lightPosY = lightPos2D[1] < 0 ? -lightPos2D[1] : lightPos2D[1];

	// determine the transparency factor of the flare
	if (pCam->GetDVector().Dot(lightDirection) < 0)
	{
		alpha = 0;
	}
	else
	{
		if(lightPosY > max)
		{
			alpha = 1;
		}
		else
		{
			if(lightPosY < min)
			{
				alpha = 0;
			}
			else
			{
				alpha = (lightPosY - min) / (max-min);
			}
		}
	}

	if (alpha > 0 && !rCuller.IsVisible(mspLightSource))
	{
		alpha = 0;
	}

	for (UInt i = 0; i < GetQuantity(); i++)
	{
		Spatial* pFlare = GetChild(i);
		WIRE_ASSERT(pFlare);
		if (alpha > 0.01F)
		{
			pFlare->Culling = CULL_NEVER;
		}
		else
		{
			pFlare->Culling = CULL_ALWAYS;
		}

		StateMaterial* pMaterial = DynamicCast<StateMaterial>(pFlare->
			GetState(State::MATERIAL));
		WIRE_ASSERT(pMaterial);
		pMaterial->Ambient.A() = s_DefaultDef[i].Color.A() * alpha;

		Float position = s_DefaultDef[i].PositionFactor;
		Vector3F newPos = pCam->GetLocation() +
			pCam->GetDVector() * (n + (0.01F * (GetQuantity()-i))) +
			pCam->GetRVector() * lightPos2D[0] * position +
			pCam->GetUVector() * lightPos2D[1] * position;
	
		// set new position
		pFlare->World.SetTranslate(newPos);

		// set rotation from camera
		pFlare->World.SetRotate(camTrafo.GetRotate());
	}

	// call the parent class function to collect the visible children
	// of this node.
	Node::GetVisibleSet(rCuller, noCull);
}

//----------------------------------------------------------------------------
void LensflareNode::CreateFlares()
{
	CreateTextures();
	
	UInt quantity = sizeof(s_DefaultDef) / sizeof(FlareDef);
	for (UInt i = 0; i < quantity; i++)
	{
		Node* pFlare = CreateFlare(s_DefaultDef[i]);
		AttachChild(pFlare);
	}

	// lens flares need to be blended
	StateAlpha* pAlpha = WIRE_NEW StateAlpha;
	pAlpha->BlendEnabled = true;
	AttachState(pAlpha);

	// lens flares do not write the zbuffer
	StateZBuffer* pZBuffer = WIRE_NEW StateZBuffer;
	pZBuffer->Writable = false;
	pZBuffer->Enabled = false;
	AttachState(pZBuffer);

	// we use light and material state to color the lens flare
	Light* pLight = WIRE_NEW Light;
	pLight->Ambient = ColorRGB::WHITE;
	AttachLight(pLight);

	UpdateRS();
}

//----------------------------------------------------------------------------
Node* LensflareNode::CreateFlare(const FlareDef& rDef)
{
	Float uvFactor;
	Float uOffset;
	Float vOffset;
	Texture2D* pTexture = mspLensTex0;

	// There are 5 flare textures stored in 2 actual textures, so we
	// map the type to the appropriate texture pointer and UV set.
	switch (rDef.Type)
	{
	case FT_FLARE_1:
		uvFactor = 0.496F; uOffset = 0.0F; vOffset = 0.0F;
		break;

	case FT_FLARE_2:
		uvFactor = 0.5F; uOffset = 0.501F; vOffset = 0.502F;
		break;

	case FT_FLARE_3:
		uvFactor = 0.5F; uOffset = 0.501F; vOffset = 0.0F;
		break;

	case FT_FLARE_4:
		uvFactor = 0.5F; uOffset = 0.0F; vOffset = 0.5F;
		break;

	case FT_FLARE_5:
		uvFactor = 1.0F; uOffset = 0.0F; vOffset = 0.0F;
		pTexture = mspLensTex1;
		break;

	default:
		WIRE_ASSERT(false);
		return NULL;
	}

	RenderObject* pQuad = StandardMesh::CreateQuad(0, 1, false,
		rDef.SizeFactor);
	VertexBuffer* pVBuffer = pQuad->GetMesh()->GetVertexBuffer();

	for (UInt i = 0; i < pVBuffer->GetQuantity(); i++)
	{
		pVBuffer->TCoord2(i) *= uvFactor;
 		pVBuffer->TCoord2(i).X() += uOffset;
 		pVBuffer->TCoord2(i).Y() += vOffset;
	}

	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(pTexture, Material::BM_MODULATE);
	pQuad->SetMaterial(pMaterial);

	Node* pQuadNode = WIRE_NEW Node(pQuad);

	// material state (used with light) to color the flare
	StateMaterial* pStateMaterial = WIRE_NEW StateMaterial;
	pStateMaterial->Ambient = rDef.Color;
	pQuadNode->AttachState(pStateMaterial);

	// tell the system that we are doing transformations manually
	pQuadNode->WorldIsCurrent = true;
	pQuadNode->WorldBoundIsCurrent = true;

	return pQuadNode;
}

//----------------------------------------------------------------------------
void LensflareNode::CreateTextures()
{
	// Lens flare texture creation was taken from:
	// http://www.blackpawn.com/texts/lensflare/
	//
	// create 4 textures (256,256) and store them in one (1024,1024) texture
	const Image2D::FormatMode format = Image2D::FM_RGBA8888;
	const UInt bpp = Image2D::GetBytesPerPixel(format);
	UInt width = 1024;
	UInt height = 1024;
	Float r = MathF::Min(static_cast<Float>(width/2), static_cast<Float>(
		height/2)) * 0.5F;

	UChar* pDst = WIRE_NEW UChar[width * height * bpp];
	UChar* pDst0 = pDst;
	UChar* pDst1 = pDst + width/2*bpp;
	UChar* pDst2 = pDst + width*height/2*bpp;
	UChar* pDst3 = pDst + width*height/2*bpp + width/2*bpp;

	for (UInt y = 0; y < height/2; y++)
	{
		for (UInt x = 0; x < width/2; x++)
		{
			const Float dx = r-x;
			const Float dy = r-y;
			const Float r1 = MathF::Sqrt(dx*dx + dy*dy) / r;

			// first texture
			Float c = 1-r1;
			c = c*c;
			c *= 1 - SmoothStep(1-0.01F, 1+0.01F, r1);
			UChar c0 = static_cast<UChar>(c * 255.0F);
			*pDst0++ = c0;
			*pDst0++ = c0;
			*pDst0++ = c0;
			*pDst0++ = c0;

			// second texture
			c = r1*r1;
			c = c*c;
			c = c*c*c;
			c *= 1 - SmoothStep(1-0.01F, 1+0.01F, r1);
			UChar c1 = static_cast<UChar>(c * 255.0F);
			*pDst1++ = c1;
			*pDst1++ = c1;
			*pDst1++ = c1;
			*pDst1++ = c1;

			// third texture
			c = r1;
			c *= 1 - SmoothStep(1-0.01F, 1+0.01F, r1);
			UChar c2 = static_cast<UChar>(c * 255.0F);
			*pDst2++ = c2;
			*pDst2++ = c2;
			*pDst2++ = c2;
			*pDst2++ = c2;

			// fourth texture
			c = 1-MathF::FAbs(r1 - 0.9F) / 0.1F;
			if (c < 0)
			{
				c = 0;
			}
			else
			{
				c = c*c;
				c = c*c;
			}

			UChar c3 = static_cast<UChar>(c * 255.0F);
			*pDst3++ = c3;
			*pDst3++ = c3;
			*pDst3++ = c3;
			*pDst3++ = c3;
		}

		pDst0 += width/2*bpp;
		pDst1 += width/2*bpp;
		pDst2 += width/2*bpp;
		pDst3 += width/2*bpp;
	}

	Image2D* pImage = WIRE_NEW Image2D(format, width, height, pDst);
	mspLensTex0 = WIRE_NEW Texture2D(pImage);

	// create a texture by shooting particles from its center
 	width = 256;
 	height = 256;
	r = MathF::Min(static_cast<Float>(width), static_cast<Float>(height))
		* 0.5F;
	pDst = WIRE_NEW UChar[width * height * bpp];
	Float* const pTemp = WIRE_NEW Float[width*height];
	for (UInt i = 0; i < width*height; i++)
	{
		pTemp[i] = 0.0F;
	}

	Random rand;
	for (UInt count = 0; count < 300; count++)
	{
		// pick a random direction
		Float angle = rand.GetFloat() * MathF::TWO_PI;
		Float dx = MathF::Cos(angle);
		Float dy = MathF::Sin(angle);

		// push particle along this path
		Float fx = static_cast<Float>(width) * 0.5F;
		Float fy = static_cast<Float>(height) * 0.5F;

		for (UInt step = 0; step < r; step++)
		{
			DrawParticle(pTemp, fx, fy, width);
			fx += dx;
			fy += dy;
		}
	}

	// normalize
	Float max = 0;
	for (UInt i = 0; i < width*height; i++)
	{
		if (pTemp[i] > max)
		{
			max = pTemp[i];
		}
	}

	max = (1/max) * 500.0F;
	const Vector2F center(width* 0.5F, height*0.5F);
	const Float radius = width * 0.5F;

	for (UInt x = 0; x < width; x++)
	{
		for (UInt y = 0; y < height; y++)
		{
			const UInt i = y*width+x;
			Float f = MathF::Clamp(0, pTemp[i] * max + 150.0F, 255.0F);
			Vector2F pos(static_cast<Float>(x), static_cast<Float>(y));
			Float distance = radius - (center - pos).Length();
			distance = distance < 0 ? 0 : distance;
			Float distFactor = distance / radius;
			UChar val = static_cast<UChar>(f * distFactor);
			pDst[i*bpp] = val;
			pDst[i*bpp+1] = val;
			pDst[i*bpp+2] = val;
			pDst[i*bpp+3] = val;
		}
	}

	WIRE_DELETE[] pTemp;

	pImage = WIRE_NEW Image2D(format, width, height, pDst);
	mspLensTex1 = WIRE_NEW Texture2D(pImage);
}

//----------------------------------------------------------------------------
void LensflareNode::DrawParticle(Float* const pDst, Float fx, Float fy,
	UInt width)
{
	const Int partRadius = 5;

	for (Int y = -partRadius; y < partRadius; y++)
	{
		for (Int x = -partRadius; x < partRadius; x++)
		{
			Float r2 = static_cast<Float>(x*x + y*y);
			Float c = 1 - r2 / static_cast<Float>(partRadius*partRadius);
			c = c*c;
			c = c*c;
			pDst[(y+static_cast<Int>(fy))*width+ x+static_cast<Int>(fx)] += c;
		}
	}
}

//----------------------------------------------------------------------------
Float LensflareNode::SmoothStep(Float a, Float b, Float x)
{
	if (x < a)
	{
		return 0;
	}

	if (x >= b)
	{
		return 1;
	}

	x = (x-a) / (b-a);
	return (x*x) * (3-2*x);
}
