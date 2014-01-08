// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREGXSHADER_H
#define WIREGXSHADER_H

namespace Wire
{

class Renderer;
class Shader;

class PdrShader
{

// no shaders on the Wii, so there is nothing to do
public:
	PdrShader(Renderer*, const Shader*) {}
	~PdrShader() {}

	inline void Enable(Renderer*) {}
	inline void Disable(Renderer*) {}
	inline UInt GetBufferSize() { return 0; }
	inline void SetBuiltInVariables(Renderer*) {}

};

}

#endif
