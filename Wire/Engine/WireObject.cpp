// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireObject.h"

#include "WireSceneObject.h"

using namespace Wire;

const Rtti Object::TYPE("Wire.Object", NULL);

THashSet<Object*>* Object::InUse = NULL;

//----------------------------------------------------------------------------
Object::Object()
	:
	mReferences(0)
{
#ifdef WIRE_DEBUG
	if (!InUse)
	{
		InUse = WIRE_NEW THashSet<Object*>(32768);
	}

	InUse->Insert(this);
#endif
}

//----------------------------------------------------------------------------
Object::~Object ()
{
#ifdef WIRE_DEBUG
	WIRE_ASSERT(InUse);
	if (InUse)
	{
		Bool found = InUse->Remove(this);
		WIRE_ASSERT(found);
		(void)found;
	}
#endif

	// This assert traps the case when someone tries 'delete spObject'
	// for a smart-pointer-based object. Instead, use 'spObject = NULL'.
	WIRE_ASSERT(mReferences == 0);
}

//----------------------------------------------------------------------------
Bool Object::SaveInUse(const Char* pFilename)
{
	if (InUse->GetQuantity() == 0)
	{
		return true;
	}

	THashSet<Object*>::Iterator it(InUse);

	FILE* pFile = System::Fopen(pFilename, "wb");
	if (!pFile)
	{
		return false;
	}

	for (Object** pObject = it.GetFirst(); pObject; pObject = it.GetNext())
	{
		WIRE_ASSERT(*pObject);
		System::Fprintf(pFile, "%s", static_cast<const Char*>((*pObject)->
			GetType().GetName()));

		SceneObject* pSceneObject = DynamicCast<SceneObject>(*pObject);
		if (pSceneObject)
		{
			System::Fprintf(pFile, " (%s)\n", static_cast<const Char*>(
				pSceneObject->GetName()));
		}
		else
		{
			System::Fprintf(pFile, "\n");
		}
	}

	return (System::Fclose(pFile) == 0);
}
