#pragma once
#ifndef SAMPLE7_H
#define SAMPLE7_H

#include "WireApplication.h"

namespace Wire
{

class Sample7 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	virtual Bool OnInitialize();
	virtual void OnIdle();

private:
	RenderObject* CreateTorus();
	void GeneratePositions(VertexBuffer* pVBuffer, Float radiusAngle);
	void GenerateNormals(VertexBuffer* pVBuffer, IndexBuffer* pIBuffer);

	CameraPtr mspCamera;
	RenderObjectPtr mspTorus;
	MeshPtr mspMeshA;
	MeshPtr mspMeshB;

	Float mAngle;
	Double mLastTime;

	static const UInt s_ShapeCount = 24;
	static const UInt s_SegmentCount = 192;
	static const UInt s_P = 3;
	static const UInt s_Q = 2;
	TArray<TArray<UInt> > mBuckets;

	RenderTextPtr mspText;

	Double mSegmentCount;
	Bool mIsExpanding;
};

WIRE_REGISTER_INITIALIZE(Sample7);

}

#endif
