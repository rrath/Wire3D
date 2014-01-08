// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireMatrix3.h"

namespace Wire
{
template<> const Matrix3<Float> Matrix3<Float>::ZERO(
	0.0F, 0.0F, 0.0F,
	0.0F, 0.0F, 0.0F,
	0.0F, 0.0F, 0.0F);
template<> const Matrix3<Float> Matrix3<Float>::IDENTITY(
	1.0F, 0.0F, 0.0F,
	0.0F, 1.0F, 0.0F,
	0.0F, 0.0F, 1.0F);

template<> const Matrix3<Double> Matrix3<Double>::ZERO(
	0.0, 0.0, 0.0,
	0.0, 0.0, 0.0,
	0.0, 0.0, 0.0);
template<> const Matrix3<Double> Matrix3<Double>::IDENTITY(
	1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0);
}
