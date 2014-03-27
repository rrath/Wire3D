// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRENODELENSFLARE_H
#define WIRENODELENSFLARE_H

#include "WireNode.h"

#include "WireColorRGBA.h"

namespace Wire
{

class NodeLensflare : public Node
{
	WIRE_DECLARE_RTTI;

public:
	enum TextureLayout
	{
		TL_1_LARGE_4_SMALL = 0,
		TL_1_LARGE_2_MEDIUM_8_SMALL = 1,
		TL_1_TEXTURE = 2,
		TL_2X2_GRID = 3,
		TL_3X3_GRID = 4,
		TL_4X4_GRID = 5,
		TL_QUANTITY
	};

	struct FlareElement
	{
		FlareElement()
			:
			ImageIndex(0),
			Position(0.5F),
			Size(1.0F),
			Color(ColorRGBA::WHITE),
			UseLightColor(false),
			Rotate(false),
			Zoom(false),
			Fade(false)
			{}

		UChar ImageIndex;
		Float Position;
		Float Size;
		ColorRGBA Color;
		Bool UseLightColor;
		Bool Rotate;
		Bool Zoom;
		Bool Fade;
	};

	NodeLensflare(Texture2D* pFlare, TextureLayout textureLayout,
		TArray<FlareElement>& rFlareElements);
	virtual ~NodeLensflare();

private:
	TArray<FlareElement> mFlareElements;
};

typedef Pointer<NodeLensflare> NodeLensflarePtr;

}

#endif
