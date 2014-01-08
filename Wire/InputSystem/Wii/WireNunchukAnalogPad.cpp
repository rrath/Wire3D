// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireNunchukAnalogPad.h"
#include "WireWiiInputDataBuffer.h"
#include "WireMath.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI(Wire, NunchukAnalogPad, AnalogPad);
	
NunchukAnalogPad::NunchukAnalogPad(const InputDevice* pParent)
	: AnalogPad(pParent)
{
}

NunchukAnalogPad::~NunchukAnalogPad()
{
}

/****************************************************************************/
/* Based on code found at: http://forum.wiibrew.org/read.php?11,69317,69317 */
/****************************************************************************/
Float NunchukAnalogPad::GetY() const
{
	static Float driftY = 0;

	if (GetParent()->GetDataBuffer() == NULL)
	{
		return 0;
	}

	const WPADData* pData = static_cast<const WiiInputDataBuffer*>(GetParent()->GetDataBuffer())->GetData();

	if (pData == NULL)
	{
		return 0;
	}

	Float y = pData->exp.nunchuk.js.pos.y - pData->exp.nunchuk.js.min.y;
	y -= (pData->exp.nunchuk.js.max.y - pData->exp.nunchuk.js.min.y) * 0.5f;

	if (MathF::FAbs(y) - 100.0F > driftY)
	{
		if (y < 0)
		{
			driftY = -(y + 100.0F);
		}
		else
		{
			driftY = 100.0F - y;
		}
	}

	if (MathF::FAbs(y + driftY) > 100.0F)
	{
		driftY = 0;
	}

	y += driftY;
	y *= 0.01F;

	if (MathF::FAbs(y) < 0.15F)
	{
		y = 0;
	}

	return y;
}

/****************************************************************************/
/* Based on code found at: http://forum.wiibrew.org/read.php?11,69317,69317 */
/****************************************************************************/
Float NunchukAnalogPad::GetX() const
{
	static Float driftX = 0;

	if (GetParent()->GetDataBuffer() == NULL)
	{
		return 0;
	}

	const WPADData* pData = static_cast<const WiiInputDataBuffer*>(GetParent()->GetDataBuffer())->GetData();

	if (pData == NULL)
	{
		return 0;
	}

	Float x = pData->exp.nunchuk.js.pos.x - pData->exp.nunchuk.js.min.x;
	x -= (pData->exp.nunchuk.js.max.x - pData->exp.nunchuk.js.min.x) * 0.5F;

	if (MathF::FAbs(x) - 100.0F > driftX)
	{
		if (x < 0)
		{
			driftX = -(x + 100.0F);
		}
		else
		{
			driftX = 100.0F - x;
		}
	}

	if (MathF::FAbs(x + driftX) > 100.0F)
	{
		driftX = 0;
	}

	x += driftX;
	x *= 0.01F;

	if (MathF::FAbs(x) < 0.15F)
	{
		x = 0;
	}

	return x;
}

Float NunchukAnalogPad::GetDown() const
{
	return -GetY();
}

Float NunchukAnalogPad::GetLeft() const
{
	return -GetX();
}

Float NunchukAnalogPad::GetRight() const
{
	return GetX();
}

Float NunchukAnalogPad::GetUp() const
{
	return GetY();
}
