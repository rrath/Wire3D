// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireGXRendererData.h"

using namespace Wire;

const UChar PdrRendererData::TEX_BLEND[Material::BM_QUANTITY] =
{
	GX_REPLACE,			// Material::BM_REPLACE
	GX_MODULATE,		// Material::BM_MODULATE
	GX_PASSCLR,			// Material::BM_PASS
	GX_BLEND,			// Material::BM_BLEND
	GX_DECAL,			// Material::BM_DECAL
	0,					// Material::BM_ADD
};

//----------------------------------------------------------------------------
void Renderer::EnableTextureStage(Material::BlendMode blendMode, UInt unit,
	Bool hasAlpha)
{
	UInt stage = GX_TEVSTAGE0 + unit;
	if (blendMode != Material::BM_ADD)
	{
		GXSetTevOp(stage, PdrRendererData::TEX_BLEND[blendMode]);
	}
	else
	{
		UChar defColor = GX_CC_RASC;
		UChar defAlpha = GX_CA_RASA;

		if (stage != GX_TEVSTAGE0)
		{
			defColor = GX_CC_CPREV;
			defAlpha = GX_CA_APREV;
		}

		GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_ONE, defColor);
 		GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, GX_CA_KONST, defAlpha);
		GXSetTevKAlphaSel(stage, GX_TEV_KASEL_1);

		GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
		GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	}
}

//----------------------------------------------------------------------------
void Renderer::DisableTextureStage(UInt)
{
}
