#pragma once
#ifndef PHYSICSWORLD_H
#define PHYSICSWORLD_H

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "CharacterController.h"
#include "RigidBodyController.h"
#include "WireColor32.h"
#include "WireIndexBuffer.h"
#include "WireMatrix3.h"
#include "WireObject.h"
#include "WireQuaternion.h"
#include "WireRenderObject.h"
#include "WireTHashTable.h"
#include "WireVector3.h"
#include "WireVertexBuffer.h"
#include "WireStateWireframe.h"

class PhysicsWorld : public Wire::Object
{
	WIRE_DECLARE_RTTI;

public:
	PhysicsWorld(const Wire::Vector3F& rGravity = Wire::Vector3F(0, -9.81F, 0));
	virtual ~PhysicsWorld();

	inline btDynamicsWorld* Get();

	Int StepSimulation(Double deltaTime, Int maxSubSteps = 1,
		Double fixedTimeStep = 1.0/60.0);

	inline Double GetFixedTimeStep();

	void AddController(PhysicsController* pController, btCollisionObject* pCollisionObject = NULL);
	void RemoveController(PhysicsController* pController);

	// transfer ownership to PhysicsWorld. Shapes are destroyed in its destructor
	void AddCollisionShape(btCollisionShape* pShape, Wire::VertexBuffer* pVBRef = NULL,
		Wire::IndexBuffer* pIBRef = NULL);

	void ToggleDebugShapes(Bool show = true, Bool destroyOnHide = false);

	static inline btVector3 Convert(const Wire::Vector3F& rIn);
	static inline Wire::Vector3F Convert(const btVector3& rIn);
	static inline btMatrix3x3 Convert(const Wire::Matrix3F& rIn);
	static inline Wire::QuaternionF Convert(const btQuaternion& rIn);
	static inline btQuaternion Convert(const Wire::QuaternionF& rIn);
	static btTriangleIndexVertexArray* Convert(Wire::Mesh* pMesh, Bool copy = true);

	Wire::Node* CreateDebugShape(btCollisionShape* pShape);

private:
	Wire::Node* CreateDebugBox(btBoxShape* pBox);
	Wire::Node* CreateDebugSphere(btSphereShape* pSphereShape);
	Wire::Node* CreateDebugCapsule(btCapsuleShape* pCapsuleShape);
	Wire::Node* CreateDebugMesh(btStridingMeshInterface* pMeshInterface);
//	Wire::Node* CreateDebugHull(btConvexHullShape* pConvexHullShape);
	void ApplyVertexColor(const Wire::Color32& rColor, Wire::RenderObject*
		pRenderObject);

	btDefaultCollisionConfiguration* mpCollisionConfiguration;
	btCollisionDispatcher* mpDispatcher;
	btBroadphaseInterface* mpOverlappingPairCache;
	btSequentialImpulseConstraintSolver* mpSolver;
	btDiscreteDynamicsWorld* mpDynamicsWorld;
	btGhostPairCallback* mpGhostPairCallback;

	Wire::THashTable<PhysicsController*, btCollisionObject*> mCollisionObjectMap;

	struct CollisionShapeItem
	{
		CollisionShapeItem(btCollisionShape* pShape = NULL)
			:
			CollisionShape(pShape) {}

		btCollisionShape* CollisionShape;
		Wire::VertexBufferPtr VBReference;
		Wire::IndexBufferPtr IBReference;
	};

	Wire::TArray<CollisionShapeItem> mCollisionShapes;

	Double mFixedTimeStep;

	Wire::RenderObjectPtr mspDebugBox;
	Wire::RenderObjectPtr mspDebugSphere;
	Wire::StateWireframePtr mspDebugWireframe;
	Wire::Color32 mDebugColor;
};

typedef Wire::Pointer<PhysicsWorld> PhysicsWorldPtr;
#include "PhysicsWorld.inl"

#endif
