// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRECOLORRGBA_H
#define WIRECOLORRGBA_H

#include "WireTypes.h"
#include "WireColor32.h"

namespace Wire
{

class ColorRGBA
{
public:
	ColorRGBA();  // initial values (0, 0, 0, 0)
	ColorRGBA(Float red, Float green, Float blue, Float alpha);

 	inline operator Color32 ();
 	inline operator const Color32 () const;
	inline ColorRGBA& operator= (const Color32& color32);

	inline operator Float* ();
	inline operator const Float* () const;
	inline Float R() const;
	inline Float& R();
	inline Float G() const;
	inline Float& G();
	inline Float B() const;
	inline Float& B();
	inline Float A() const;
	inline Float& A();

	// arithmetic operations
	inline ColorRGBA operator* (Float scalar) const;
	inline friend ColorRGBA operator* (Float scalar, const ColorRGBA& rCol);

	// arithmetic updates
	inline ColorRGBA& operator*= (Float scalar);
	inline ColorRGBA& operator+= (const ColorRGBA& rCol);

	void Saturate();

	static const ColorRGBA BLACK;
	static const ColorRGBA WHITE;
	static const ColorRGBA RED;
	static const ColorRGBA GREEN;
	static const ColorRGBA BLUE;

private:
	Float mTuple[4];
};

#include "WireColorRGBA.inl"

}

#endif
