// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderText.h"

#include "WireBoundingVolume.h"
#include "WireIndexBuffer.h"
#include "WireMesh.h"
#include "WireRenderer.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, RenderText, RenderObject);

//----------------------------------------------------------------------------
RenderText::RenderText(UInt fontHeight, Texture2D* pFontTexture,
	TArray<Vector2F>& rUvs, TArray<Vector4F>& rCharSizes, UInt maxLength)
	:
	mUvs(rUvs),
	mCharSizes(rCharSizes),
	mColor(Color32::WHITE),
	mFontHeight(static_cast<Float>(fontHeight)),
	mLineWidth(MathF::MAX_REAL),
	mWhitespaceWidth(mCharSizes[' '].Z()),
	mIsPdrBufferOutOfDate(true)
{
	Init(pFontTexture, maxLength);
}

//----------------------------------------------------------------------------
RenderText::RenderText(const RenderText* pText, UInt maxLength)
	:
	mUvs(pText->mUvs),
	mCharSizes(pText->mCharSizes),
	mColor(pText->mColor),
	mFontHeight(pText->mFontHeight),
	mLineWidth(pText->mLineWidth),
	mPenX(pText->mPenX),
	mPenY(pText->mPenY),
	mWhitespaceWidth(pText->mWhitespaceWidth),
	mIsPdrBufferOutOfDate(true)
{
	WIRE_ASSERT(pText);
	Texture2D* pFontTexture = NULL;
	const Material* pMaterial = pText->GetMaterial();
	if (pMaterial && pMaterial->GetTextureQuantity() > 0)
	{
		pFontTexture = pMaterial->GetTexture();
	}

	Init(pFontTexture, maxLength);
}

//----------------------------------------------------------------------------
void RenderText::Init(Texture2D* pFontTexture, UInt maxLength)
{
	WIRE_ASSERT(mUvs.GetQuantity() == mCharSizes.GetQuantity()*4);
	WIRE_ASSERT(maxLength < (0x10000/4));

	VertexAttributes attr;
	attr.SetPositionChannels(3);
	attr.SetTCoordChannels(2);
	attr.SetColorChannels(4);
	VertexBuffer* pVertexBuffer = WIRE_NEW VertexBuffer(attr, maxLength*4,
		Buffer::UT_DYNAMIC_WRITE_ONLY);

	IndexBuffer* pIndexBuffer = WIRE_NEW IndexBuffer(maxLength*3*2);
	for (UShort i = 0; i < maxLength; i++)
	{
		(*pIndexBuffer)[i*6] = 0 + i*4;
		(*pIndexBuffer)[i*6+1] = 1 + i*4;
		(*pIndexBuffer)[i*6+2] = 2 + i*4;
		(*pIndexBuffer)[i*6+3] = 0 + i*4;
		(*pIndexBuffer)[i*6+4] = 2 + i*4;
		(*pIndexBuffer)[i*6+5] = 3 + i*4;
	}

	Mesh* pMesh = WIRE_NEW Mesh(pVertexBuffer, pIndexBuffer);
	SetMesh(pMesh);

	Clear();

	Material* pMaterial = WIRE_NEW Material;
	pMaterial->AddTexture(pFontTexture);
	SetMaterial(pMaterial);

	State::Init(GetStates());

	StateAlpha* pAlpha = WIRE_NEW StateAlpha;
	pAlpha->BlendEnabled = true;
	GetStates()[State::ALPHA] = pAlpha;
}

//----------------------------------------------------------------------------
RenderText::~RenderText()
{
}

//----------------------------------------------------------------------------
void RenderText::Clear()
{
	WIRE_ASSERT(GetMesh());
	GetMesh()->SetIndexCount(0);
	GetMesh()->GetModelBound()->SetCenter(Vector3F::ZERO);
	GetMesh()->GetModelBound()->SetRadius(0);
	mPenX = 0.0F;
	mPenY = 0.0F;
}

//----------------------------------------------------------------------------
void RenderText::Clear(const Color32& rColor)
{
	mColor = rColor;
	Clear();
}

//----------------------------------------------------------------------------
void RenderText::SetPen(Float x, Float y)
{
	mPenX = x;
	mPenY = y;
}

//----------------------------------------------------------------------------
void RenderText::SetColor(const Color32& rColor)
{
	mColor = rColor;
}

//----------------------------------------------------------------------------
Bool RenderText::Set(const Char* pText, Float x, Float y)
{
	Clear();
	return Append(pText, x, y);
}

//----------------------------------------------------------------------------
Bool RenderText::Set(const Char* pText, const Color32& rColor, Float x, Float y)
{
	mColor = rColor;
	return Set(pText, x, y);
}

//----------------------------------------------------------------------------
Bool RenderText::Append(const Char* pText)
{
	return Append(pText, mPenX, mPenY);
}

//----------------------------------------------------------------------------
Bool RenderText::Append(const Char* pText, const Color32& rColor)
{
	mColor = rColor;
	return Append(pText);
}

//----------------------------------------------------------------------------
Bool RenderText::Append(const Char* pText, Float x, Float y)
{
	if (!pText)
	{
		return true;
	}

	VertexBuffer* pVertexBuffer = GetMesh()->GetVertexBuffer();
	WIRE_ASSERT(pVertexBuffer && pVertexBuffer->GetAttributes().HasTCoord());
	const UInt offset = GetMesh()->GetIndexCount()/6;

	const UInt maxLength = pVertexBuffer->GetQuantity() / 4;
	UInt indexCount = 0;

	BoundingVolume* pModelBound = GetMesh()->GetModelBound();
	WIRE_ASSERT(pModelBound);
	Vector2F min(System::MAX_FLOAT, System::MAX_FLOAT);
	Vector2F max(System::MIN_FLOAT, System::MIN_FLOAT);
	Float radius = pModelBound->GetRadius();
	Vector3F center3 = pModelBound->GetCenter();
	if (radius != 0)
	{
		min = Vector2F(center3.X() - radius, center3.Y() - radius);
		max = Vector2F(center3.X() + radius, center3.Y() + radius);
	}

	for (UInt j = 0; pText[j]; j++)
	{
		if ((offset+indexCount) >= maxLength)
		{
			break;
		}

		UInt c = static_cast<UInt>(pText[j]);
		if (c == ' ')
		{
			x+= mWhitespaceWidth;
			continue;
		}

		if (c >= mCharSizes.GetQuantity())		
		{
			return false;
		}

		UInt i = (indexCount + offset) * 4;
		Float cWidth = mCharSizes[c].X();
		Float cHeight = mCharSizes[c].Y();
		Float cStride = mCharSizes[c].Z();
		if ((x+cStride) >= mLineWidth || c == '\n')
		{
			x = 0.0F;
			y -= mFontHeight;

			if (c == '\n')
			{
				continue;
			}
		}

		Float cy = y - mCharSizes[c].W();
		const Float x1 = x + cWidth;
		const Float cy1 = cy + cHeight;

		min.X() = min.X() > x ? x : min.X();
		max.X() = max.X() < x1 ? x1 : max.X();
		min.Y() = min.Y() > cy1 ? cy : min.Y();
		max.Y() = max.Y() < cy1 ? cy1 : max.Y();

		pVertexBuffer->Position3(i) = Vector3F(x, cy1, 0);
		pVertexBuffer->Color4(i) = mColor;
		pVertexBuffer->Position3(i+1) = Vector3F(x1, cy1, 0);
		pVertexBuffer->Color4(i+1) = mColor;
		pVertexBuffer->Position3(i+2) = Vector3F(x1, cy, 0);
		pVertexBuffer->Color4(i+2) = mColor;
		pVertexBuffer->Position3(i+3) = Vector3F(x, cy, 0);
		pVertexBuffer->Color4(i+3) = mColor;

		UInt c4 = c*4;
		pVertexBuffer->TCoord2(i) = mUvs[c4];
		pVertexBuffer->TCoord2(i+1) = mUvs[c4+1];
		pVertexBuffer->TCoord2(i+2) = mUvs[c4+2];
		pVertexBuffer->TCoord2(i+3) = mUvs[c4+3];

		x+= cStride;
		indexCount++;
	}

	GetMesh()->SetIndexCount(GetMesh()->GetIndexCount() + indexCount*6);
	mIsPdrBufferOutOfDate = true;

	Vector2F center((max.X()+min.X()) * 0.5F, (max.Y()+min.Y()) * 0.5F);
	pModelBound->SetCenter(Vector3F(center.X(), center.Y(), 0));
	pModelBound->SetRadius((center - min).Length());

	mPenX = x;
	mPenY = y;

	return true;
}

//----------------------------------------------------------------------------
Bool RenderText::Append(const Char* pText, const Color32& rColor, Float x,
	Float y)
{
	mColor = rColor;
	return Append(pText, x, y);
}

//----------------------------------------------------------------------------
void RenderText::Update(Renderer* pRenderer)
{
	if (!mIsPdrBufferOutOfDate)
	{
		return;
	}

	const UInt indexCount = GetMesh()->GetIndexCount();
	WIRE_ASSERT(indexCount % 6 == 0);
	pRenderer->Update(GetMesh()->GetVertexBuffer(), indexCount/6*4);

	mIsPdrBufferOutOfDate = false;
}

//----------------------------------------------------------------------------
Float RenderText::GetFontHeight()
{
	return mFontHeight;
}

//----------------------------------------------------------------------------
void RenderText::SetLineWidth(Float lineWidth)
{
	mLineWidth = lineWidth;
}

//----------------------------------------------------------------------------
void RenderText::SetLineWidth(UInt lineWidth)
{
	mLineWidth = static_cast<Float>(lineWidth);
}

//----------------------------------------------------------------------------
void RenderText::SetWhitespaceWidth(Float whitespaceWidth)
{
	mWhitespaceWidth = whitespaceWidth;
}
