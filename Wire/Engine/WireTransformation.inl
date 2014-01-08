// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Bool Transformation::IsIdentity() const
{
	return mIsIdentity;
}

//----------------------------------------------------------------------------
inline Bool Transformation::IsRSMatrix() const
{
	return mIsRSMatrix;
}

//----------------------------------------------------------------------------
inline Bool Transformation::IsUniformScale() const
{
	return mIsRSMatrix && mIsUniformScale;
}

//----------------------------------------------------------------------------
inline Transformation::operator Matrix34F ()
{
	Matrix34F transformation;
	GetTransformation(transformation);
	return transformation;
}

//----------------------------------------------------------------------------
inline Transformation::operator const Matrix34F () const
{
	Matrix34F transformation;
	GetTransformation(transformation);
	return transformation;
}

//----------------------------------------------------------------------------
inline const Matrix34F& Transformation::GetMatrix() const
{
	return mMatrix;
}

//----------------------------------------------------------------------------
inline Vector3F Transformation::GetTranslate() const
{
	return mMatrix.GetColumn(3);
}

//----------------------------------------------------------------------------
inline const Vector3F& Transformation::GetScale() const
{
	WIRE_ASSERT(mIsRSMatrix);
	return mScale;
}

//----------------------------------------------------------------------------
inline Float Transformation::GetUniformScale() const
{
	WIRE_ASSERT(mIsRSMatrix && mIsUniformScale);
	return mScale.X();
}
