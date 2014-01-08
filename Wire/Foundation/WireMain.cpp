// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireMain.h"
#include "WireObject.h"

using namespace Wire;

Main::InitializerArray* Main::s_pInitializers = NULL;
Main::TerminatorArray* Main::s_pTerminators = NULL;

UInt Main::s_StartObjectCount = 0;
UInt Main::s_FinalObjectCount = 0;

//----------------------------------------------------------------------------
void Main::AddInitializer(Initializer initializer)
{
	if (!s_pInitializers)
	{
		s_pInitializers = WIRE_NEW InitializerArray(10, 10);
	}

	s_pInitializers->Append(initializer);
}

//----------------------------------------------------------------------------
void Main::AddTerminator(Terminator terminator)
{
	if (!s_pTerminators)
	{
		s_pTerminators = WIRE_NEW TerminatorArray(10, 10);
	}

	s_pTerminators->Append(terminator);
}

//----------------------------------------------------------------------------
void Main::Initialize()
{
	Bool isObjectCountCorrect = true;
	(void)isObjectCountCorrect;

	if (Object::InUse)
	{
		isObjectCountCorrect = false;
		Object::SaveInUse("PreMainWireObjects.txt");
	}

	WIRE_ASSERT(isObjectCountCorrect /* objects should not be created pre Initialize() */);

	// number of objects created before initialization
	s_StartObjectCount = Object::InUse ? Object::InUse->GetQuantity() : 0;

	if (s_pInitializers)
	{
		for (UInt i = 0; i < s_pInitializers->GetQuantity(); i++)
		{
			(*s_pInitializers)[i]();
		}
	}

	WIRE_DELETE s_pInitializers;
	s_pInitializers = NULL;
}

//----------------------------------------------------------------------------
void Main::Terminate()
{
	if (s_pTerminators)
	{
		for (UInt i = 0; i < s_pTerminators->GetQuantity(); i++)
		{
			(*s_pTerminators)[i]();
		}
	}

	// all objects created during the application should be deleted by now
	s_FinalObjectCount = Object::InUse ? Object::InUse->GetQuantity() : 0;
	Bool isObjectCountCorrect = (s_StartObjectCount == s_FinalObjectCount);
	if (!isObjectCountCorrect)
	{
		Object::SaveInUse("LeakingWireObjects.txt");
	}

	WIRE_DELETE s_pTerminators;
	s_pTerminators = NULL;

	if (isObjectCountCorrect)
	{
		// objects should not be deleted post-terminate
		s_FinalObjectCount = Object::InUse ? Object::InUse->GetQuantity() : 0;
		isObjectCountCorrect = s_FinalObjectCount == 0;

		if (!isObjectCountCorrect)
		{
			Object::SaveInUse("PostMainWireObjects.txt");
		}
	}

	// If this assert hits, there is a memory leak of Wire::Object(s).
	// Check the .txt files (see above) for leaked objects.
	WIRE_ASSERT(isObjectCountCorrect);

	// Now that the object leak detection system has completed its tasks,
	// delete the hash table to free up memory so that the debug memory
	// system will not flag it as a leak.
	WIRE_DELETE Object::InUse;
	Object::InUse = NULL;
}
