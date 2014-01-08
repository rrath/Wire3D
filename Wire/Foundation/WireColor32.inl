// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UChar Color32::R() const
{
	return mTuple.m8[WIRE_RGBA_R];
}

//----------------------------------------------------------------------------
inline UChar& Color32::R()
{
	return mTuple.m8[WIRE_RGBA_R];
}

//----------------------------------------------------------------------------
inline UChar Color32::G() const
{
	return mTuple.m8[WIRE_RGBA_G];
}

//----------------------------------------------------------------------------
inline UChar& Color32::G()
{
	return mTuple.m8[WIRE_RGBA_G];
}

//----------------------------------------------------------------------------
inline UChar Color32::B() const
{
	return mTuple.m8[WIRE_RGBA_B];
}

//----------------------------------------------------------------------------
inline UChar& Color32::B()
{
	return mTuple.m8[WIRE_RGBA_B];
}

//----------------------------------------------------------------------------
inline UChar Color32::A() const
{
	return mTuple.m8[WIRE_RGBA_A];
}

//----------------------------------------------------------------------------
inline UChar& Color32::A()
{
	return mTuple.m8[WIRE_RGBA_A];
}
