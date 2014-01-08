// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRESHADER_H
#define WIRESHADER_H

#include "WireObject.h"
#include "WireString.h"
#include "WireTArray.h"

namespace Wire
{

class Shader : public Object
{
	WIRE_DECLARE_RTTI;

public:
	Shader(const Char* pProgram, const Char* pFunction,	const Char* pProfile);
	virtual ~Shader();

	inline const Char* GetProgram() const;
	inline const Char* GetFunction() const;
	inline const Char* GetProfile() const;
	inline UInt GetProgramLength() const;

	inline UInt GetSamplerIndex(UInt at);
	inline void SetSamplerIndex(UInt samplerIndex, UInt at);

private:
	String* mpProgram;
	String* mpFunction;
	String* mpProfile;

	TArray<UInt> mSamplerIndices;
};

typedef Pointer<Shader> ShaderPtr;
#include "WireShader.inl"

}

#endif
