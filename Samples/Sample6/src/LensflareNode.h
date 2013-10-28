#pragma once
#ifndef LENSFLARENODE_H
#define LENSFLARENODE_H

#include "WireEngine.h"

namespace Wire
{

// To implement your own custom nodes for the scene graph and culling system,
// you need to derive your class from Wire::Node.
// See WireNodeSwitch, WireNodeBillboard or WireNodeLight for further examples
// of how Node derived classes work.
class LensflareNode : public Node
{
public:
	// Declare the run-time type information for this class.
	// This is optional, but recommended for every class derived from
	// Wire::Object. If we do not declare the RTTI, LensflareNode
	// objects will return the type of their parent (i.e. Wire::Node).
	WIRE_DECLARE_RTTI;

	LensflareNode(Spatial* pLightSource);

protected:
	// We handle culling ourselves, so we override GetVisibleSet()
	virtual void GetVisibleSet(Culler& rCuller, Bool noCull);

private:
	enum FlareType
	{
		FT_FLARE_1,
		FT_FLARE_2,
		FT_FLARE_3,
		FT_FLARE_4,
		FT_FLARE_5
	};

	struct FlareDef
	{
		FlareType Type;
		Float SizeFactor;
		Float PositionFactor;
		ColorRGBA Color;
	};

	void CreateFlares();
	Node* CreateFlare(const FlareDef& rDef);
	void CreateTextures();
	Float SmoothStep(Float a, Float b, Float x);
	void DrawParticle(Float* const pDst, Float fx, Float fy, UInt width);

	Texture2DPtr mspLensTex0;
	Texture2DPtr mspLensTex1;
	SpatialPtr mspLightSource;

	static const FlareDef s_DefaultDef[];
	static const Float s_Scale;
};

typedef Pointer<LensflareNode> LensflareNodePtr;

}

#endif
