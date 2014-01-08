// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#pragma once
#ifndef WIREOGCDEFINES_H
#define WIREOGCDEFINES_H
#include <gctypes.h>
#include <gccore.h>
#include <ogc/gu.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/machine/processor.h>
#include <wiiuse/wpad.h>

typedef guVector Vec;

#define GX_LIGHT_NULL GX_LIGHTNULL
#define GX_TEXCOORD_NULL GX_TEXCOORDNULL
#define GX_COLOR_NULL GX_COLORNULL

#define GXBegin GX_Begin
#define GXBeginDisplayList GX_BeginDispList
#define GXCallDisplayList GX_CallDispList
#define GXClearVtxDesc GX_ClearVtxDesc
#define GXColor1u32 GX_Color1u32
#define GXColor1x16 GX_Color1x16
#define GXColor1x8 GX_Color1x8
#define GXCopyDisp GX_CopyDisp
#define GXDrawDone GX_DrawDone
#define GXEnd GX_End
#define GXEndDisplayList GX_EndDispList
#define GXGetYScaleFactor GX_GetYScaleFactor
#define GXInit GX_Init
#define GXInitLightAttnK GX_InitLightAttnK
#define GXInitLightColor GX_InitLightColor
#define GXInitLightDir GX_InitLightDir
#define GXInitLightDirv GX_InitLightDirv
#define GXInitLightDistAttn GX_InitLightDistAttn
#define GXInitLightPos GX_InitLightPos
#define GXInitLightPosv GX_InitLightPosv
#define GXInitLightShininess GX_InitLightShininess
#define GXInitLightSpot GX_InitLightSpot
#define GXInitSpecularDirv GX_InitSpecularDirv
#define GXInitTexObj GX_InitTexObj
#define GXInitTexObjFilter GX_InitTexObjFilterMode
#define GXInitTexObjLOD GX_InitTexObjLOD
#define GXInitTexObjMaxAniso GX_InitTexObjMaxAniso
#define GXInitTexObjWrapMode GX_InitTexObjWrapMode
#define GXInvalidateTexAll GX_InvalidateTexAll
#define GXInvalidateVtxCache GX_InvVtxCache
#define GXLoadLightObjImm GX_LoadLightObj
#define GXLoadPosMtxImm GX_LoadPosMtxImm
#define GXLoadNrmMtxImm GX_LoadNrmMtxImm
#define GXLoadTexObj GX_LoadTexObj
#define GXNormal1x16 GX_Normal1x16
#define GXPosition1x16 GX_Position1x16
#define GXPosition1x8 GX_Position1x8
#define GXPosition3f32 GX_Position3f32
#define GXRenderModeObj GXRModeObj
#define GXSetAlphaUpdate GX_SetAlphaUpdate
#define GXSetArray GX_SetArray
#define GXSetBlendMode GX_SetBlendMode
#define GXSetChanAmbColor GX_SetChanAmbColor
#define GXSetChanCtrl GX_SetChanCtrl
#define GXSetChanMatColor GX_SetChanMatColor
#define GXSetColorUpdate GX_SetColorUpdate
#define GXSetCopyClear GX_SetCopyClear
#define GXSetCopyFilter GX_SetCopyFilter
#define GXSetCullMode GX_SetCullMode
#define GXSetDispCopyDst GX_SetDispCopyDst
#define GXSetDispCopyGamma GX_SetDispCopyGamma
#define GXSetDispCopySrc GX_SetDispCopySrc
#define GXSetDispCopyYScale GX_SetDispCopyYScale
#define GXSetFieldMode GX_SetFieldMode
#define GXSetFog GX_SetFog
#define GXSetNumChans GX_SetNumChans
#define GXSetNumTevStages GX_SetNumTevStages
#define GXSetNumTexGens GX_SetNumTexGens
#define GXSetPixelFmt GX_SetPixelFmt
#define GXSetProjection GX_LoadProjectionMtx
#define GXSetScissor GX_SetScissor
#define GXSetTexCoordGen GX_SetTexCoordGen
#define GXSetViewport GX_SetViewport
#define GXSetViewportJitter GX_SetViewportJitter
#define GXSetVtxAttrFmt GX_SetVtxAttrFmt
#define GXSetVtxDesc GX_SetVtxDesc
#define GXSetTevAlphaIn GX_SetTevAlphaIn
#define GXSetTevAlphaOp GX_SetTevAlphaOp
#define GXSetTevColorIn GX_SetTevColorIn
#define GXSetTevColorOp GX_SetTevColorOp
#define GXSetTevKAlphaSel GX_SetTevKAlphaSel
#define GXSetTevOp GX_SetTevOp
#define GXSetTevOrder GX_SetTevOrder
#define GXTexCoord1x16 GX_TexCoord1x16
#define GXTexCoord2f32 GX_TexCoord2f32
#define GXSetZMode GX_SetZMode

#define MTXConcat guMtxConcat
#define MTXFrustum guFrustum
#define MTXIdentity guMtxIdentity
#define MTXInverse guMtxInverse
#define MTXLookAt guLookAt
#define MTXMultVec guVecMultiply
#define MTXMultVecSR guVecMultiplySR
#define MTXOrtho guOrtho
#define MTXPerspective guPerspective
#define MTXRotAxisRad guMtxRotAxisRad
#define MTXTrans guMtxTrans
#define MTXTranspose guMtxTranspose
#define VECAdd guVecAdd
#define VECCrossProduct guVecCross
#define VECDotProduct guVecDotProduct
#define VECScale guVecScale
#define VECSubtract guVecSub

#define PADInit WPAD_Init

#define PPCSync ppcsync

#define VIConfigure VIDEO_Configure
#define VIFlush VIDEO_Flush
#define VIGetNextField VIDEO_GetNextField
#define VIInit VIDEO_Init
#define VISetBlack VIDEO_SetBlack
#define VISetNextFrameBuffer VIDEO_SetNextFramebuffer
#define VIWaitForRetrace VIDEO_WaitVSync

#endif
