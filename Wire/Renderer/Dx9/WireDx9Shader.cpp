// Wire3D by Roman Rath (antibyte@gmail.com)
// http://wire3d.googlecode.com
// Copyright(c) 2009-2014. All rights reserved.
// 
// The Wire3D source code is supplied under the terms of the LGPL and
// may not be copied or disclosed except in accordance with the terms of
// that agreement.

#include "WireDx9Shader.h"

#include "WireRenderer.h"
#include "WireShader.h"

using namespace Wire;

static const Char DefaultPixelProgram[] =
	"struct VS_OUTPUT\n"
	"{\n"
	"	float4 hposition : POSITION;\n"
	"};\n"
	"struct PS_OUTPUT\n"
	"{\n"
	"	float4 color : COLOR;\n"
	"};\n"
	"PS_OUTPUT main(VS_OUTPUT IN)\n"
	"{\n"
	"	PS_OUTPUT OUT;\n"
	"	OUT.color = float4(1,0,1,1);\n"
	"	return OUT;\n"
	"}\n";

static const Char DefaultVertexProgram[] =
	"float4x4 WIRE_MATRIX_MVP;\n"
	"struct VS_INPUT\n"
	"{\n"
	"	float3 position  : POSITION;\n"
	"};\n"
	"struct VS_OUTPUT\n"
	"{\n"
	"	float4 hposition : POSITION;\n"
	"};\n"
	"VS_OUTPUT main(VS_INPUT IN)\n"
	"{\n"
	"	VS_OUTPUT OUT;\n"
	"	float4 v = float4(IN.position.x,\n"
	"		              IN.position.y,\n"
	"					  IN.position.z,\n"
	"					  1.0f);\n"
	"    OUT.hposition = mul(v, WIRE_MATRIX_MVP);\n"
	"    return OUT;\n"
	"}\n";

//----------------------------------------------------------------------------
PdrShader::PdrShader(Renderer* pRenderer, const Shader* pShader)
	:
	mpConstantTable(NULL),
	mpPixelShader(NULL),
	mpVertexShader(NULL),
	mModelViewProjection(NULL),
	mModelView(NULL),
	mProjection(NULL)
{
	WIRE_ASSERT(pShader);
	HRESULT hr;

	LPD3DXBUFFER pCode = NULL;
	DWORD dwShaderFlags = 0;
	LPD3DXBUFFER pBufferErrors = NULL;
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	const Bool isPixelShader = (pShader->GetProfile()[0] == 'p');

	Char* pProfile = const_cast<Char*>(pShader->GetProfile());
	const Char* const pHighestProfile = isPixelShader ?
		D3DXGetPixelShaderProfile(rDevice) :
		D3DXGetVertexShaderProfile(rDevice);

	WIRE_ASSERT(System::Strlen(pProfile) == System::Strlen(pHighestProfile));
	for (UInt i = 0; pProfile && pProfile[i]; i++)
	{
		if (pHighestProfile[i] < pProfile[i])
		{
			// force down to highest available profile. If compilation fails,
			// shader will be replaced by default shader.
			pProfile = const_cast<Char*>(pHighestProfile);
			break;
		}
	}

	hr = D3DXCompileShader(pShader->GetProgram(), pShader->GetProgramLength(),
		NULL, NULL, pShader->GetFunction(), pProfile, dwShaderFlags, &pCode,
		&pBufferErrors, &mpConstantTable);

	if (FAILED(hr))
	{
		LPVOID pCompileErrors = pBufferErrors->GetBufferPointer();
		MessageBox(NULL, static_cast<const char*>(pCompileErrors),
			"Shader Compile Error", MB_OK|MB_ICONEXCLAMATION);

		if (isPixelShader)
		{
			UINT srcDataLen = static_cast<UINT>(strlen(DefaultPixelProgram));
			hr = D3DXCompileShader(DefaultPixelProgram, srcDataLen, NULL,
				NULL, "main", "ps_2_0", dwShaderFlags, &pCode, &pBufferErrors,
				&mpConstantTable);
			WIRE_ASSERT(SUCCEEDED(hr));
		}
		else
		{
			WIRE_ASSERT(pShader->GetProfile()[0] == 'v');
			UINT srcDataLen = static_cast<UINT>(strlen(DefaultVertexProgram));
			hr = D3DXCompileShader(DefaultVertexProgram, srcDataLen, NULL,
				NULL, "main", "vs_2_0", dwShaderFlags, &pCode, &pBufferErrors,
				&mpConstantTable);
			WIRE_ASSERT(SUCCEEDED(hr));
		}
	}
	else
	{
		ParseSamplerNames(pShader);
	}

	InitBuiltInVariableHandles();

	if (isPixelShader)
	{
		hr = rDevice->CreatePixelShader((DWORD*)pCode->GetBufferPointer(),
			&mpPixelShader);
	}
	else
	{
		WIRE_ASSERT(pShader->GetProfile()[0] == 'v');
		hr = rDevice->CreateVertexShader((DWORD*)pCode->GetBufferPointer(),
			&mpVertexShader);
	}

	WIRE_ASSERT(SUCCEEDED(hr));

	pCode->Release();
	if (pBufferErrors)
	{
		pBufferErrors->Release();
	}
}

//----------------------------------------------------------------------------
PdrShader::~PdrShader()
{
	if (mpPixelShader)
	{
		mpPixelShader->Release();
	}
	else
	{
		WIRE_ASSERT(mpVertexShader);
		mpVertexShader->Release();
	}

	mpConstantTable->Release();
}

//----------------------------------------------------------------------------
void PdrShader::Enable(Renderer* pRenderer)
{
	HRESULT hr;
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	if (mpPixelShader)
	{
		hr = rDevice->SetPixelShader(mpPixelShader);
	}
	else
	{
		WIRE_ASSERT(mpVertexShader);
		hr = rDevice->SetVertexShader(mpVertexShader);
	}

	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void PdrShader::Disable(Renderer* pRenderer)
{
	HRESULT hr;
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	if (mpPixelShader)
	{
		hr = rDevice->SetPixelShader(NULL);
	}
	else
	{
		WIRE_ASSERT(mpVertexShader);
		hr = rDevice->SetVertexShader(NULL);
	}

	WIRE_ASSERT(SUCCEEDED(hr));
}

//----------------------------------------------------------------------------
void PdrShader::SetBuiltInVariables(Renderer* pRenderer)
{
	PdrRendererData* pData = pRenderer->GetRendererData();
	IDirect3DDevice9*& rDevice = pData->D3DDevice;

	if (mModelViewProjection)
	{
		mpConstantTable->SetMatrix(rDevice, mModelViewProjection,
			reinterpret_cast<D3DXMATRIX*>(&pData->ModelViewProjection));
	}

	if (mModelView)
	{
		mpConstantTable->SetMatrix(rDevice, mModelView,
			reinterpret_cast<D3DXMATRIX*>(&pData->ModelView));
	}

	if (mProjection)
	{
		mpConstantTable->SetMatrix(rDevice, mProjection,
			reinterpret_cast<D3DXMATRIX*>(&pData->Projection));
	}
}

//----------------------------------------------------------------------------
void PdrShader::SetMatrix(Renderer* pRenderer, const Char* pName,
	const Matrix4F* pMatrix)
{
	WIRE_ASSERT(pRenderer && pName && pMatrix && mpConstantTable);
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	mpConstantTable->SetMatrix(rDevice, pName, reinterpret_cast<const
		D3DXMATRIX*>(pMatrix));
}

//----------------------------------------------------------------------------
void PdrShader::SetFloat4(Renderer* pRenderer, const Char* pName,
	const Vector4F* pFloat4)
{
	WIRE_ASSERT(pRenderer && pName && pFloat4 && mpConstantTable);
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	mpConstantTable->SetVector(rDevice, pName, reinterpret_cast<const
		D3DXVECTOR4*>(pFloat4));
}

//----------------------------------------------------------------------------
void PdrShader::SetFloat(Renderer* pRenderer, const Char* pName, Float value)
{
	WIRE_ASSERT(pRenderer && pName && mpConstantTable);
	IDirect3DDevice9*& rDevice = pRenderer->GetRendererData()->D3DDevice;
	mpConstantTable->SetFloat(rDevice, pName, value);
}

//----------------------------------------------------------------------------
void PdrShader::InitBuiltInVariableHandles()
{
	mModelViewProjection = mpConstantTable->GetConstantByName(NULL,
		"WIRE_MATRIX_MVP");
	mModelView = mpConstantTable->GetConstantByName(NULL, "WIRE_MATRIX_MV");
	mProjection = mpConstantTable->GetConstantByName(NULL, "WIRE_MATRIX_P");
}

//----------------------------------------------------------------------------
void PdrShader::ParseSamplerNames(const Shader* pShader)
{
	WIRE_ASSERT(mpConstantTable);

	String program(pShader->GetProgram());
	TArray<String> programLines;
	UInt index = 0;

	while (index < program.GetLength())
	{
		UInt lineStartIndex = index;
		while (index < program.GetLength() && program[index] != '\n')
		{
			index++;
		}
	
		if (index > (lineStartIndex+1))
		{
			programLines.Append(program.SubString(lineStartIndex, index));
		}

		index++;
	}

	UInt samplerNameCount = 0;
	for (UInt i = 0; i < programLines.GetQuantity(); i++)
	{
		UInt index = 0;
		SkipWhitespace(programLines[i], index);
		Int samplerAt = programLines[i].Find("sampler", index);
		if (samplerAt != -1)
		{
			index = samplerAt + sizeof("sampler");
			SkipWhitespace(programLines[i], index);
			UInt endIndex = index;
			while ((programLines[i][endIndex] != ' '
				&& programLines[i][endIndex] != '\t'
				&& programLines[i][endIndex] != ';')
				&& endIndex < programLines[i].GetLength())
			{
				endIndex++;
			}

			String samplerName = programLines[i].SubString(index, endIndex-1);
			D3DXHANDLE handle;
			handle = mpConstantTable->GetConstantByName(NULL, static_cast<
				const Char*>(samplerName));
			if (handle)
			{
				D3DXCONSTANT_DESC constDesc;
				UINT count;

				mpConstantTable->GetConstantDesc(handle, &constDesc, &count);

				if (constDesc.RegisterSet == D3DXRS_SAMPLER)
				{
					const_cast<Shader*>(pShader)->SetSamplerIndex(constDesc.
						RegisterIndex, samplerNameCount++);
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
void PdrShader::SkipWhitespace(String& rStr, UInt& rFrom)
{
	while (rStr[rFrom] == ' ' || rStr[rFrom] == '\t')
	{
		rFrom++;
	}
}
