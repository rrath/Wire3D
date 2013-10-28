#pragma once
#ifndef LOGOFADER_H
#define LOGOFADER_H

#include "WireController.h"

class LogoFader : public Wire::Controller
{
public:
	LogoFader();

	virtual Bool Update(Double appTime);

private:
	Float mBeforeOutTime;
	Float mBeforeInTime;
	Bool mIsFadeOut;

	static const Float s_FadeTime;
	static const Float s_BeforeOutTime;
	static const Float s_BeforeInTime;
};

#endif
