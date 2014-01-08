// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireSystem.h"
#include <cctype>
#include <stdarg.h>
#include <sys/stat.h>

using namespace Wire;

#ifdef WIRE_WII
#include "Wii/WireSystemWii.inl"
#else
#include "Dx9/WireSystemDx9.inl"
#endif

Bool System::s_InitializedTime = false;

const UInt System::MAX_UINT = UINT_MAX;
const Int System::MAX_INT = INT_MAX;
const Int System::MIN_INT = INT_MIN;
const UShort System::MAX_USHORT = USHRT_MAX;
const Short System::MAX_SHORT = SHRT_MAX;
const Short System::MIN_SHORT = SHRT_MIN;
const Float System::MAX_FLOAT = FLT_MAX;
const Float System::MIN_FLOAT = FLT_MIN;

//----------------------------------------------------------------------------
Bool System::IsBigEndian()
{
	UInt i = 1;
	Char* pChar = reinterpret_cast<Char*>(&i);
	return !(*pChar);
}

//----------------------------------------------------------------------------
void* System::Memcpy(void* pDst, size_t dstSize, const void* pSrc,
	size_t srcSize)
{
	if (!pDst || dstSize == 0 || !pSrc || srcSize == 0)
	{
		// Be consistent with the behavior of memcpy_s.
		return NULL;
	}

	if (srcSize > dstSize)
	{
		// The source memory is too large to copy to the destination. To
		// be consistent with memcpy_s, return null as an indication that the
		// copy failed.
		return NULL;
	}

	memcpy(pDst, pSrc, srcSize);
	return pDst;
}

//----------------------------------------------------------------------------
void System::Memset(void* pDst, Int value, size_t count)
{
	memset(pDst, value, count);
}

//----------------------------------------------------------------------------
Int System::Memcmp(const void* pBuf1, const void* pBuf2, size_t size)
{
	return memcmp(pBuf1, pBuf2, size);
}

//----------------------------------------------------------------------------
void* System::Memmove(void* pDst, const void* pSrc, size_t size)
{
	return memmove(pDst, pSrc, size);
}

//----------------------------------------------------------------------------
Bool System::Load(const Char* pFilename, Char*& rBuffer, Int& rSize)
{
	struct stat fileStatus;
	if (stat(pFilename, &fileStatus) != 0)
	{
		// file does not exist
		rBuffer = NULL;
		rSize = 0;
		return false;
	}

	FILE* pFile = System::Fopen(pFilename, "rb");
	WIRE_ASSERT(pFile);
	if (!pFile)
	{
		rBuffer = NULL;
		rSize = 0;
		return false;
	}

	rSize = fileStatus.st_size;
	rBuffer = WIRE_NEW Char[rSize];
	Int readCount = static_cast<Int>(fread(rBuffer, sizeof(Char), rSize,
		pFile));
	if (System::Fclose(pFile) != 0 || readCount != rSize)
	{
		WIRE_ASSERT(false);
		WIRE_DELETE[] rBuffer;
		rBuffer = NULL;
		rSize = 0;
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
Bool System::Load(const Char* pFilename, UChar*& rBuffer, Int& rSize)
{
	return Load(pFilename, reinterpret_cast<Char*&>(rBuffer), rSize);
}

//----------------------------------------------------------------------------
Bool System::Save(const Char* pFilename, const Char* pBuffer, Int size)
{
	if (!pBuffer || size <= 0)
	{
		// The input buffer must exist. It is not possible to verify that
		// the buffer has the specified number of bytes.
		WIRE_ASSERT(false);
		return false;
	}

	FILE* pFile = System::Fopen(pFilename, "wb");
	if (!pFile)
	{
		return false;
	}

	Int writeCount = static_cast<Int>(fwrite(pBuffer, sizeof(Char), size,
		pFile));
	if (System::Fclose(pFile) != 0 || writeCount != size)
	{
		WIRE_ASSERT(false);
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------
Bool System::Save(const Char* pFilename, const UChar* pBuffer, Int size)
{
	return Save(pFilename, reinterpret_cast<const Char*>(pBuffer), size);
}

//----------------------------------------------------------------------------
Int System::Fclose(FILE* pFile)
{
	fflush(pFile);
	return fclose(pFile);
}

//----------------------------------------------------------------------------
Int System::Fprintf(FILE* pFile, const Char* pFormat, ...)
{
	if (!pFile || !pFormat)
	{
		return -1;
	}

	va_list args;
	va_start(args, pFormat);

	Int numWritten = vfprintf(pFile, pFormat, args);

	va_end(args);
	return numWritten;
}

//----------------------------------------------------------------------------
const Char* System::Strstr(const Char* pStr, const Char* pSubStr)
{
	return strstr(pStr, pSubStr);
}

//----------------------------------------------------------------------------
UInt System::Strlen(const Char* pString)
{
	return static_cast<UInt>(strlen(pString));
}

//----------------------------------------------------------------------------
Int System::Strcmp(const Char* pStr1, const Char* pStr2)
{
	return strcmp(pStr1, pStr2);
}

//----------------------------------------------------------------------------
Int System::Strncmp(const Char* pStr1, const Char* pStr2, size_t size)
{
	return strncmp(pStr1, pStr2, size);
}

//----------------------------------------------------------------------------
Int System::Toupper(Int c)
{
	return toupper(c);
}

//----------------------------------------------------------------------------
Int System::Tolower(Int c)
{
	return tolower(c);
}
