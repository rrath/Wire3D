#include "ConveyorBelt.h"

#include "WireNode.h"

using namespace Wire;

//----------------------------------------------------------------------------
ConveyorBelt::ConveyorBelt(RenderObject* pRenderObject, Renderer* pRenderer)
	:
	mpRenderer(pRenderer),
	mOffset(0)
{
	if (!pRenderObject || !pRenderObject->GetMesh())
	{
		return;
	}

	Mesh* pMesh = pRenderObject->GetMesh();
	for (UInt i = 0; i < pMesh->GetVertexBuffers().GetQuantity(); i++)
	{
		VertexBuffer* pVertexBuffer = pMesh->GetVertexBuffer(i);
		if (pVertexBuffer->GetAttributes().HasTCoord())
		{
			mspVertexBufferCopy = WIRE_NEW VertexBuffer(pVertexBuffer);
			break;
		}
	}

	WIRE_ASSERT(mspVertexBufferCopy);
}

//----------------------------------------------------------------------------
Bool ConveyorBelt::Update(Double appTime)
{
	Double elapsedTime = appTime - mLastApplicationTime;
	if (mLastApplicationTime == -MathD::MAX_REAL)
	{
		elapsedTime = 0;
	}

	mLastApplicationTime = appTime;

	mOffset -= static_cast<Float>(elapsedTime);
	if (mOffset < -1.0F)
	{
		mOffset += 1.0F;
	}

	return true;
}

//----------------------------------------------------------------------------
Bool ConveyorBelt::OnGetVisibleUpdate(const Camera*)
{
	Node* pGeo = DynamicCast<Node>(mpSceneObject);
	if (!mspVertexBufferCopy || !pGeo)
	{
		return false;
	}

	VertexBuffer* pVertexBuffer = NULL;
	Mesh* pMesh = pGeo->GetRenderObject()->GetMesh();
	for (UInt i = 0; i < pMesh->GetVertexBuffers().GetQuantity(); i++)
	{
		if (pMesh->GetVertexBuffer(i)->GetAttributes().HasTCoord())
		{
			pVertexBuffer = pMesh->GetVertexBuffer(i);
			break;
		}
	}

	WIRE_ASSERT(pVertexBuffer);
	if (!pVertexBuffer)
	{
		return false;
	}

	for (UInt i = 0; i < mspVertexBufferCopy->GetQuantity(); i++)
	{
		Vector2F vec2 = mspVertexBufferCopy->TCoord2(i);
		vec2.Y() += mOffset;
		pVertexBuffer->TCoord2(i) = vec2;
	}

	mpRenderer->Update(pVertexBuffer);

	return true;
}
