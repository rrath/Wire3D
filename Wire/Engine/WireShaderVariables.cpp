// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireShaderVariables.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, ShaderVariables, Object);

//----------------------------------------------------------------------------
ShaderVariables::ShaderVariables()
{
}

//----------------------------------------------------------------------------
ShaderVariables::~ShaderVariables()
{
}

//----------------------------------------------------------------------------
void ShaderVariables::SetMatrix(const Char* pName, const Matrix4F& rMatrix)
{
	String name(pName);
	UInt i = mMatrixNames.Find(name);
	if (i < mMatrixNames.GetQuantity())
	{
		mMatrices[i] = rMatrix;
	}
	else
	{
		mMatrixNames.Append(name);
		mMatrices.Append(rMatrix);
	}
}

//----------------------------------------------------------------------------
void ShaderVariables::SetFloat(const Char* pName, const Float value)
{
	String name(pName);
	UInt i = mFloatNames.Find(name);
	if (i < mFloatNames.GetQuantity())
	{
		mFloats[i] = value;
	}
	else
	{
		mFloatNames.Append(name);
		mFloats.Append(value);
	}
}

//----------------------------------------------------------------------------
void ShaderVariables::SetFloat4(const Char* pName, const Vector4F& rFloat4)
{
	String name(pName);
	UInt i = mFloat4Names.Find(name);
	if (i < mFloat4Names.GetQuantity())
	{
		mFloat4s[i] = rFloat4;
	}
	else
	{
		mFloat4Names.Append(name);
		mFloat4s.Append(rFloat4);
	}
}
