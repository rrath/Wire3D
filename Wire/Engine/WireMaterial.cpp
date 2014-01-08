// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireMaterial.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Material, Object);

//----------------------------------------------------------------------------
Material::Material(Shader* pPixelShader, Shader* pVertexShader,
	ShaderVariables* pPixelVars, ShaderVariables* pVertexVars)
	:
	mspPixelShader(pPixelShader),
	mspVertexShader(pVertexShader),
	mspPixelShaderVariables(pPixelVars),
	mspVertexShaderVariables(pVertexVars)
{
}

//----------------------------------------------------------------------------
Material::~Material()
{
}

//----------------------------------------------------------------------------
void Material::AddTexture(Texture2D* pTexture, BlendMode blendMode)
{
	TextureBlendMode item;
	item.Texture = pTexture;
	item.BlendMode = blendMode;
	mTextures.Append(item);
}

//----------------------------------------------------------------------------
void Material::SetTexture(UInt i, Texture2D* pTexture, BlendMode blendMode)
{
	TextureBlendMode item;
	item.Texture = pTexture;
	item.BlendMode = blendMode;
	mTextures.SetElement(i, item);
}
