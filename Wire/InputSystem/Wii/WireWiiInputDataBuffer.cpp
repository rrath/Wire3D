#include "WireWiiInputDataBuffer.h"

namespace Wire
{

WiiInputDataBuffer::WiiInputDataBuffer()
{
}

WiiInputDataBuffer::~WiiInputDataBuffer()
{
}

void WiiInputDataBuffer::SetData(WPADData* pData)
{
	mpData = pData;
}

const WPADData* WiiInputDataBuffer::GetData() const
{
	return mpData;
}

}
