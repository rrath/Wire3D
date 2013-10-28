#pragma once
#ifndef CURSORS_H
#define CURSORS_H

#include "WireMaterial.h"
#include "WireNode.h"

class Cursors
{
public:
	enum CursorMode
	{
		CM_POINTING,
		CM_PRIMARY_BUTTON_PRESSED,
		CM_SECONDARY_BUTTON_PRESSED,
		CM_OFF
	};

	Cursors();

	void SetCursor(Float x, Float y, CursorMode mode = CM_POINTING,
		UInt playerNo = 0, Float zRollInRadian = 0);
	Wire::Node* GetRoot();

private:
	void InitCursors();
	Wire::Node* CreateCursor(Float uOffset, Float vOffset);

	Wire::NodePtr mspCursors;
	Wire::MaterialPtr mspMaterial;

	static const UChar s_Png[];
	static const Int s_PngSize;
};

#endif
