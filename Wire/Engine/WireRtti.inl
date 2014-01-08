// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline const Char* Rtti::GetName() const
{
	return mpName;
}

//----------------------------------------------------------------------------
inline Bool Rtti::IsExactly(const Rtti& rType) const
{
	return &rType == this;
}

//----------------------------------------------------------------------------
inline const Rtti* Rtti::GetBaseType() const
{
	return mpBaseType;
}
