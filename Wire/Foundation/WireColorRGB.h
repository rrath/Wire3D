// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRECOLORRGB_H
#define WIRECOLORRGB_H

#include "WireTypes.h"
#include "WireColor32.h"

namespace Wire
{

class ColorRGB
{
public:
	ColorRGB();  // initial values (0, 0, 0)
	ColorRGB(Float red, Float green, Float blue);

	inline operator Color32 ();
	inline operator const Color32 () const;
	inline ColorRGB& operator= (const Color32& color32);

	inline operator Float* ();
	inline operator const Float* () const;
	inline Float R() const;
	inline Float& R();
	inline Float G() const;
	inline Float& G();
	inline Float B() const;
	inline Float& B();

	// arithmetic operations
	inline ColorRGB operator* (Float scalar) const;
	inline friend ColorRGB operator* (Float scalar, const ColorRGB& rCol);

	// arithmetic updates
	inline ColorRGB& operator*= (Float scalar);
	inline ColorRGB& operator+= (const ColorRGB& rCol);

	void Saturate();

	static const ColorRGB BLACK;
	static const ColorRGB WHITE;
	static const ColorRGB RED;
	static const ColorRGB GREEN;
	static const ColorRGB BLUE;

private:
	Float mTuple[3];
};

#include "WireColorRGB.inl"

}

#endif
