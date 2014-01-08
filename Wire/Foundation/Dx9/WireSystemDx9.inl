// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#define WIN32_LEAN_AND_MEAN

#include <sys/timeb.h>
#include <stdlib.h>
#include <windows.h>

//----------------------------------------------------------------------------
Double System::GetTime()
{
	// time with microseconds resolution
	static LARGE_INTEGER s_HRFrequency;
	static LARGE_INTEGER s_InitialHRTime;
	static Bool s_UseHRTimer = true;

	if (!s_InitializedTime)
	{
		if (QueryPerformanceFrequency(&s_HRFrequency))
		{
			QueryPerformanceCounter(&s_InitialHRTime);
			s_InitializedTime = true;
		}
		else
		{
			s_UseHRTimer = false;
		}
	}

	if (s_UseHRTimer)
	{
		LARGE_INTEGER hRTime;
		QueryPerformanceCounter(&hRTime);
		
		LONGLONG timeDiff = hRTime.QuadPart - s_InitialHRTime.QuadPart;
		Double time = (static_cast<Double>(timeDiff)) / static_cast<Double>(
			s_HRFrequency.QuadPart);
		return time;
	}
	
	// fall back if QueryPerformance is not supported:
	// time with milliseconds resolution
	static Long s_InitialSec = 0;
	static Long s_InitialUSec = 0;

	struct timeb tb;

	if (!s_InitializedTime)
	{
		s_InitializedTime = true;
		ftime(&tb);
		s_InitialSec = static_cast<Long>(tb.time);
		s_InitialUSec = 1000 * tb.millitm;
	}

	ftime(&tb);
	Long currentSec = static_cast<Long>(tb.time);
	Long currentUSec = 1000 * tb.millitm;
	Long deltaSec = currentSec - s_InitialSec;
	Long deltaUSec = currentUSec - s_InitialUSec;
	if (deltaUSec < 0)
	{
		deltaUSec += 1000000;
		deltaSec--;
	}

	return 0.001 * static_cast<Double>(1000*deltaSec + deltaUSec/1000);
}

//----------------------------------------------------------------------------
void System::Print(const Char* pFormat, ...)
{
	va_list args;
	va_start(args, pFormat);

	vprintf(pFormat, args);

	va_end(args);
}

#ifdef WIRE_DEBUG
//----------------------------------------------------------------------------
void System::Assert(const Char* pExpression, const Char* pFile,
	Int lineNumber)
{
	size_t convertedChars = 0;

	size_t size = strlen(pExpression) + 1;
	wchar_t* pWcExpression = WIRE_NEW wchar_t[size];
	mbstowcs_s(&convertedChars, pWcExpression, size, pExpression, _TRUNCATE);

	size = strlen(pFile) + 1;
	wchar_t* pWcFile = WIRE_NEW wchar_t[size];
	mbstowcs_s(&convertedChars, pWcFile, size, pFile, _TRUNCATE);

	_wassert(pWcExpression, pWcFile, lineNumber);

	WIRE_DELETE[] pWcFile;
	WIRE_DELETE[] pWcExpression;
}
#endif

//----------------------------------------------------------------------------
FILE* System::Fopen(const Char* pFilename, const Char* pMode)
{
	FILE* pFile;
	errno_t error = fopen_s(&pFile, pFilename, pMode);
	if (error == 0)
	{
		return pFile;
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------
Char* System::Strcpy(Char* pDst, size_t dstSize, const Char* pSrc)
{
	errno_t error = strcpy_s(pDst, dstSize, pSrc);
	if (error == 0)
	{
		return pDst;
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------
Char* System::Strcat(Char* pDst, size_t dstSize, const Char* pSrc)
{
	errno_t error = strcat_s(pDst, dstSize, pSrc);
	if (error == 0)
	{
		return pDst;
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------
Char* System::Strncpy(Char* pDst, size_t dstSize, const Char* pSrc,
	size_t srcSize)
{
	errno_t error = strncpy_s(pDst, dstSize, pSrc, srcSize);
	if (error == 0)
	{
		return pDst;
	}
	else
	{
		return NULL;
	}
}

//----------------------------------------------------------------------------
Int System::Sprintf(Char* pDst, size_t dstSize, const Char* pFormat, ...)
{
	if (!pDst || dstSize == 0 || !pFormat)
	{
		return -1;
	}

	va_list args;
	va_start(args, pFormat);

	Int numWritten = vsprintf_s(pDst, dstSize, pFormat, args);

	va_end(args);
	return numWritten;
}

//----------------------------------------------------------------------------
System::Platform System::GetPlatform()
{
	return System::PF_DX9;
}
