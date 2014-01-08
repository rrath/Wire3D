// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetDrawCalls() const
{
	return mDrawCalls;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetBatchCount() const
{
	return mBatchCount;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetBatchedStatic() const
{
	return mBatchedStatic;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetBatchedDynamic() const
{
	return mBatchedDynamic;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetBatchedDynamicTransformed() const
{
	return mBatchedDynamicTransformed;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetTriangles() const
{
	return mTriangles;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetVertices() const
{
	return mVertices;
}

// number of buffers and their total size bound to the Renderer
//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetVBOCount() const
{
	return mVBOCount;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetVBOsSize() const
{
	return mVBOsSize;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetIBOCount() const
{
	return mIBOCount;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetIBOsSize() const
{
	return mIBOsSize;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetTextureCount() const
{
	return mTextureCount;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetTexturesSize() const
{
	return mTexturesSize;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetBatchVBOCount() const
{
	return mBatchVBOCount;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetBatchVBOsSize() const
{
	return mBatchVBOsSize;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetShaderCount() const
{
	return mShaderCount;
}


//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetRenderTargetCount() const
{
	return mRenderTargetCount;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetRenderTargetSize() const
{
	return mRenderTargetSize;
}

//----------------------------------------------------------------------------
inline UInt RendererStatistics::GetVertexFormatCount() const
{
	return mVertexFormatCount;
}
