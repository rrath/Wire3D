// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireLight.h"
#include "WireGXRendererData.h"

using namespace Wire;

//----------------------------------------------------------------------------
void Renderer::SetLight(const Light* pLight, UInt unit)
{
	WIRE_ASSERT(unit < PdrRendererData::MAXLIGHTS);
	mLights[unit] = const_cast<Light*>(pLight);

	Bool useLight = pLight ? pLight->Enabled : false;
	if (!useLight)
	{
		mpData->LightsMask &= ~(1<<unit);
		return;
	}

	GXLightObj gxLight;
	const ColorRGB& rCol = pLight->Color;
	const GXColor diffuse = { rCol.R()*255, rCol.G()*255, rCol.B()*255,
		255 };
	GXInitLightColor(&gxLight, diffuse);

	switch (pLight->Type)
	{
	case Light::LT_POINT:
	{
		Light* pL = const_cast<Light*>(pLight);
		Vec* pLightPos = reinterpret_cast<Vec*>(&pL->Position);
		Vec lightViewPos;
		MTXMultVec(mpData->View, pLightPos, &lightViewPos);
		GXInitLightPosv(&gxLight, &lightViewPos);
		GXInitLightSpot(&gxLight, 0.0F, GX_SP_OFF);
		GXInitLightAttnK(&gxLight, pLight->Constant, pLight->Linear, pLight->Quadric);
		break;
	}

	case Light::LT_DIRECTIONAL:
	{
		Light* pL = const_cast<Light*>(pLight);
		Vec lightDir = *(reinterpret_cast<Vec*>(&pL->Direction));
		static Vec s_zero = {0, 0, 0};
		VECSubtract(&s_zero, &lightDir, &lightDir);
		lightDir.x = lightDir.x * 1000000;
		lightDir.y = lightDir.y * 1000000;
		lightDir.z = lightDir.z * 1000000;
		MTXMultVec(mpData->View, &lightDir, &lightDir);
		GXInitLightPosv(&gxLight, &lightDir);
		GXInitLightSpot(&gxLight, 0.0F, GX_SP_OFF);
		GXInitLightDistAttn(&gxLight, 20, 1, GX_DA_OFF);
		break;
	}

	case Light::LT_SPOT:
	{
		Light* pL = const_cast<Light*>(pLight);

		Vec* pLightPos = reinterpret_cast<Vec*>(&pL->Position);
		Vec lightViewPos;
		MTXMultVec(mpData->View, pLightPos, &lightViewPos);
		GXInitLightPosv(&gxLight, &lightViewPos);

		Vec* pLightDir = reinterpret_cast<Vec*>(&pL->Direction);
		Vec lightViewDir;
		MTXMultVecSR(mpData->View, pLightDir, &lightViewDir);
		GXInitLightDirv(&gxLight, &lightViewDir);
		
		Float angle = pL->Angle * MathF::RAD_TO_DEG * 0.5F;
		GXInitLightSpot(&gxLight, angle, GX_SP_COS2);
		GXInitLightAttnK(&gxLight, pLight->Constant, pLight->Linear, pLight->Quadric);
		break;
	}

	default:
		WIRE_ASSERT(false);
	}

// 		Light* pL = const_cast<Light*>(pLight);
// 		Vec* pLightDir = reinterpret_cast<Vec*>(&pL->Direction);
// 		Vec lightViewDir;
// 		MTXMultVecSR(mpData->View, pLightDir, &lightViewDir);
// 		GXInitSpecularDirv(&gxLight, &lightViewDir);
// 		GXInitLightShininess(&gxLight, 4);
//		GXInitLightSpot(&gxLight, 0.0F, GX_SP_OFF);

	GXLoadLightObjImm(&gxLight, 1 << unit);

	mpData->LightsMask |= 1<<unit;
}

//----------------------------------------------------------------------------
void Renderer::EnableLighting(const ColorRGB& rAmbient)
{
	const GXColor amb = { rAmbient.R()*255, rAmbient.G()*255,
		rAmbient.B()*255, 255 };
	GXSetChanAmbColor(GX_COLOR0A0, amb);

	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
}

//----------------------------------------------------------------------------
void Renderer::DisableLighting()
{
	mpData->LightsMask = 0;
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX,
		GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
}
