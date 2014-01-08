// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIRERENDERERSTATISTICS_H
#define WIRERENDERERSTATISTICS_H

#include "WireCamera.h"
#include "WireTransformation.h"

namespace Wire
{

class Renderer;
class RenderText;

class RendererStatistics
{
public:

	// Drawing and Renderer bound memory statistics
	RendererStatistics();

	// accumulated number of draw calls, triangles, etc. since last Reset()
	inline UInt GetDrawCalls() const;
	inline UInt GetBatchCount() const;
	inline UInt GetBatchedStatic() const;
	inline UInt GetBatchedDynamic() const;
	inline UInt GetBatchedDynamicTransformed() const;
	inline UInt GetTriangles() const;
	inline UInt GetVertices() const;

	// number of buffers and their total size bound to the Renderer
	inline UInt GetVBOCount() const;
	inline UInt GetVBOsSize() const;
	inline UInt GetIBOCount() const;
	inline UInt GetIBOsSize() const;
	inline UInt GetTextureCount() const;
	inline UInt GetTexturesSize() const;
	inline UInt GetBatchVBOCount() const;
	inline UInt GetBatchVBOsSize() const;
	inline UInt GetShaderCount() const;
	inline UInt GetRenderTargetCount() const;
	inline UInt GetRenderTargetSize() const;
	inline UInt GetVertexFormatCount() const;

	void Reset();
	void AppendToText(RenderText* pText);
	void AppendToText(RenderText* pText, Float fps, Bool useAverageFps = true);

	// NOTE: If a Camera is supplied, Text layout is omitted and should be
	// handled by the caller. Draw() requires that Renderer::PreDraw() has
	// been called beforehand.
	void Draw(RenderText* pText, const Transformation& rTransformation, Float fps,
		Camera* pCamera = NULL, Bool useAverageFps = true,
		Bool restoreState = false);

private:
	Float AverageFps(Float currentFps);

	// accumulated number of draw calls, triangles and batching statistics
	// since last Reset()
	UInt mDrawCalls;
	UInt mTriangles;
	UInt mVertices;
	UInt mBatchCount;
	UInt mBatchCountMax;
	UInt mBatchedStatic;
	UInt mBatchedDynamic;
	UInt mBatchedDynamicTransformed;
	UInt mBatchedIBOTotalData;
	UInt mBatchedIBOLargestBatch;
	UInt mBatchedIBOMaxLargestBatch;
	UInt mBatchedVBOTotalData;
	UInt mBatchedVBOLargestBatch;
	UInt mBatchedVBOMaxLargestBatch;

	// number of buffers and their total size bound to the Renderer
	UInt mVBOCount;
	UInt mVBOsSize;
	UInt mIBOCount;
	UInt mIBOsSize;
	UInt mTextureCount;
	UInt mTexturesSize;
	UInt mBatchIBOSize;
	UInt mBatchVBOCount;
	UInt mBatchVBOsSize;
	UInt mShaderCount;
	UInt mRenderTargetCount;
	UInt mRenderTargetSize;

	// total number of vertex formats used
	UInt mVertexFormatCount;

	friend class Renderer;

	Renderer* mpRenderer;
	CameraPtr mspCamera;

	enum
	{
		FPS_SAMPLE_QUANTITY = 10
	};

	Float mFpsSamples[FPS_SAMPLE_QUANTITY];
	UInt mFpsSamplesIndex;
};

#include "WireRendererStatistics.inl"

}

#endif
