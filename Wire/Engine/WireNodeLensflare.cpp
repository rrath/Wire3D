// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNodeLensflare.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NodeLensflare, Node);

//----------------------------------------------------------------------------
NodeLensflare::NodeLensflare(Texture2D* /*pFlare*/, TextureLayout /*textureLayout*/,
	TArray<FlareElement>& rFlareElements)
{
	mFlareElements = rFlareElements;
}

//----------------------------------------------------------------------------
NodeLensflare::~NodeLensflare()
{

}
