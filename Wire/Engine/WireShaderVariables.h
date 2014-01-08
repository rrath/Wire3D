// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRESHADERVARIABLES_H
#define WIRESHADERVARIABLES_H

#include "WireMatrix4.h"
#include "WireObject.h"
#include "WireString.h"
#include "WireTArray.h"

namespace Wire
{

class ShaderVariables : public Object
{
	WIRE_DECLARE_RTTI;

public:
	ShaderVariables();
	virtual ~ShaderVariables();

	// TODO: use hashes/handles instead of strings
	void SetMatrix(const Char* pName, const Matrix4F& rMatrix);
	void SetFloat4(const Char* pName, const Vector4F& rFloat4);
	void SetFloat(const Char* pName, const Float value);

	inline const TArray<Matrix4F>& GetMatrices() const;
	inline const TArray<String>& GetMatrixNames() const;
	inline const TArray<Float>& GetFloats() const;
	inline const TArray<String>& GetFloatNames() const;
	inline const TArray<Vector4F>& GetFloat4s() const;
	inline const TArray<String>& GetFloat4Names() const;

private:
	TArray<Matrix4F> mMatrices;
	TArray<String> mMatrixNames;

	TArray<Float> mFloats;
	TArray<String> mFloatNames;

	TArray<Vector4F> mFloat4s;
	TArray<String> mFloat4Names;
};

typedef Pointer<ShaderVariables> ShaderVariablesPtr;
#include "WireShaderVariables.inl"

}

#endif
