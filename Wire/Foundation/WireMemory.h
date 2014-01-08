// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREMEMORY_H
#define WIREMEMORY_H

#define WIRE_NEW new
#define WIRE_DELETE delete

// #ifndef WIRE_DEBUG
// 
// #define WIRE_NEW new
// #define WIRE_DELETE delete
// 
// #else
// 
// #include "WireTypes.h"
// #include <cstring>	// for size_t
// 
// namespace Wire
// {
// 
// class Memory
// {
// public:
// 	static void* Allocate(size_t size, Char* pFile, UInt line, Bool isArray);
// 	static void Deallocate(void* pAddr, bool isArray);
// 
// 	static size_t AllocatedMemory;
// 	static UInt AllocationCount;
// };
// 
// }
// 
// #ifdef WIRE_WII
// #include <new>
// void* operator new (size_t size) throw (std::bad_alloc);
// void* operator new[](size_t size) throw (std::bad_alloc);
// void* operator new (size_t size, Char* pFile, UInt line) throw (std::bad_alloc);
// void* operator new[] (size_t size, Char* pFile, UInt line) throw (std::bad_alloc);
// void operator delete (void* pAddr) throw();
// void operator delete[] (void* pAddr) throw();
// void operator delete (void* pAddr, Char* pFile, UInt line) throw();
// void operator delete[] (void* pAddr, Char* pFile, UInt line) throw();
// #else
// void* operator new (size_t size);
// void* operator new[](size_t size);
// void* operator new (size_t size, Char* pFile, UInt line);
// void* operator new[] (size_t size, Char* pFile, UInt line);
// void operator delete (void* pAddr);
// void operator delete[] (void* pAddr);
// void operator delete (void* pAddr, Char* pFile, UInt line);
// void operator delete[] (void* pAddr, Char* pFile, UInt line);
// #endif
// 
// //#define WIRE_NEW new(__FILE__,__LINE__)
// #define WIRE_NEW new
// #define WIRE_DELETE delete
// 
// #endif

#endif
