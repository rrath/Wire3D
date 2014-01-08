// Wire3D by Pedro Boechat (pboechat@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireWiiInputSystem.h"

#include "WireApplication.h"
#include "WireWiiNunchuk.h"
#include "WireWiiMote.h"
#include <wiiuse/wpad.h>

using namespace Wire;

const UInt WiiInputSystem::FIRST_CHANNEL = 0;
const UInt WiiInputSystem::LAST_CHANNEL = MAXIMUM_NUMBER_OF_CHANNELS - 1;
UInt WiiInputSystem::s_mEventCounter = 0;

WiiInputSystem::WiiInputSystem() :
	mDataBufferByChannel(16),
	mChanged(false)
{
	PAD_Init();
	WPAD_Init();

	// reset channel status
	System::Memset(mChannelsConnectionStatus, false, MAXIMUM_NUMBER_OF_CHANNELS);
}

WiiInputSystem::~WiiInputSystem()
{
	THashTable<Int, WiiInputDataBuffer*>::Iterator it(&mDataBufferByChannel);
	Int key;
	for (WiiInputDataBuffer** pValue = it.GetFirst(&key); pValue; pValue = it.GetNext(&key))
	{
		WIRE_ASSERT(*pValue);
		WIRE_DELETE *pValue;
	}

	WPAD_Shutdown();
}

void WiiInputSystem::AfterDevicesDiscovery()
{
	Application* pApp = Application::GetApplication();
	WIRE_ASSERT(pApp /* Application is not instantiated */);

	for (UInt i = 0; i < mMainDevices.GetQuantity(); i++)
	{
		WiiMote* pWiiMote = DynamicCast<WiiMote>(mMainDevices[i]);
		WIRE_ASSERT(pWiiMote /* MainInputDevice is not WiiMote */);

		// TODO: what about the x and y offsets?
		WPAD_SetVRes(pWiiMote->GetChannel(), pApp->GetWidth(), pApp->GetHeight());
	}
}

WiiInputDataBuffer* WiiInputSystem::GetChannelDataBuffer(UInt channel)
{
	WiiInputDataBuffer** pValue = mDataBufferByChannel.Find(channel);

	if (pValue == NULL)
	{
		WiiInputDataBuffer* pDataBuffer = WIRE_NEW WiiInputDataBuffer;
		mDataBufferByChannel.Insert(channel, pDataBuffer);
		return pDataBuffer;
	}

	return *pValue;
}

void WiiInputSystem::Capture()
{
	WPAD_ReadPending(WPAD_CHAN_ALL,	&WiiInputSystem::ReadWPADPendingEventsCallback);

	for (UInt i = 0; i < mMainDevices.GetQuantity(); i++)
	{
		WiiMote* pWiiMote = DynamicCast<WiiMote>(mMainDevices[i]);
		WIRE_ASSERT(pWiiMote /* MainInputDevice is not WiiMote */);

		UInt channel = pWiiMote->GetChannel();
		WiiInputDataBuffer* pChannelDataBuffer = GetChannelDataBuffer(channel);
		pChannelDataBuffer->SetData(WPAD_Data(channel));
		pWiiMote->SetDataBuffer(pChannelDataBuffer);
	}
}

Bool WiiInputSystem::DiscoverDevices()
{
	Bool hasChanged = DoDevicesDiscovery();
	AfterDevicesDiscovery();
	return hasChanged;
}

WiiMote* WiiInputSystem::GetWiiMoteByChannel(UInt channel)
{
	for (UInt i = 0; i < mMainDevices.GetQuantity(); i++)
	{
		WiiMote* pWiiMote = DynamicCast<WiiMote>(mMainDevices[i]);
		WIRE_ASSERT(pWiiMote /* MainInputDevice is not WiiMote */);

		if (pWiiMote->GetChannel() == channel)
		{
			return pWiiMote;
		}
	}

	return NULL;
}

Bool WiiInputSystem::DoDevicesDiscovery()
{
	WiiMote* pWiiMote;
	for (UInt channel = FIRST_CHANNEL; channel <= LAST_CHANNEL; channel++)
	{
		Bool isConnected = GetChannelConnectionStatus(channel);
		Bool wasConnected = mChannelsConnectionStatus[channel];

		if (isConnected)
		{
			// if the channel is connected and was not connected before, 
			// add a new wiimote to use it
			if (!wasConnected)
			{
				// register wiimote's expected data format
				WPAD_SetDataFormat(channel, WPAD_FMT_BTNS_ACC_IR);

				// create and add the wiimote
				pWiiMote = WIRE_NEW WiiMote(channel);
				pWiiMote->SetUp();

				AddDevice(pWiiMote);

				DiscoverExpansions(pWiiMote);

				mChanged = true;
			}
			// if the channel still connected, get wiimote that is using it
			else
			{
				pWiiMote = GetWiiMoteByChannel(channel);
				WIRE_ASSERT(pWiiMote /* There's no WiiMote connected to channel */);
			}

			// probe channel for new WiiMote expansions
			DiscoverExpansions(pWiiMote);
		}

		// if the channel was connected but is not connected anymore,
		// remove the wiimote using the channel
		else if (!isConnected && wasConnected)
		{
			pWiiMote = GetWiiMoteByChannel(channel);
			WIRE_ASSERT(pWiiMote /* There's no WiiMote connected to channel */);

			// note: all expansions (InputDeviceExtensions) of that WiiMote are removed as well
			RemoveDevice(pWiiMote);

			mChanged = true;
		}

		mChannelsConnectionStatus[channel] = isConnected;
	}
	
	Bool hasChanged = mChanged;
	mChanged = false;

	return hasChanged;
}

void WiiInputSystem::DiscoverExpansions(WiiMote* pWiiMote)
{
	expansion_t data;
	WPAD_Expansion(pWiiMote->GetChannel(), &data);

	InputDeviceExtension* pExpansion;
	const TArray<Pointer<InputDeviceExtension> >& rExtensions = pWiiMote->GetExtensions();

	switch (data.type)
	{
	case WPAD_EXP_NONE:
		// if there was no extension already, exit
		if (pWiiMote->GetExtensionsCount() == 0)
		{
			return;
		}

		for (UInt i = 0; i < rExtensions.GetQuantity(); i++)
		{
			RemoveDevice(rExtensions[i]);
		}
		pWiiMote->RemoveAllExtensions();

		break;

	case WPAD_EXP_NUNCHUK:
		// if a nunchuk was already added, exit
		if (pWiiMote->HasExtension(Nunchuk::TYPE))
		{
			return;
		}

		pExpansion = WIRE_NEW WiiNunchuk(pWiiMote);
		pWiiMote->AddExtension(pExpansion);
		AddDevice(pExpansion);
		pExpansion->SetUp();

		break;

	default:
		WIRE_ASSERT(false /* Unsupported WiiMote expansion */);

		return;
	}

	mChanged = true;
}

Bool WiiInputSystem::GetChannelConnectionStatus(UInt channel)
{
	UInt type;
	return (WPAD_Probe(channel, &type) == WPAD_ERR_NONE);
}

void WiiInputSystem::ReadWPADPendingEventsCallback(Int channel, const WPADData* pData)
{
	WiiInputSystem::s_mEventCounter++;
}
