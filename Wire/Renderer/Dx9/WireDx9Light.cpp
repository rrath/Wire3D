// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireRenderer.h"

#include "WireDx9RendererData.h"
#include "WireLight.h"

using namespace Wire;

//----------------------------------------------------------------------------
void Renderer::SetLight(const Light* pLight, UInt unit)
{
	WIRE_ASSERT(mLights.GetQuantity() > unit);
	mLights[unit] = const_cast<Light*>(pLight);

	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;

	Bool useLight = pLight ? pLight->Enabled : false;
	if (!useLight)
	{
		hr = rDevice->LightEnable(unit, FALSE);
		WIRE_ASSERT(SUCCEEDED(hr));
		return;
	}

	hr = rDevice->LightEnable(unit, TRUE);
	WIRE_ASSERT(SUCCEEDED(hr));

	D3DLIGHT9 d3dLight;
	d3dLight.Ambient.r = 0.0F; // pLight->Ambient.R();
	d3dLight.Ambient.g = 0.0F; // pLight->Ambient.G();
	d3dLight.Ambient.b = 0.0F; // pLight->Ambient.B();
	d3dLight.Ambient.a = 1.0F;

	d3dLight.Diffuse.r = pLight->Color.R();
	d3dLight.Diffuse.g = pLight->Color.G();
	d3dLight.Diffuse.b = pLight->Color.B();
	d3dLight.Diffuse.a = 1.0F;

	d3dLight.Specular.r = 1.0F;
	d3dLight.Specular.g = 1.0F;
	d3dLight.Specular.b = 1.0F;
	d3dLight.Specular.a = 1.0F;

	d3dLight.Attenuation0 = pLight->Constant;
	d3dLight.Attenuation1 = pLight->Linear;
	d3dLight.Attenuation2 = pLight->Quadric;

	static Float sqrtMaxReal = MathF::Sqrt(MathF::MAX_REAL);
	d3dLight.Range = sqrtMaxReal;

	d3dLight.Falloff = 1.0F;
	d3dLight.Theta = 0.0F;
	d3dLight.Phi = 0.0F;

	switch (pLight->Type)
	{
	case Light::LT_POINT:
		d3dLight.Type = D3DLIGHT_POINT;
		d3dLight.Position.x = pLight->Position.X();
		d3dLight.Position.y = pLight->Position.Y();
		d3dLight.Position.z = pLight->Position.Z();
		break;

	case Light::LT_DIRECTIONAL:
		d3dLight.Type = D3DLIGHT_DIRECTIONAL;
		WIRE_ASSERT(pLight->Direction.Length() > 0);
		d3dLight.Direction.x = pLight->Direction.X();
		d3dLight.Direction.y = pLight->Direction.Y();
		d3dLight.Direction.z = pLight->Direction.Z();
		break;

	case Light::LT_SPOT:
		d3dLight.Type = D3DLIGHT_SPOT;
		d3dLight.Position.x = pLight->Position.X();
		d3dLight.Position.y = pLight->Position.Y();
		d3dLight.Position.z = pLight->Position.Z();
		d3dLight.Direction.x = pLight->Direction.X();
		d3dLight.Direction.y = pLight->Direction.Y();
		d3dLight.Direction.z = pLight->Direction.Z();
 		d3dLight.Phi = pLight->Angle;
 		d3dLight.Theta = 0.0F;
 		d3dLight.Falloff = pLight->Exponent;
		break;

	default:
		WIRE_ASSERT(false);
	}

	hr = rDevice->SetLight(unit, &d3dLight);
 	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void Renderer::EnableLighting(const ColorRGB& rAmbient)
{
	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;

	PdrRendererData::StateLight& rState = mpData->LightState;
	if (!rState.IsValid)
	{
		hr = rDevice->SetRenderState(D3DRS_AMBIENT, rState.AMBIENT);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetRenderState(D3DRS_LIGHTING, rState.LIGHTING);
		WIRE_ASSERT(SUCCEEDED(hr));

		rState.IsValid = true;
	}

	DWORD ambientColor = D3DCOLOR_COLORVALUE(rAmbient.R(), rAmbient.G(),
		rAmbient.B(), 1.0F);

	if (rState.AMBIENT != ambientColor)
	{
		hr = rDevice->SetRenderState(D3DRS_AMBIENT, ambientColor);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.AMBIENT = ambientColor;
	}

	if (rState.LIGHTING != TRUE)
	{
		hr = rDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.LIGHTING = TRUE;
	}

// 	hr = rDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
// 	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void Renderer::DisableLighting()
{
	IDirect3DDevice9*& rDevice = mpData->D3DDevice;
	HRESULT hr;

	PdrRendererData::StateLight& rState = mpData->LightState;
	if (!rState.IsValid)
	{
		hr = rDevice->SetRenderState(D3DRS_AMBIENT, rState.AMBIENT);
		WIRE_ASSERT(SUCCEEDED(hr));

		hr = rDevice->SetRenderState(D3DRS_LIGHTING, rState.LIGHTING);
		WIRE_ASSERT(SUCCEEDED(hr));

		rState.IsValid = true;
	}

	if (rState.LIGHTING != FALSE)
	{
		hr = rDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		WIRE_ASSERT(SUCCEEDED(hr));
		rState.LIGHTING = FALSE;
	}

// 	hr = rDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
// 	WIRE_ASSERT(SUCCEEDED(hr));
}
