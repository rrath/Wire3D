// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline Renderer* Application::GetRenderer() const
{
	return mpRenderer;
}

//----------------------------------------------------------------------------
inline InputSystem* Application::GetInputSystem() const
{
	return mpInputSystem;
}

//----------------------------------------------------------------------------
inline UInt Application::GetWidth() const
{
	return mWidth;
}

//----------------------------------------------------------------------------
inline UInt Application::GetHeight() const
{
	return mHeight;
}

//----------------------------------------------------------------------------
inline Float Application::GetWidthF() const
{
	return static_cast<Float>(mWidth);
}

//----------------------------------------------------------------------------
inline Float Application::GetHeightF() const
{
	return static_cast<Float>(mHeight);
}
