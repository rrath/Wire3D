// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireShader.h"

#include "WireRenderer.h"
#include "WireString.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Shader, Object);

//----------------------------------------------------------------------------
Shader::Shader(const Char* pProgram, const Char* pFunction,
	const Char* pProfile)
{
	WIRE_ASSERT(pProgram);
	WIRE_ASSERT(pFunction);
	WIRE_ASSERT(pProfile);
	mpProgram = WIRE_NEW String(pProgram);
	mpFunction = WIRE_NEW String(pFunction);
	mpProfile = WIRE_NEW String(pProfile);
}

//----------------------------------------------------------------------------
Shader::~Shader()
{
	WIRE_DELETE mpProgram;
	WIRE_DELETE mpFunction;
	WIRE_DELETE mpProfile;

	Renderer::UnbindAll(this);
}
