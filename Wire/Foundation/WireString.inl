// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt String::GetLength() const
{
    return mLength;
}

//----------------------------------------------------------------------------
inline String::operator const Char* () const
{
    return mpText;
}

//----------------------------------------------------------------------------
inline char& String::operator[] (UInt index)
{
	WIRE_ASSERT(index <= mLength);
	return mpText[index];
}
//----------------------------------------------------------------------------
inline const char& String::operator[] (UInt index) const
{
	WIRE_ASSERT(index <= mLength);
	return mpText[index];
}
