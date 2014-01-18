#include "PhysicsWorld.h"

#include "WireIndexBuffer.h"
#include "WireStandardMesh.h"

using namespace Wire;

WIRE_IMPLEMENT_RTTI_NO_NAMESPACE(PhysicsWorld, Wire::Object);

//----------------------------------------------------------------------------
PhysicsWorld::PhysicsWorld(const Vector3F& rGravity)
	:
	mpGhostPairCallback(NULL),
	mCollisionShapes(10, 10),
	mDebugColor(Color32(127, 255, 127, 255))
{
	///collision configuration contains default setup for memory.
	// Advanced users can create their own configuration.
	mpCollisionConfiguration = WIRE_NEW btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you
	// can use a different dispatcher (see Extras/BulletMultiThreaded)
	mpDispatcher = WIRE_NEW btCollisionDispatcher(mpCollisionConfiguration);

	mpOverlappingPairCache = WIRE_NEW btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use
	// a different solver (see Extras/BulletMultiThreaded)
	mpSolver = WIRE_NEW btSequentialImpulseConstraintSolver;

	mpDynamicsWorld = WIRE_NEW btDiscreteDynamicsWorld(mpDispatcher,
		mpOverlappingPairCache, mpSolver, mpCollisionConfiguration);

	mpDynamicsWorld->setGravity(Convert(rGravity));
}

//----------------------------------------------------------------------------
PhysicsWorld::~PhysicsWorld()
{
	//cleanup in the reverse order of creation/initialization
	while (mCollisionObjectMap.GetQuantity() > 0)
	{
		THashTable<PhysicsController*, btCollisionObject*>::Iterator it(&mCollisionObjectMap);
		PhysicsController* pKey = NULL;
		it.GetFirst(&pKey);
		WIRE_ASSERT(pKey);
		pKey->SetEnabled(false);
		pKey->mpPhysicsWorld = NULL;
	}

	//delete collision shapes
	for (UInt i = 0; i < mCollisionShapes.GetQuantity(); i++)
	{
		btCollisionShape* pShape = mCollisionShapes[i].CollisionShape;
		if (pShape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
		{
			btBvhTriangleMeshShape* pMeshShape = static_cast<btBvhTriangleMeshShape*>(pShape);
			btTriangleIndexVertexArray* pMeshInterface = static_cast<btTriangleIndexVertexArray*>(
				pMeshShape->getMeshInterface());
			IndexedMeshArray& rMeshArray = pMeshInterface->getIndexedMeshArray();

			if (!mCollisionShapes[i].VBReference &&
				!mCollisionShapes[i].IBReference)
			{
				for (Int k = 0; k < rMeshArray.size(); k++)
				{
					WIRE_DELETE[] rMeshArray[k].m_triangleIndexBase;
					rMeshArray[k].m_triangleIndexBase = NULL;
					WIRE_DELETE[] rMeshArray[k].m_vertexBase;
					rMeshArray[k].m_vertexBase = NULL;
				}
			}

			WIRE_DELETE pMeshInterface;
		}

		WIRE_DELETE pShape;
		mCollisionShapes[i] = CollisionShapeItem();
	}

	WIRE_DELETE mpGhostPairCallback;
	WIRE_DELETE mpDynamicsWorld;
	WIRE_DELETE mpSolver;
	WIRE_DELETE mpOverlappingPairCache;
	WIRE_DELETE mpDispatcher;
	WIRE_DELETE mpCollisionConfiguration;
}

//----------------------------------------------------------------------------
Vector3F PhysicsWorld::GetGravity()
{
	btVector3 btGravity = mpDynamicsWorld->getGravity();
	return Convert(btGravity);
}

//----------------------------------------------------------------------------
void PhysicsWorld::SetGravity(const Vector3F& rGravity)
{
	mpDynamicsWorld->setGravity(Convert(rGravity));
}

//----------------------------------------------------------------------------
Int PhysicsWorld::StepSimulation(Double deltaTime, Int maxSubSteps,
	Double fixedTimeStep)
{
	mFixedTimeStep = fixedTimeStep;

	THashTable<PhysicsController*, btCollisionObject*>::Iterator it(&mCollisionObjectMap);
	PhysicsController* pKey;
	for (btCollisionObject** pValue = it.GetFirst(&pKey); pValue;
		pValue = it.GetNext(&pKey))
	{
		WIRE_ASSERT(pValue && pKey);
		pKey->PhysicsUpdate(deltaTime);
	}

	return mpDynamicsWorld->stepSimulation(btScalar(deltaTime), maxSubSteps,
		btScalar(fixedTimeStep));
}

//----------------------------------------------------------------------------
void PhysicsWorld::AddController(PhysicsController* pController,
	btCollisionObject* pCollisionObject)
{
	WIRE_ASSERT(pController);
	WIRE_ASSERT(mCollisionObjectMap.Find(pController) == NULL);

	if (pController->IsDerived(CharacterController::TYPE))
	{
		WIRE_ASSERT(pCollisionObject && btRigidBody::upcast(pCollisionObject) == NULL);
		if (!mpGhostPairCallback)
		{
			mpGhostPairCallback = WIRE_NEW btGhostPairCallback();
			mpDynamicsWorld->getPairCache()->setInternalGhostPairCallback(
				mpGhostPairCallback);
		}
	}

	mCollisionObjectMap.Insert(pController, pCollisionObject);
}

//----------------------------------------------------------------------------
void PhysicsWorld::RemoveController(PhysicsController* pController)
{
	WIRE_ASSERT(pController);
	WIRE_ASSERT(mCollisionObjectMap.Find(pController));
	mCollisionObjectMap.Remove(pController);
}

//----------------------------------------------------------------------------
void PhysicsWorld::AddCollisionShape(btCollisionShape* pShape, VertexBuffer*
	pVBRef, IndexBuffer* pIBRef)
{
	CollisionShapeItem item;
	item.CollisionShape = pShape;
	item.VBReference = pVBRef;
	item.IBReference = pIBRef;
	mCollisionShapes.Append(item);
}

//----------------------------------------------------------------------------
void PhysicsWorld::ToggleDebugShapes(Bool show, Bool destroyOnHide)
{
	THashTable<PhysicsController*, btCollisionObject*>::Iterator it(&mCollisionObjectMap);
	PhysicsController* pKey;

	for (btCollisionObject** pValue = it.GetFirst(&pKey); pValue;
		pValue = it.GetNext(&pKey))
	{
		WIRE_ASSERT(pValue && pKey);
		pKey->ToggleDebugShape(show, destroyOnHide);
	}
}

//----------------------------------------------------------------------------
btTriangleIndexVertexArray* PhysicsWorld::Convert(Mesh* pMesh, Bool copy)
{
	IndexBuffer* pIndexBuffer = pMesh->GetIndexBuffer();
	UShort* pTriangleIndexBase = pIndexBuffer->GetData();
	VertexBuffer* pVertexBuffer = pMesh->GetPositionBuffer();
	Vector3F* pVertexBase = &(pVertexBuffer->Position3(0));
	const VertexAttributes& rAttr = pVertexBuffer->GetAttributes();
	Int vertexStride = rAttr.GetChannelQuantity() * sizeof(Float);

	if (copy)
	{
		pTriangleIndexBase = WIRE_NEW UShort[pIndexBuffer->GetQuantity()];
		UInt size = pIndexBuffer->GetQuantity() * sizeof(UShort);
		System::Memcpy(pTriangleIndexBase, size, pIndexBuffer->GetData(), size);

		pVertexBase = WIRE_NEW Vector3F[pVertexBuffer->GetQuantity()];
		size = pVertexBuffer->GetQuantity() * sizeof(Vector3F);

		if (rAttr.GetChannelQuantity() == rAttr.GetPositionChannels())
		{
			System::Memcpy(pVertexBase, size, pVertexBuffer->GetPosition(), size);
		}
		else
		{
			vertexStride = sizeof(Vector3F);
			for (UInt i = 0; i < pVertexBuffer->GetQuantity(); i++)
			{
				pVertexBase[i] = pVertexBuffer->Position3(i);
			}
		}
	}

	btIndexedMesh indexedMesh;
	WIRE_ASSERT((pIndexBuffer->GetQuantity() % 3) == 0);
	indexedMesh.m_numTriangles = pIndexBuffer->GetQuantity() / 3;
	indexedMesh.m_triangleIndexBase = reinterpret_cast<const UChar*>(
		pTriangleIndexBase);
	indexedMesh.m_triangleIndexStride = sizeof(UShort) * 3;
	indexedMesh.m_numVertices = pVertexBuffer->GetQuantity();
	indexedMesh.m_vertexBase = reinterpret_cast<const UChar*>(pVertexBase);
	indexedMesh.m_vertexStride = vertexStride;

	btTriangleIndexVertexArray* pTriangleIndexVertexArray =
		WIRE_NEW btTriangleIndexVertexArray();
	pTriangleIndexVertexArray->addIndexedMesh(indexedMesh, PHY_SHORT);
	return pTriangleIndexVertexArray;
}

//----------------------------------------------------------------------------
Node* PhysicsWorld::CreateDebugShape(btCollisionShape* pShape)
{
	Node* pNode = NULL;
	if (pShape->getShapeType() == BOX_SHAPE_PROXYTYPE)
	{
		pNode = CreateDebugBox(static_cast<btBoxShape*>(pShape));
	}
	else if (pShape->getShapeType() == SPHERE_SHAPE_PROXYTYPE)
	{
		pNode =  CreateDebugSphere(static_cast<btSphereShape*>(pShape));
	}
	else if (pShape->getShapeType() == CAPSULE_SHAPE_PROXYTYPE)
	{
		pNode =  CreateDebugCapsule(static_cast<btCapsuleShape*>(pShape));
	}
	else if (pShape->getShapeType() == TRIANGLE_MESH_SHAPE_PROXYTYPE)
	{
		pNode =  CreateDebugMesh(static_cast<btBvhTriangleMeshShape*>(pShape)->
			getMeshInterface());
	}
	else if (pShape->getShapeType() == CONVEX_TRIANGLEMESH_SHAPE_PROXYTYPE)
	{
		pNode =  CreateDebugMesh(static_cast<btConvexTriangleMeshShape*>(
			pShape)->getMeshInterface());
	}
	else if (pShape->getShapeType() == CONVEX_HULL_SHAPE_PROXYTYPE)
	{
		// TODO: implement debug shape
		return NULL;
	}
	else if (pShape->getShapeType() == EMPTY_SHAPE_PROXYTYPE)
	{
		return NULL;
	}
	else
	{
		WIRE_ASSERT(false /* collision shape not supported */);
		return NULL;
	}

	if (!mspDebugWireframe)
	{
		mspDebugWireframe = WIRE_NEW StateWireframe;
		mspDebugWireframe->Enabled = true;
	}

	WIRE_ASSERT(pNode);
	pNode->AttachState(mspDebugWireframe);
	pNode->UpdateRS();
	return pNode;
}

//----------------------------------------------------------------------------
Node* PhysicsWorld::CreateDebugBox(btBoxShape* pBox)
{
	// Resetting margin to acquire the real half extents
	Float margin = pBox->getMargin();
	pBox->setMargin(0);
	btVector3 halfExtentsWithoutMargin = pBox->getHalfExtentsWithoutMargin();
	pBox->setMargin(margin);
	Vector3F halfExtents = PhysicsWorld::Convert(halfExtentsWithoutMargin);
		
	if (!mspDebugBox)
	{
		mspDebugBox = StandardMesh::CreateCube8(4);
		ApplyVertexColor(mDebugColor, mspDebugBox);
	}

	Node* pNode = WIRE_NEW Node(mspDebugBox);

	// fix flat collision boxes (0 in one dimension) for rendering
	const Float zt = MathF::ZERO_TOLERANCE;
	halfExtents.X() = halfExtents.X() == 0.0F ? zt : halfExtents.X();
	halfExtents.Y() = halfExtents.Y() == 0.0F ? zt : halfExtents.Y();
	halfExtents.Z() = halfExtents.Z() == 0.0F ? zt : halfExtents.Z();

	pNode->Local.SetScale(halfExtents);
	pNode->World.SetScale(halfExtents);
	return pNode;
}

//----------------------------------------------------------------------------
Node* PhysicsWorld::CreateDebugSphere(btSphereShape* pSphereShape)
{
	if (!mspDebugSphere)
	{
		mspDebugSphere = StandardMesh::CreateSphere(10, 10, 1, 0, 4);
		ApplyVertexColor(mDebugColor, mspDebugSphere);
	}

	Node* pNode = WIRE_NEW Node(mspDebugSphere);
	pNode->Local.SetUniformScale(pSphereShape->getRadius());
	pNode->World.SetUniformScale(pSphereShape->getRadius());
	return pNode;
}

//----------------------------------------------------------------------------
Node* PhysicsWorld::CreateDebugCapsule(btCapsuleShape* pCapsuleShape)
{
	RenderObject* pDebugCapsule = StandardMesh::CreateCapsule(10, 10,
		pCapsuleShape->getRadius(), pCapsuleShape->getHalfHeight()*2+
		pCapsuleShape->getRadius()*2, 0, 4);

	Int axis = pCapsuleShape->getUpAxis();
	Transformation transformation;
	Matrix3F rotation;
	VertexBuffer* pVertexBuffer = pDebugCapsule->GetMesh()->GetVertexBuffer();

	switch (axis)
	{
	case 0 :
		rotation.FromAxisAngle(Vector3F(0, 1, 0), MathF::HALF_PI);
		transformation.SetRotate(rotation);
		pVertexBuffer->ApplyForward(transformation, pVertexBuffer->GetData());
		break;
	case 1:
		rotation.FromAxisAngle(Vector3F(1, 0, 0), MathF::HALF_PI);
		transformation.SetRotate(rotation);
		pVertexBuffer->ApplyForward(transformation, pVertexBuffer->GetData());
		break;
	case 2:
		break;
	default:
		WIRE_ASSERT(false);
	}

	ApplyVertexColor(mDebugColor, pDebugCapsule);

	Node* pNode = WIRE_NEW Node(pDebugCapsule);
	return pNode;
}

//----------------------------------------------------------------------------
Node* PhysicsWorld::CreateDebugMesh(btStridingMeshInterface* pMeshInterface)
{
	WIRE_ASSERT(pMeshInterface);

	VertexAttributes attributes;
	attributes.SetPositionChannels(3);
	attributes.SetColorChannels(4);

	UChar* pVertexBase;
	Int vertexCount;
	PHY_ScalarType type;
	Int stride;
	UChar* pIndexBase;
	Int triangleStride;
	Int triangleCount;
	PHY_ScalarType indexType;

	pMeshInterface->getLockedVertexIndexBase(&pVertexBase,
		vertexCount, type, stride, &pIndexBase, triangleStride, triangleCount,
		indexType);

	WIRE_ASSERT(type == PHY_FLOAT);
	WIRE_ASSERT(triangleStride == 3*sizeof(UShort));
	WIRE_ASSERT(indexType == PHY_SHORT);
	if (type != PHY_FLOAT || triangleStride != 3*sizeof(UShort) ||
		indexType != PHY_SHORT)
	{
		return NULL;
	}

	VertexBuffer* pVertexBuffer = WIRE_NEW VertexBuffer(attributes,
		vertexCount);
	IndexBuffer* pIndexBuffer = WIRE_NEW IndexBuffer(triangleCount*3);

	for (Int i = 0; i < vertexCount; i++)
	{
		Float* pVertexData = reinterpret_cast<Float*>(pVertexBase + i * stride);
		Vector3F v;
		v.X() = *pVertexData++;
		v.Y() = *pVertexData++;
		v.Z() = *pVertexData++;
		pVertexBuffer->Position3(i) = v;
		pVertexBuffer->Color4(i) = mDebugColor;
	}

	UShort* pIndexData = reinterpret_cast<UShort*>(pIndexBase);
	for (Int i = 0; i < triangleCount*3; i += 3)
	{
		(*pIndexBuffer)[i] = pIndexData[i];
		(*pIndexBuffer)[i+1] = pIndexData[i+1];
		(*pIndexBuffer)[i+2] = pIndexData[i+2];
	}

	Mesh* pMesh = WIRE_NEW Mesh(pVertexBuffer, pIndexBuffer);
	RenderObject* pDebugMesh = WIRE_NEW RenderObject(pMesh);
	Node* pNode = WIRE_NEW Node(pDebugMesh);
	return pNode;
}

//----------------------------------------------------------------------------
void PhysicsWorld::ApplyVertexColor(const Color32& rColor,
	RenderObject* pRenderObject)
{
	WIRE_ASSERT(pRenderObject);
	VertexBuffer* pVertexBuffer = pRenderObject->GetMesh()->GetVertexBuffer();
	for (UInt i = 0; i < pVertexBuffer->GetQuantity(); i++)
	{
		pVertexBuffer->Color4(i) = rColor;
	}
}
