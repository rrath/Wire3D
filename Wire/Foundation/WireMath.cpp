// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireMath.h"

namespace Wire
{
template<> const Float Math<Float>::PI = static_cast<Float>(4.0*atan(1.0));
template<> const Float Math<Float>::TWO_PI = 2.0F * Math<Float>::PI;
template<> const Float Math<Float>::HALF_PI = 0.5F * Math<Float>::PI;
template<> const Float Math<Float>::MAX_REAL =  FLT_MAX;
template<> const Float Math<Float>::DEG_TO_RAD = Math<Float>::PI/180.0F;
template<> const Float Math<Float>::RAD_TO_DEG = 180.0F/Math<Float>::PI;
template<> const Float Math<Float>::ZERO_TOLERANCE = 1e-06F;

template<> const Double Math<Double>::PI = 4.0*atan(1.0);
template<> const Double Math<Double>::TWO_PI = 2.0 * Math<Double>::PI;
template<> const Double Math<Double>::HALF_PI = 0.5 * Math<Double>::PI;
template<> const Double Math<Double>::MAX_REAL =  DBL_MAX;
template<> const Double Math<Double>::DEG_TO_RAD = Math<Double>::PI/180.0;
template<> const Double Math<Double>::RAD_TO_DEG = 180.0/Math<Double>::PI;
template<> const Double Math<Double>::ZERO_TOLERANCE = 1e-08;
}
