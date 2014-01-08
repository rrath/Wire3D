// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRECOLOR32_H
#define WIRECOLOR32_H

#include "WireSystem.h"

namespace Wire
{
 
class Color32
{
public:
	Color32();  // initial values (0, 0, 0, 0)
	Color32(UChar red, UChar green, UChar blue, UChar alpha = 0xff);

	inline UChar R() const;
	inline UChar& R();
	inline UChar G() const;
	inline UChar& G();
	inline UChar B() const;
	inline UChar& B();
	inline UChar A() const;
	inline UChar& A();

	static const Color32 BLACK;
	static const Color32 WHITE;
	static const Color32 RED;
	static const Color32 GREEN;
	static const Color32 BLUE;

private:
	union tTuple
	{
		UInt m32;
		UChar m8[4];
	} mTuple;
};

#include "WireColor32.inl"

}

#endif
