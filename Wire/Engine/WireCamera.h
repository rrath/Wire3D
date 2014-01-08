// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRECAMERA_H
#define WIRECAMERA_H

#include "WireMatrix34.h"
#include "WireMatrix4.h"
#include "WireVector2.h"
#include "WireObject.h"

namespace Wire
{

class Camera : public Object
{
	WIRE_DECLARE_RTTI;

public:
	Camera(Bool isPerspective = true);
	Camera(const Vector3F& location, const Vector3F& direction,
		const Vector3F& up, Bool isPerspective = true);
	virtual ~Camera();

	enum
	{
		VF_DMIN     = 0,  // near
		VF_DMAX     = 1,  // far
		VF_UMIN     = 2,  // bottom
		VF_UMAX     = 3,  // top
		VF_RMIN     = 4,  // left
		VF_RMAX     = 5,  // right
		VF_QUANTITY = 6
	};

	inline Vector3F GetLocation() const;
	inline Vector3F GetDVector() const;
	inline Vector3F GetUVector() const;
	inline Vector3F GetRVector() const;

	// The camera frame is always in world coordinates.
	//   default location  E = (0, 0, 0)
	//   default direction D = (0, 0,-1)
	//   default up        U = (0, 1, 0)
	//   default right     R = (1, 0, 0)
	void SetLocation(const Vector3F& rLocation);

	void SetFrame(const Vector3F& rLocation, const Vector3F& rDVector,
		const Vector3F& rUVector, const Vector3F& rRVector);

	void LookAt(const Vector3F& rLocation, const Vector3F& rLookAt,
		const Vector3F& rUp);

	void SetAxes(const Vector3F& rDVector, const Vector3F& rUVector,
		const Vector3F& rRVector);

	void SetAspectRatio(Float aspectRatio);

	// Set the view frustum. The interval [rmin, rmax] is measured in the
	// right direction R. These are the "left" and "right" frustum values.
	// The interval [umin, umax] is measured in the up direction U.  These
	// are the "bottom" and "top" values. The interval [dmin, dmax] is
	// measured in the view direction D. These are the "near" and "far"
	// values.
	void SetFrustum(Float rMin, Float rMax, Float uMin, Float uMax,
		Float dMin, Float dMax);

	// Set a symmetric view frustum (umin = -umax, rmin = -rmax) 
	// using a field of view in the "up" direction and an aspect ratio
	// "width/height". This call is the equivalent of gluPerspective in
	// OpenGL and MTXPerspective in GX. As such, the field of view in 
	// this function must be specified in degrees and be in the
	// interval (0, 180).
	void SetFrustum(Float upFovDegrees, Float aspectRatio, Float dMin,
		Float dMax);

	void GetFrustum(Float& rRMin, Float& rRMax, Float& rUMin, Float& rUMax,
		Float& rDMin, Float& rDMax) const;

	// Get the parameters for a symmetric view frustum. The return value is
	// 'true' if the current frustum is symmetric, in which case the output
	// parameters are valid.
	Bool GetFrustum(Float& rUpFovDegrees, Float& rAspectRatio,
		Float& rDMin, Float& rDMax) const;

	// Get all the view frustum values simultaneously.
	inline const Float* GetFrustum() const;

	// Get the individual frustum values.
	inline Float GetDMin() const;
	inline Float GetDMax() const;
	inline Float GetUMin() const;
	inline Float GetUMax() const;
	inline Float GetRMin() const;
	inline Float GetRMax() const;

	// viewport (contained in [0,1]^2)
	void SetViewport(Float left, Float right, Float top, Float bottom);
	void GetViewport(Float& rLeft, Float& rRight, Float& rTop,
		Float& rBottom);

	inline Bool IsPerspective() const;

	// The frustum values are N (near), F (far), B (bottom), T (top),
	// L (left), and R (right).
	// perspective, depth [0,1]
	//   +-                                               -+
	//   | 2*N/(R-L)  0           -(R+L)/(R-L)  0          |
	//   | 0          2*N/(T-B)   -(T+B)/(T-B)  0          |
	//   | 0          0           F/(F-N)       -N*F/(F-N) |
	//   | 0          0           1             0          |
	//   +-                                               -+
	//
	// orthographic, depth [0,1]
	//   +-                                       -+
	//   | 2/(R-L)  0  0              -(R+L)/(R-L) |
	//   | 0        2/(T-B)  0        -(T+B)/(T-B) |
	//   | 0        0        1/(F-N)  -N/(F-N)  0  |
	//   | 0        0        0        1            |
	//   +-                                       -+
	// projects left-bottom (0,0) to (-1,-1) and right-top to (1,1)
	//
	// The projection matrix multiplies vectors on its right, matrix*vector4.
	Matrix4F GetProjectionMatrix() const;

	// Access the view matrix of the camera.
	// If Location is P, D = (d0,d1,d2), U = (u0,u1,u2) and R = (r0,r1,r2),
	// then the view matrix is
	//   +-                     -+
	//   | r0  r1  r2  -Dot(R,P) |
	//   | u0  u1  u2  -Dot(U,P) |
	//   | d0  d1  d2  -Dot(D,P) |
	//   |  0   0   0          1 |
	//   +-                     -+
	// The view matrix multiplies vectors on its right, matrix*vector.

	// 4x4 view matrix
	Matrix4F GetViewMatrix() const;
	Matrix4F GetViewMatrixInverse() const; // faster than matrix.Inverse()

	// 3x4 view matrix with implicit 4th row, can be multiplied with Vector3F
	Matrix34F GetViewMatrix34() const;
	Matrix34F GetViewMatrixInverse34() const; // faster than matrix.Inverse()

	// Position [-1,1], returns world coordinates of the pick ray
	void GetPickRay(const Vector2F& rPosition, Vector3F& rRayOrigin,
		Vector3F& rRayDirection);

	inline void SetLayerMask(UInt64 layerMask);
	inline UInt64 GetLayerMask() const;

private:
	void Init();

	// world coordinate frame
	Vector3F mLocation;
	Vector3F mDVector;
	Vector3F mUVector;
	Vector3F mRVector;

	// view frustum (near, far, bottom, top, left, right)
	Float mFrustum[VF_QUANTITY];

	// viewport
	Float mPortLeft;
	Float mPortRight;
	Float mPortTop;
	Float mPortBottom;

	// default behavior is to cull all layers
	UInt64 mLayerMask;	// default: 0xFFFFFFFFFFFFFFFF

	// perspective or orthographic
	Bool mIsPerspective;
};

typedef Pointer<Camera> CameraPtr;

#include "WireCamera.inl"

}

#endif
