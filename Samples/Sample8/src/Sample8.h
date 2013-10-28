#pragma once
#ifndef SAMPLE8_H
#define SAMPLE8_H

#include "WireApplication.h"
#include "btBulletDynamicsCommon.h"

namespace Wire
{

class Sample8 : public WIREAPPLICATION
{
	WIRE_DECLARE_INITIALIZE;

	typedef WIREAPPLICATION Parent;

public:
	Sample8();
	virtual Bool OnInitialize();
	virtual void OnIdle();
	virtual void OnTerminate();

private:
	struct GameObject  
	{
		GameObject() {};
		GameObject(RenderObject* pRenderObject, btCollisionShape* pCol,
			btRigidBody* pRigidBody)
		{
			spRenderObject = pRenderObject;
			pCollisionObject = pCol;
			pRigidBody = pRigidBody;
		}

		RenderObjectPtr spRenderObject;
		Transformation WorldTransformation;
		btCollisionShape* pCollisionObject;
		btRigidBody* pRigidBody;
	};

	void CreateGameObjects();
	Material* CreateMaterial();
	void CreatePhysicsWorld();
	btRigidBody* CreateRigidBody(btCollisionShape* pColShape, Float mass,
		Vector3F position, Float extent = 1.0F);
	void ResetRigidBodies();

	void RandomizeBallVelocity(btRigidBody* pBody);
	void UpdatePhysicsWorld(btScalar elapsedTime);
	void DestroyPhysicsWorld();


	inline btVector3 Convert(Vector3F in)
	{
		return btVector3(in.X(), in.Y(), in.Z());
	}

	inline Vector3F Convert(btVector3& in)
	{
		return Vector3F(in.getX(), in.getY(), in.getZ());
	}

	inline QuaternionF Convert(const btQuaternion& in)
	{
		return QuaternionF(in.getW(), in.getX(), in.getY(), in.getZ());
	}


	CameraPtr mspCamera;
	Culler mCuller;

	TArray<GameObject> mGameObjects;

	RenderObjectPtr mspBox;
	RenderObjectPtr mspBall;
	BoundingVolumePtr mspWorldBound;
	MaterialPtr mspMaterial;
	StateMaterialPtr mspStateMaterial;
	LightPtr mspLightA;
	LightPtr mspLightB;

	Float mAngle;
	Double mLastTime;
	Double mResetTime;
	Random mRandom;

	btDefaultCollisionConfiguration* mpCollisionConfiguration;
	btCollisionDispatcher* mpDispatcher;
	btAxisSweep3* mpOverlappingPairCache;
	btSequentialImpulseConstraintSolver* mpSolver;
	btDiscreteDynamicsWorld* mpDynamicsWorld;

	static const UInt s_BoxCountX = 5;
	static const UInt s_BoxCountY = 6;
};

WIRE_REGISTER_INITIALIZE(Sample8);

}

#endif
