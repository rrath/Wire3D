// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireQuaternion.h"

namespace Wire
{
template<> const Quaternion<Float> Quaternion<Float>::IDENTITY(
	1.0F, 0.0F, 0.0F, 0.0F);
template<> const Quaternion<Float> Quaternion<Float>::ZERO(
	0.0F, 0.0F, 0.0F, 0.0F);

template<> const Quaternion<Double> Quaternion<Double>::IDENTITY(
	1.0, 0.0, 0.0 ,0.0);
template<> const Quaternion<Double> Quaternion<Double>::ZERO(
	0.0, 0.0, 0.0, 0.0);
}
