// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireVector2.h"

namespace Wire
{
template<> const Vector2<Float> Vector2<Float>::ZERO(0.0F, 0.0F);
template<> const Vector2<Float> Vector2<Float>::UNIT_X(1.0F, 0.0F);
template<> const Vector2<Float> Vector2<Float>::UNIT_Y(0.0F, 1.0F);
template<> const Vector2<Float> Vector2<Float>::ONE(1.0F, 1.0F);

template<> const Vector2<Double> Vector2<Double>::ZERO(0.0, 0.0);
template<> const Vector2<Double> Vector2<Double>::UNIT_X(1.0, 0.0);
template<> const Vector2<Double> Vector2<Double>::UNIT_Y(0.0, 1.0);
template<> const Vector2<Double> Vector2<Double>::ONE(1.0, 1.0);
}
