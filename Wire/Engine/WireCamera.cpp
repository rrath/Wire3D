// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireCamera.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, Camera, Object);

//----------------------------------------------------------------------------
Camera::Camera(Bool isPerspective)
	:
	mIsPerspective(isPerspective)
{
	Init();
	SetFrame(Vector3F::ZERO, -Vector3F::UNIT_Z, Vector3F::UNIT_Y,
		Vector3F::UNIT_X);
}

//----------------------------------------------------------------------------
Camera::Camera(const Vector3F& location, const Vector3F& direction,
	const Vector3F& up, Bool isPerspective)
	:
	mIsPerspective(isPerspective)
{
	Init();
	Vector3F right = direction.Cross(up);
	SetFrame(location, direction, up, right);
}

//----------------------------------------------------------------------------
Camera::~Camera()
{
}

//----------------------------------------------------------------------------
void Camera::Init()
{
	SetFrustum(-0.5F, 0.5F, -0.4F, 0.4F, 1.0F, 100.0F);
	SetViewport(0.0F, 1.0F, 1.0F, 0.0F);
	mLayerMask = static_cast<UInt64>(~0);
}

//----------------------------------------------------------------------------
void Camera::SetFrustum(Float rMin, Float rMax, Float uMin, Float uMax,
	Float dMin, Float dMax)
{
	mFrustum[VF_DMIN] = dMin;
	mFrustum[VF_DMAX] = dMax;
	mFrustum[VF_UMIN] = uMin;
	mFrustum[VF_UMAX] = uMax;
	mFrustum[VF_RMIN] = rMin;
	mFrustum[VF_RMAX] = rMax;
}

//----------------------------------------------------------------------------
void Camera::SetFrustum(Float upFovDegrees, Float aspectRatio, Float dMin,
	Float dMax)
{
	Float halfAngleRadians = 0.5F * upFovDegrees * MathF::DEG_TO_RAD;
	mFrustum[VF_UMAX] = dMin * MathF::Tan(halfAngleRadians);
	mFrustum[VF_RMAX] = aspectRatio * mFrustum[VF_UMAX];
	mFrustum[VF_UMIN] = -mFrustum[VF_UMAX];
	mFrustum[VF_RMIN] = -mFrustum[VF_RMAX];
	mFrustum[VF_DMIN] = dMin;
	mFrustum[VF_DMAX] = dMax;
}

//----------------------------------------------------------------------------
void Camera::GetFrustum(Float& rRMin, Float& rRMax, Float& rUMin,
	Float& rUMax, Float& rDMin, Float& rDMax) const
{
	rDMin = mFrustum[VF_DMIN];
	rDMax = mFrustum[VF_DMAX];
	rUMin = mFrustum[VF_UMIN];
	rUMax = mFrustum[VF_UMAX];
	rRMin = mFrustum[VF_RMIN];
	rRMax = mFrustum[VF_RMAX];
}

//----------------------------------------------------------------------------
Bool Camera::GetFrustum(Float& rUpFovDegrees, Float& rAspectRatio,
	Float& rDMin, Float& rDMax) const
{
	if (mFrustum[VF_RMIN] == -mFrustum[VF_RMAX]
	&&  mFrustum[VF_UMIN] == -mFrustum[VF_UMAX])
	{
		Float tmp = mFrustum[VF_UMAX] / mFrustum[VF_DMIN];
		rUpFovDegrees = 2.0F * MathF::ATan(tmp) * MathF::RAD_TO_DEG;
		rAspectRatio = mFrustum[VF_RMAX] / mFrustum[VF_UMAX];
		rDMin = mFrustum[VF_DMIN];
		rDMax = mFrustum[VF_DMAX];
		return true;
	}

	return false;
}

//----------------------------------------------------------------------------
void Camera::SetAspectRatio(Float aspectRation)
{
	Float fov;
	Float oldAspectRatio;
	Float near;
	Float far;

	GetFrustum(fov, oldAspectRatio, near, far);
	SetFrustum(fov, aspectRation, near, far);
}

//----------------------------------------------------------------------------
void Camera::SetAxes(const Vector3F& rDVector, const Vector3F& rUVector,
	const Vector3F& rRVector)
{
	mDVector = rDVector;
	mUVector = rUVector;
	mRVector = rRVector;

	Float aDet = MathF::FAbs(mDVector.Dot(mUVector.Cross(mRVector)));
	if (MathF::FAbs(1.0F - aDet) > 0.01F)
	{
		// The input vectors do not appear to form an orthonormal set. Time
		// to renormalize.
		Vector3F::Orthonormalize(mDVector, mUVector, mRVector);
	}
}

//----------------------------------------------------------------------------
void Camera::SetFrame(const Vector3F& rLocation, const Vector3F& rDVector,
	const Vector3F& rUVector, const Vector3F& rRVector)
{
	mLocation = rLocation;
	SetAxes(rDVector, rUVector, rRVector);
}

//----------------------------------------------------------------------------
void Camera::LookAt(const Vector3F& rLocation, const Vector3F& rLookAt,
	const Vector3F& rUp)
{
	const Vector3F direction = rLookAt - rLocation;
	SetFrame(rLocation, direction, rUp, direction.Cross(rUp));
}

//----------------------------------------------------------------------------
void Camera::SetViewport(Float left, Float right, Float top, Float bottom)
{
	mPortLeft = left;
	mPortRight = right;
	mPortTop = top;
	mPortBottom = bottom;
}

//----------------------------------------------------------------------------
void Camera::GetViewport(Float& rLeft, Float& rRight, Float& rTop,
	Float& rBottom)
{
	rLeft = mPortLeft;
	rRight = mPortRight;
	rTop = mPortTop;
	rBottom = mPortBottom;
}

//----------------------------------------------------------------------------
Matrix4F Camera::GetProjectionMatrix() const
{
	Float n = mFrustum[VF_DMIN];
	Float f = mFrustum[VF_DMAX];
	Float b = mFrustum[VF_UMIN];
	Float t = mFrustum[VF_UMAX];
	Float l = mFrustum[VF_RMIN];
	Float r = mFrustum[VF_RMAX];

	Float w = 1.0F/(r-l);
	Float h = 1.0F/(t-b);
	Float d = 1.0F/(f-n);

	if (mIsPerspective)
	{
		return Matrix4F(
			2*n*w,  0,      -(r+l)*w,  0,
			0,      2*n*h,  -(t+b)*h,  0,
			0,      0,       f*d,      -n*f*d,
			0,      0,       1,        0);
	}

	return Matrix4F(
		2*w,  0,    0,  -(r+l)*w,
		0,    2*h,  0,  -(t+b)*h,
		0,    0,    d,  -n*d,
		0,    0,    0,  1);
}

//----------------------------------------------------------------------------
Matrix4F Camera::GetViewMatrix() const
{
	const Vector3F& rEye = mLocation;
	return Matrix4F(
		mRVector.X(), mRVector.Y(), mRVector.Z(), -mRVector.Dot(rEye),
		mUVector.X(), mUVector.Y(), mUVector.Z(), -mUVector.Dot(rEye),
		mDVector.X(), mDVector.Y(), mDVector.Z(), -mDVector.Dot(rEye),
		0.0F, 0.0F, 0.0F, 1.0F);
}

//----------------------------------------------------------------------------
Matrix4F Camera::GetViewMatrixInverse() const
{
	return Matrix4F(
		mRVector.X(), mUVector.X(), mDVector.X(), mLocation.X(),
		mRVector.Y(), mUVector.Y(), mDVector.Y(), mLocation.Y(),
		mRVector.Z(), mUVector.Z(), mDVector.Z(), mLocation.Z(),
		0.0F, 0.0F, 0.0F, 1.0F);
}

//----------------------------------------------------------------------------
Matrix34F Camera::GetViewMatrix34() const
{
	const Vector3F& rEye = mLocation;
	return Matrix34F(
		mRVector.X(), mRVector.Y(), mRVector.Z(), -mRVector.Dot(rEye),
		mUVector.X(), mUVector.Y(), mUVector.Z(), -mUVector.Dot(rEye),
		mDVector.X(), mDVector.Y(), mDVector.Z(), -mDVector.Dot(rEye));
}

//----------------------------------------------------------------------------
Matrix34F Camera::GetViewMatrixInverse34() const
{
	return Matrix34F(
		mRVector.X(), mUVector.X(), mDVector.X(), mLocation.X(),
		mRVector.Y(), mUVector.Y(), mDVector.Y(), mLocation.Y(),
		mRVector.Z(), mUVector.Z(), mDVector.Z(), mLocation.Z());
}

//----------------------------------------------------------------------------
void Camera::GetPickRay(const Vector2F& rPosition, Vector3F& rRayOrigin,
	Vector3F& rRayDirection)
{
	WIRE_ASSERT(rPosition.X() >= -1.0F && rPosition.X() <= 1.0F);
	WIRE_ASSERT(rPosition.Y() >= -1.0F && rPosition.Y() <= 1.0F);
	Matrix4F projectionMatrix = GetProjectionMatrix();
	Vector3F v(rPosition.X() / projectionMatrix(0, 0),
		rPosition.Y() / projectionMatrix(1, 1), 1);

	rRayDirection = GetViewMatrixInverse34().Times3(v);
	rRayOrigin = GetViewMatrixInverse34().GetColumn(3);
}
