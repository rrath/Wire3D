// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREWIIINPUTSYSTEM_H
#define WIREWIIINPUTSYSTEM_H

#include "WireInputSystem.h"
#include "WireTHashTable.h"
#include "WireWiiInputDataBuffer.h"
#include "WireWiiMote.h"
#include <wiiuse/wpad.h>

#define MAXIMUM_NUMBER_OF_CHANNELS 4

namespace Wire
{

class WiiInputSystem : public InputSystem
{

public:
	WiiInputSystem();
	virtual ~WiiInputSystem();

	virtual void Capture();
	virtual Bool DiscoverDevices();

private:
	Bool DoDevicesDiscovery();
	void AfterDevicesDiscovery();

	static const UInt FIRST_CHANNEL;
	static const UInt LAST_CHANNEL;

	static UInt s_mEventCounter;

	THashTable<Int, WiiInputDataBuffer*> mDataBufferByChannel;
	Bool mChannelsConnectionStatus[MAXIMUM_NUMBER_OF_CHANNELS];
	Bool mChanged;

	void DiscoverExpansions(WiiMote* pWiiMote);
	Bool GetChannelConnectionStatus(UInt channel);
	WiiMote* GetWiiMoteByChannel(UInt channel);
	WiiInputDataBuffer* GetChannelDataBuffer(UInt channel);

	static void ReadWPADPendingEventsCallback(Int channel, const WPADData* pData);

};

}

#endif
