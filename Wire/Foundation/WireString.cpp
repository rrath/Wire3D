// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireString.h"

using namespace Wire;

const Char* String::s_pEmpty = "";

//----------------------------------------------------------------------------
String::String(const Char* pText)
{
    if (pText)
    {
		mLength = System::Strlen(pText);
        mpText = WIRE_NEW Char[mLength+1];
        const size_t size = mLength + 1;
        System::Strcpy(mpText, size, pText);
    }
    else
    {
        mLength = 0;
		mpText = const_cast<Char*>(s_pEmpty);
    }
}

//----------------------------------------------------------------------------
String::String(UInt length, const Char* pText)
{
    if ((length > 0) && !pText)
    {
        WIRE_ASSERT(false);
        length = 0;
    }

    // make sure pText has as many non-null characters as specified
	WIRE_ASSERT(VerifyLength(length, pText) == length);

    mLength = length;
    if (mLength > 0)
    {
        const size_t srcSize = mLength;
        const size_t dstSize = mLength + 1;
        mpText = WIRE_NEW Char[dstSize];
        System::Strncpy(mpText, dstSize, pText, srcSize);
        mpText[mLength] = 0;
    }
    else
    {
		mpText = const_cast<Char*>(s_pEmpty);
    }
}

//----------------------------------------------------------------------------
String::String(const String& rString)
{
    mpText = NULL;
    *this = rString;
}

//----------------------------------------------------------------------------
String::~String()
{
	if(mpText != s_pEmpty)
	{
		WIRE_DELETE[] mpText;
	}
}

//----------------------------------------------------------------------------
String& String::operator= (const String& rString)
{
	if (&rString == this)
	{
		return *this;
	}

	if(mpText != s_pEmpty)
	{
		WIRE_DELETE[] mpText;
	}

    mLength = rString.mLength;
	if (mLength == 0)
	{
		mpText = const_cast<Char*>(s_pEmpty);
		return *this;
	}

    const size_t size = mLength + 1;
    mpText = WIRE_NEW Char[size];
    System::Strcpy(mpText, size, rString.mpText);
    return *this;
}

//----------------------------------------------------------------------------
String& String::operator+= (const String& rString)
{
    mLength += rString.mLength;
    const size_t size = mLength + 1;
    Char* pNew = WIRE_NEW Char[size];
    System::Strcpy(pNew,size,mpText);
	if (mpText != s_pEmpty)
	{
		WIRE_DELETE[] mpText;
	}

	mpText = System::Strcat(pNew, size, rString.mpText);
    return *this;
}

//----------------------------------------------------------------------------
String String::operator+ (const String& rString) const
{
    UInt length = mLength + rString.mLength;
    const size_t size = length + 1;
    Char* pNew = WIRE_NEW Char[size];
    System::Strcpy(pNew, size, mpText);
    System::Strcat(pNew, size, rString.mpText);
    String appended(length, pNew);
    WIRE_DELETE[] pNew;
    return appended;
}

//----------------------------------------------------------------------------
Bool String::operator== (const String& rString) const
{
    return (mLength == rString.mLength)	&&
		(System::Memcmp(mpText, rString.mpText, mLength) == 0);
}

//----------------------------------------------------------------------------
Bool String::operator!= (const String& rString) const
{
    return (mLength != rString.mLength) ||
		(System::Memcmp(mpText, rString.mpText, mLength) != 0);
}

//----------------------------------------------------------------------------
String::operator UInt () const
{
	UInt hash = 5381;

	for (UInt i = 0; i < mLength; i++)
	{
		hash += (hash << 5) + mpText[i];
	}

	return hash;
}

//----------------------------------------------------------------------------
Int String::Find(const String& rSubString, UInt from) const
{
	if (from >= mLength)
	{
		return -1;
	}

	const Char* p = System::Strstr(mpText+from, static_cast<const char*>(
		rSubString));

	if (!p)
	{
		return -1;
	}

	return static_cast<Int>(p - mpText);
}

//----------------------------------------------------------------------------
Bool String::StartsWith(const String& rString) const
{
	if (mLength < rString.GetLength())
	{
		return false;
	}

	return (System::Strncmp(mpText, rString.mpText, rString.GetLength()) == 0);
}

//----------------------------------------------------------------------------
String String::SubString(UInt beginIndex, UInt endIndex) const
{
	WIRE_ASSERT(beginIndex < mLength);
	WIRE_ASSERT(endIndex < mLength);
	if (endIndex < beginIndex)
	{
		return String();
	}

	UInt length = endIndex - beginIndex+1;
	const size_t size = static_cast<size_t>(length + 1);
	Char* pNew = WIRE_NEW char[size];
	System::Strncpy(pNew, size, mpText+beginIndex, length);

	String subString(length, pNew);
	WIRE_DELETE[] pNew;
	return subString;
}

//----------------------------------------------------------------------------
String String::ToUpper() const
{
    String copy(*this);
    for (UInt i = 0; i < mLength; i++)
    {
		copy.mpText[i] = static_cast<Char>(System::Toupper(mpText[i]));
    }

    return copy;
}

//----------------------------------------------------------------------------
String String::ToLower() const
{
    String copy(*this);
    for (UInt i = 0; i < mLength; i++)
    {
		copy.mpText[i] = static_cast<Char>(System::Tolower(mpText[i]));
    }

    return copy;
}

//----------------------------------------------------------------------------
UInt String::VerifyLength(UInt length, const Char* pText) const
{
	if (length > 0)
	{
		UInt i;
		for (i = 0; i < length; i++)
		{
			if (pText[i] == 0)
			{
				break;
			}
		}

		return i;
	}

	return 0;
}
