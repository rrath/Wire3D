// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRERTTI_H
#define WIRERTTI_H

#include "WireTypes.h"

namespace Wire
{

class Rtti
{

public:
	Rtti(const Char* pName, const Rtti* pBaseType);
	~Rtti();

	inline const Char* GetName () const;
	inline Bool IsExactly(const Rtti& rType) const;
	inline const Rtti* GetBaseType() const;
	Bool IsDerived(const Rtti& rType) const;

private:
	const Char* mpName;
	const Rtti* mpBaseType;
};

#include "WireRtti.inl"

}

//----------------------------------------------------------------------------
#define WIRE_DECLARE_RTTI                                                    \
public:                                                                      \
	static const Wire::Rtti TYPE;                                            \
	inline virtual const Wire::Rtti& GetType() const { return TYPE; }

//----------------------------------------------------------------------------
#define WIRE_IMPLEMENT_RTTI(nsName, className, baseClassName)                \
	const Wire::Rtti className::TYPE(#nsName"."#className,                   \
		&baseClassName::TYPE)

//----------------------------------------------------------------------------
#define WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(className, baseClassName)           \
	const Wire::Rtti className::TYPE(#className, &baseClassName::TYPE)

#endif
