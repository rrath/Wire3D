#include "SplineCamera.h"

using namespace Wire;

//----------------------------------------------------------------------------
SplineCamera::SplineCamera(Node* pSplineRoot, Camera* pCamera)
{
	if (!pSplineRoot)
	{
		return;
	}

	mspCamera = pCamera;

	for (UInt i = 0; i < pSplineRoot->GetQuantity(); i++)
	{
		Spatial* pChild = pSplineRoot->GetChild(i);
		if (pChild)
		{
			mSplinePoints.Append(&(pChild->World));
		}
	}

	Transformation* pTrafo = mSplinePoints[0];
	mSplinePoints.Append(mSplinePoints[0]);
	mSplinePoints.Append(mSplinePoints[1]);
	mSplinePoints.Insert(0, pTrafo);

	mT = 0;
	mSplinePointIndex = 1;
}

//----------------------------------------------------------------------------
Bool SplineCamera::Update(Double appTime)
{
	if (!mspCamera && mSplinePoints.GetQuantity() == 0)
	{
		return false;
	}

	Double elapsedTime = appTime - mLastApplicationTime;
	if (mLastApplicationTime == -MathD::MAX_REAL)
	{
		elapsedTime = 0;
	}

	mLastApplicationTime = appTime;

	Vector3F camPos = GetHermite(mSplinePoints, mSplinePointIndex, mT);
	QuaternionF camRot = GetSquad(mSplinePoints, mSplinePointIndex, mT);
	Matrix3F camMat;
	camRot.ToRotationMatrix(camMat);
	Matrix3F mat90(Vector3F::UNIT_Y, -MathF::HALF_PI);
	camMat = camMat * mat90;
	mspCamera->SetFrame(camPos, camMat.GetColumn(0), camMat.GetColumn(1),
		camMat.GetColumn(2));

	mT += static_cast<Float>(elapsedTime*0.25F);
	while (mT >= 1.0F)
	{
		mT -= 1.0F;
		mSplinePointIndex++;
		if (mSplinePointIndex == mSplinePoints.GetQuantity()-2)
		{
			mSplinePointIndex = 1;
		}
	}

	return true;
}

//----------------------------------------------------------------------------
Vector3F SplineCamera::GetHermite(TArray<Transformation*>& rControlPoints,
	UInt idx, Float t)
{
	WIRE_ASSERT(idx > 0 && idx < rControlPoints.GetQuantity()-2);
	Float t2 = t * t;
	Float t3 = t2 * t;

	Vector3F p0 = (*(rControlPoints[idx - 1])).GetTranslate();
	Vector3F p1 = (*(rControlPoints[idx])).GetTranslate();
	Vector3F p2 = (*(rControlPoints[idx + 1])).GetTranslate();
	Vector3F p3 = (*(rControlPoints[idx + 2])).GetTranslate();

	const Float tension = 0.5f;	// 0.5 is catmull-rom

	Vector3F v4 = tension * (p2 - p0);
	Vector3F v5 = tension * (p3 - p1);

	Float blend1 = 2 * t3 - 3 * t2 + 1;
	Float blend2 = -2 * t3 + 3 * t2;
	Float blend3 = t3 - 2 * t2 + t;
	Float blend4 = t3 - t2;

	return blend1 * p1 + blend2 * p2 + blend3 * v4 + blend4 * v5;
}

//----------------------------------------------------------------------------
QuaternionF SplineCamera::GetSquad(TArray<Transformation*>& rControlPoints,
	UInt idx, Float t)
{
	QuaternionF q0((*(rControlPoints[idx - 1])).GetRotate());
	QuaternionF q1((*(rControlPoints[idx])).GetRotate());
	QuaternionF q2((*(rControlPoints[idx + 1])).GetRotate());
	QuaternionF q3((*(rControlPoints[idx + 2])).GetRotate());

	QuaternionF t1 = q0.Intermediate(q0, q1, q2);
	QuaternionF t2 = q3.Intermediate(q1, q2, q3);

	return q0.Squad(t, q1, t1, t2, q2);
}
