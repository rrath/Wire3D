#pragma once
#ifndef GAME_H
#define GAME_H

#include "WireApplication.h"

#include "ProbeRobot.h"
#include "Player.h"
#include "PhysicsWorld.h"

class Game : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	Game();

	virtual Bool OnInitialize();
	virtual void OnTerminate();
	virtual void OnIdle();

private:
	enum AppState
	{
		AS_LOADING,
		AS_RUNNING
	};

	void OnLoading(Double time, Double deltaTime);
	void OnRunning(Double time, Double deltaTime);
	Wire::Node* LoadAndInitializeLoading();
	Wire::Node* LoadAndInitializeScene();
	Wire::Node* LoadAndInitializeGUI();

	void ProcessInput();

	void DrawWarning(Double time);

	Double mLastApplicationTime;
	UInt mAppState;

	// loading screen
	Wire::NodePtr mspLogo;
	Wire::CameraPtr mspLogoCamera;

	// cursor
	Wire::NodePtr mspGUI;
	Wire::CameraPtr mspGUICamera;

	// scene
	Wire::NodePtr mspScene;
	Wire::CameraPtr mspSceneCamera;
	Wire::SpatialPtr mspCrosshair;
	PhysicsWorldPtr mspPhysicsWorld;

	Wire::Pointer<ProbeRobot> mspProbeRobot;
	Wire::Pointer<Player> mspPlayer;

	Wire::Culler mCuller;
	Wire::CullerSorting mSortingCuller;

	Wire::RenderTextPtr mspText;		// render statistics text

	Wire::RenderTextPtr mspWarningText;
	Bool mShowWarning;

	Wire::Vector2F mCursorPosition;
	Bool mShowColliders;
	Bool mShowFps;
	Bool mWasButton1Pressed;
	Bool mWasButton2Pressed;
};

WIRE_REGISTER_INITIALIZE(Game);

#endif
