// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREMATERIAL_H
#define WIREMATERIAL_H

#include "WireShader.h"
#include "WireShaderVariables.h"
#include "WireTArray.h"
#include "WireTexture2D.h"
#include "WireTInstanceID.h"

namespace Wire
{

class Material : public Object
{
	WIRE_DECLARE_RTTI;

public:
	Material(Shader* pPixelShader = NULL, Shader* pVertexShader = NULL,
		ShaderVariables* pPixelShaderVariables = NULL,
		ShaderVariables* pVertexShaderVariables = NULL);
	virtual ~Material();

	enum BlendMode
	{
		BM_REPLACE,		// C=Ct, Ct=C1, A=At, At=A1
		BM_MODULATE,	// C=Ct*Cf, Ct=C1, Cf=C2, A=At*Af, At=A1, Af=A2
		BM_PASS,		// C=Cf, A=Af
		BM_BLEND,		// C=Ct+(1-Ct)*Cf, Ct=C1, Cf=C3, A=At*Af, At=A1, Af=A2
		BM_DECAL,		// C=At*Ct+(1-At)*Cf, Ct=C1, Cf=C2, A=Af, Af=A1
		BM_ADD,			// C=Ct+Cf, Ct=C1, Cf=C2, A=At+Af, At=C1, Af=A2
		BM_QUANTITY
	};
	
	inline UInt GetTextureQuantity() const;
	inline Texture2D* GetTexture(UInt i = 0) const;
	inline BlendMode GetBlendMode(UInt i = 0) const;
	inline void SetBlendMode(BlendMode blendMode, UInt i = 0);
	inline UInt GetSamplerIndex(UInt i) const;
	inline void SetSamplerIndex(UInt i);

	inline Shader* GetPixelShader() const;
	inline void SetPixelShader(Shader* pShader);
	inline Shader* GetVertexShader() const;
	inline void SetVertexShader(Shader* pShader);
	inline ShaderVariables* GetPixelShaderVariables() const;
	inline void SetPixelShaderVariables(ShaderVariables* pPSVars);
	inline ShaderVariables* GetVertexShaderVariables() const;
	inline void SetVertexShaderVariables(ShaderVariables* pVSVars);
	inline Bool HasShaders() const;

	void AddTexture(Texture2D* pTexture, BlendMode blendMode = BM_MODULATE);
 	void SetTexture(UInt i, Texture2D* pTexture, BlendMode blendMode = 
		BM_MODULATE);

	TInstanceID<Material> ID;

private:
	struct TextureBlendMode
	{
		Texture2DPtr Texture;
		Material::BlendMode BlendMode;
	};

	TArray<TextureBlendMode> mTextures;

	ShaderPtr mspPixelShader;
	ShaderPtr mspVertexShader;
	ShaderVariablesPtr mspPixelShaderVariables;
	ShaderVariablesPtr mspVertexShaderVariables;
};

typedef Pointer<Material> MaterialPtr;
#include "WireMaterial.inl"

}

#endif
