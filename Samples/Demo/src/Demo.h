#pragma once
#ifndef DEMO_H
#define DEMO_H

#include "WireApplication.h"

namespace Wire
{

class Demo : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	virtual Bool OnInitialize();
	virtual void OnIdle();
	virtual void OnInput();

private:
	enum AppState
	{
		AS_LOADING,
		AS_RUNNING
	};

	void StateLoading(Double time);
	void StateRunning(Double time);

	Node* LoadAndInitLogo();
	NodePtr mspLogo;
	CameraPtr mspLogoCamera;
	Culler mLogoCuller;

	Node* LoadAndInitScene();
	NodePtr mspScene;
	CameraPtr mspSceneCamera;
	CullerSorting mSceneCuller;

	Double mLastTime;
	UInt mAppState;

	// Renderer statistics debug text
	RenderTextPtr mspText;
	Bool mShowFps;
};

WIRE_REGISTER_INITIALIZE(Demo);

}

#endif
