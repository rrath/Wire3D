//----------------------------------------------------------------------------
inline btDynamicsWorld* PhysicsWorld::Get()
{
	return mpDynamicsWorld;
}

//----------------------------------------------------------------------------
inline Double PhysicsWorld::GetFixedTimeStep()
{
	return mFixedTimeStep;
}

//----------------------------------------------------------------------------
inline btVector3 PhysicsWorld::Convert(const Wire::Vector3F& rIn)
{
	return btVector3(rIn.X(), rIn.Y(), rIn.Z());
}

//----------------------------------------------------------------------------
inline Wire::Vector3F PhysicsWorld::Convert(const btVector3& rIn)
{
	return Wire::Vector3F(rIn.getX(), rIn.getY(), rIn.getZ());
}

//----------------------------------------------------------------------------
inline btMatrix3x3 PhysicsWorld::Convert(const Wire::Matrix3F& rIn)
{
	return btMatrix3x3(
		rIn[0][0], rIn[0][1], rIn[0][2],
		rIn[1][0], rIn[1][1], rIn[1][2],
		rIn[2][0], rIn[2][1], rIn[2][2]);
}

//----------------------------------------------------------------------------
inline Wire::QuaternionF PhysicsWorld::Convert(const btQuaternion& rIn)
{
	return Wire::QuaternionF(rIn.getW(), rIn.getX(), rIn.getY(), rIn.getZ());
}

//----------------------------------------------------------------------------
inline btQuaternion PhysicsWorld::Convert(const Wire::QuaternionF& rIn)
{
	return btQuaternion(rIn.X(), rIn.Y(), rIn.Z(), rIn.W());
}
