// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline const Char* Shader::GetProgram() const
{
	return static_cast<const Char*>(*mpProgram);
}

//----------------------------------------------------------------------------
inline const Char* Shader::GetFunction() const
{
	return static_cast<const Char*>(*mpFunction);
}

//----------------------------------------------------------------------------
inline const Char* Shader::GetProfile() const
{
	return static_cast<const Char*>(*mpProfile);
}

//----------------------------------------------------------------------------
inline UInt Shader::GetProgramLength() const
{
	return mpProgram->GetLength();
}

//----------------------------------------------------------------------------
inline UInt Shader::GetSamplerIndex(UInt at)
{
	return mSamplerIndices.GetQuantity() <= at ? at : mSamplerIndices[at];
}

//----------------------------------------------------------------------------
inline void Shader::SetSamplerIndex(UInt samplerIndex, UInt at)
{
	mSamplerIndices.SetElement(at, samplerIndex);
}
