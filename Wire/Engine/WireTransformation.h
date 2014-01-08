// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRETRANSFORMATION_H
#define WIRETRANSFORMATION_H

#include "WireMatrix3.h"
#include "WireMatrix34.h"
#include "WireMatrix4.h"
#include "WireTypes.h"

namespace Wire
{

class Transformation
{
public:
	Transformation();
 	~Transformation();

	// Hints about the structure of the transformation.  In the common case
	// of M = R*S, IsRSMatrix() returns true.
	inline Bool IsIdentity() const;
	inline Bool IsRSMatrix() const;
	inline Bool IsUniformScale() const;

	inline operator Matrix34F ();
 	inline operator const Matrix34F () const;

	void MakeIdentity();

	void SetRotate(const Matrix3F& rMatrix);
	Matrix3F GetRotate() const;
	void SetRotate(const Matrix34F& rMatrix);
	void SetMatrix(const Matrix34F& rMatrix, Bool containsScale);
	inline const Matrix34F& GetMatrix() const;
	void SetTranslate(const Vector3F& rTranslate);
	inline Vector3F GetTranslate() const;
	void SetScale(const Vector3F& rScale);
	inline const Vector3F& GetScale() const;
	void SetUniformScale(Float scale);
	inline Float GetUniformScale() const;

	// Pack the transformation into a 3-by-4 matrix with implicit 4th row.
	void GetTransformation(Matrix34F& rMatrix) const;

	// Pack the transformation into a 4-by-4 matrix, stored so that it may be
	// applied to 1-by-4 vectors on the left.
	void GetHomogeneous(Matrix4F& rHMatrix) const;

	// For M = R*S, the largest value of S in absolute value is returned.
	// For general M, the max-row-sum norm is returned (and is guaranteed to
	// be larger or equal to the largest eigenvalue of S in absolute value).
	Float GetNorm() const;

	// Compute Y = M*X+T where X is the input point and Y is the output point.
	Vector3F ApplyForward(const Vector3F& rInput) const;

	// Compute C = A*B.
	void Product(const Transformation& rA, const Transformation& kB);

    // Compute X = M^{-1}*(Y-T) where Y is the input point and X is the output
    // point.
    Vector3F ApplyInverse(const Vector3F& rInput) const;

	static const Transformation IDENTITY;

private:
	Matrix34F mMatrix;
	Vector3F mScale;

	Bool mIsIdentity;
	Bool mIsRSMatrix;
	Bool mIsUniformScale;
};

#include "WireTransformation.inl"

}

#endif
