#pragma once
#ifndef CONVEYORBELT_H
#define CONVEYORBELT_H

#include "WireController.h"
#include "WireRenderer.h"
#include "WireVertexBuffer.h"

class ConveyorBelt : public Wire::Controller
{
public:
	ConveyorBelt(Wire::RenderObject* pRenderObject, Wire::Renderer* pRenderer);

	virtual Bool Update(Double appTime);
	virtual Bool OnGetVisibleUpdate(const Wire::Camera* pCamera);

private:
	Wire::VertexBufferPtr mspVertexBufferCopy;
	Wire::Renderer* mpRenderer;

	Float mOffset;
};

#endif
