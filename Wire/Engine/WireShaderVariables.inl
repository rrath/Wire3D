// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline const TArray<Matrix4F>& ShaderVariables::GetMatrices() const
{
	return mMatrices;
}

//----------------------------------------------------------------------------
inline const TArray<String>& ShaderVariables::GetMatrixNames() const
{
	return mMatrixNames;
}

//----------------------------------------------------------------------------
inline const TArray<Float>& ShaderVariables::GetFloats() const
{
	return mFloats;
}

//----------------------------------------------------------------------------
inline const TArray<String>& ShaderVariables::GetFloatNames() const
{
	return mFloatNames;
}

//----------------------------------------------------------------------------
inline const TArray<Vector4F>& ShaderVariables::GetFloat4s() const
{
	return mFloat4s;
}

//----------------------------------------------------------------------------
inline const TArray<String>& ShaderVariables::GetFloat4Names() const
{
	return mFloat4Names;
}
