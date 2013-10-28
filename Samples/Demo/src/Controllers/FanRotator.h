#pragma once
#ifndef FANROTATOR_H
#define FANROTATOR_H

#include "WireController.h"

class FanRotator : public Wire::Controller
{
public:
	FanRotator(Float speed = 1.0F);

	virtual Bool Update(Double appTime);
	
private:
	Float mSpeed;
};

#endif
