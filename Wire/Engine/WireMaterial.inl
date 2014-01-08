// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt Material::GetTextureQuantity() const
{
	return mTextures.GetQuantity();
}

//----------------------------------------------------------------------------
inline Texture2D* Material::GetTexture(UInt i) const
{
	return mTextures[i].Texture;
}

//----------------------------------------------------------------------------
inline Material::BlendMode Material::GetBlendMode(UInt i) const
{
	return mTextures[i].BlendMode;
}

//----------------------------------------------------------------------------
inline void Material::SetBlendMode(BlendMode blendmode, UInt i)
{
	mTextures[i].BlendMode = blendmode;
}

//----------------------------------------------------------------------------
inline UInt Material::GetSamplerIndex(UInt i) const
{
	return mspPixelShader ? mspPixelShader->GetSamplerIndex(i) : i;
}

//----------------------------------------------------------------------------
inline Shader* Material::GetPixelShader() const
{
	return mspPixelShader;
}

//----------------------------------------------------------------------------
inline void Material::SetPixelShader(Shader* pShader)
{
	mspPixelShader = pShader;
}

//----------------------------------------------------------------------------
inline Shader* Material::GetVertexShader() const
{
	return mspVertexShader;
}

//----------------------------------------------------------------------------
inline void Material::SetVertexShader(Shader* pShader)
{
	mspVertexShader = pShader;
}

//----------------------------------------------------------------------------
inline ShaderVariables* Material::GetPixelShaderVariables() const
{
	return mspPixelShaderVariables;
}

//----------------------------------------------------------------------------
inline void Material::SetPixelShaderVariables(ShaderVariables* pPSVars)
{
	mspPixelShaderVariables = pPSVars;
}

//----------------------------------------------------------------------------
inline ShaderVariables* Material::GetVertexShaderVariables() const
{
	return mspVertexShaderVariables;
}

//----------------------------------------------------------------------------
inline void Material::SetVertexShaderVariables(ShaderVariables* pVSVars)
{
	mspVertexShaderVariables = pVSVars;
}

//----------------------------------------------------------------------------
inline Bool Material::HasShaders() const
{
	return mspPixelShader || mspVertexShader;
}
