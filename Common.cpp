#include "unit.h"
#include "Common.h"
#include <assert.h>
#include <vector>
#include "Scene.h"
extern HWND g_hWnd;
extern ID3D11Device*           g_pd3dDevice;
extern ID3D11DeviceContext*    g_pImmediateContext;
extern CScene g_Scene;
float4x4 g_ProjspaceToScreenSpace;
float2 WorldPosToScreenPos(float3 worldPos)
{
	FirstPersonCamera* pCamera = g_Scene.GetCamera();
	float4x4 viewMatrix = pCamera->GetViewMatrix();
	float4x4 projMatrix = pCamera->GetProjMatrix();
	float4 resultPos = float4(worldPos, 1.0f) * viewMatrix * projMatrix;
	resultPos /= resultPos.w;
	resultPos = resultPos * g_ProjspaceToScreenSpace;
	float2 result(resultPos.x, resultPos.y);
	return result;
}
const float4x4& AfxGetViewportMatrix()
{
	return g_ProjspaceToScreenSpace;
}
bool SetViewPortMatrix(const D3D11_VIEWPORT& viewPort)
{
	g_ProjspaceToScreenSpace = float4x4::Identity();
	g_ProjspaceToScreenSpace.m00 = viewPort.Width/2.0f;
	g_ProjspaceToScreenSpace.m30 = viewPort.TopLeftX + viewPort.Width / 2.0f;
	g_ProjspaceToScreenSpace.m11 = -viewPort.Height / 2.0f;
	g_ProjspaceToScreenSpace.m31 = viewPort.Height / 2.0f + viewPort.TopLeftY;
	g_ProjspaceToScreenSpace.m22 = viewPort.MaxDepth - viewPort.MinDepth;
	g_ProjspaceToScreenSpace.m32 = viewPort.MinDepth;
	return true;
}
CScene* AfxGetScene()
{
	return &g_Scene;
}
ID3D11Device* AfxGetDevice()
{
	return g_pd3dDevice;
}
ID3D11DeviceContext* AfxGetDeviceContext()
{
	return g_pImmediateContext;
}

HWND AfxGetMainWnd()
{
	return g_hWnd;
}
int2 AfxGetWindowSize()
{
	RECT rc;
	GetWindowRect(g_hWnd, &rc);
	return int2(rc.right - rc.left, rc.bottom - rc.top);
}
ID3D11Texture2D* CreateTexture2d(
	ID3D11Device* pDevice,
	DWORD width,
	DWORD height,
	UINT bindFlag,
	DXGI_FORMAT format,
	D3D11_USAGE usage,
	UINT cpuAccessFlag,
	UINT  arraySize /*= 1*/,
	UINT mipLevel /*= 1*/,
	UINT miscFlags /*= 0*/,
	DXGI_SAMPLE_DESC sampleDesc /*= {1,0}*/
)
{
	ID3D11Texture2D* pResultTexture = NULL;
	HRESULT hr;
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.ArraySize = arraySize;
	textureDesc.BindFlags = bindFlag;
	textureDesc.CPUAccessFlags = cpuAccessFlag;
	textureDesc.Format = format;
	textureDesc.MipLevels = mipLevel;
	textureDesc.SampleDesc = sampleDesc;
	textureDesc.Usage = usage;
	textureDesc.MiscFlags = miscFlags;
	hr = pDevice->CreateTexture2D(&textureDesc, NULL, &pResultTexture);
	assert(SUCCEEDED(hr));
	return pResultTexture;
}
ID3D11Buffer* CreateBuffer(
	ID3D11Device* pDevice ,
	UINT byteWidth,
	D3D11_USAGE usage,
	UINT bindFlag,
	UINT cpuAccessFlag,
	UINT miscFlag,
	const char* pInitData /*= NULL */)
{
	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.BindFlags = bindFlag;
	constBufferDesc.CPUAccessFlags = cpuAccessFlag;
	if ( bindFlag == D3D11_BIND_CONSTANT_BUFFER )
		byteWidth = (byteWidth + 15)&~15;
	constBufferDesc.ByteWidth = byteWidth;
	constBufferDesc.Usage = usage;
	D3D11_SUBRESOURCE_DATA subSourceData;
	subSourceData.pSysMem = pInitData;
	ID3D11Buffer* reBuffer;
	HRESULT hr = pDevice->CreateBuffer(&constBufferDesc, 
		subSourceData.pSysMem == nullptr ? nullptr:&subSourceData, 
		&reBuffer);
	assert(SUCCEEDED(hr));
	return reBuffer;
}

bool GetFullPath(CString fileName , CString& result)
{
	if (fileName.Find(_T('\\')) != -1 ||
		fileName.Find(_T('/')) != -1)
	{
		result = fileName;
		return true;
	}
	else
	{
		WCHAR tempStr[MAX_PATH];
		_tgetcwd(tempStr, MAX_PATH - 1);
		CString fullFilePath = tempStr;
		fullFilePath += _T("\\");
		fullFilePath += fileName;
		if (PathFileExists(fullFilePath))
		{
			result = std::move(fullFilePath);
			return true;
		}
		GetModuleFileName(NULL, tempStr, MAX_PATH - 1);
		fullFilePath = tempStr;
		int iFind = fullFilePath.ReverseFind(_T('\\'));
		if (iFind == -1)
			iFind == fullFilePath.ReverseFind(_T('/'));
		assert(iFind != -1);
		fullFilePath = fullFilePath.Left(iFind);
		fullFilePath += _T("\\");
		fullFilePath += fileName;
		if (PathFileExists(fullFilePath))
		{
			result = std::move(fullFilePath);
			return true;
		}
	}
	return false;
}
HRESULT CompileShaderFromFile(
	CString fileName,
	const D3D10_SHADER_MACRO* pMacro,
	LPD3D10INCLUDE pInclude,
	char* functionName,
	char* proFile,
	UINT flag1,
	UINT flag2,
	ID3DBlob** pShader
)
{
	CString fullFilePath;
	if (!GetFullPath(fileName, fullFilePath))
		return S_FALSE;
	HRESULT hr;
	ID3DBlob*	pBlob_Errors = NULL;
	hr = D3DCompileFromFile(fullFilePath, pMacro, D3D_COMPILE_STANDARD_FILE_INCLUDE, functionName, proFile, D3D10_SHADER_ENABLE_STRICTNESS, flag2 , pShader,
		&pBlob_Errors);
	//D3DX11CompileFromFile(fullFilePath, pMacro, pInclude, functionName, proFile, flag1, flag2, pPump, pShader,&pBlob_Errors , &hr);
	if (FAILED(hr))
	{
		if (pBlob_Errors)
		{
			OutputDebugStringA((char*)pBlob_Errors->GetBufferPointer());
			pBlob_Errors->Release();
		}
	}
	return hr;
}
void DeleteComPtr(IUnknown* comPtr)
{
	assert(comPtr);
	comPtr->Release();
}
char* CustomW2A(const WCHAR* wstr)
{
	assert(wstr);
	int len = lstrlenW(wstr);
	DWORD dwMinSize = WideCharToMultiByte(CP_ACP, 0, wstr , len, NULL, 0, NULL, FALSE);
	char *pBuf = new char[dwMinSize + 1];    
	memset(pBuf, 0, dwMinSize + 1);
	dwMinSize = WideCharToMultiByte(CP_ACP, 0, wstr, len, pBuf, dwMinSize, NULL, FALSE);
	return pBuf;
}
ID3D11InputLayout* CreateInputLayout(ID3D11Device* pDevice , const std::vector<D3D11_INPUT_ELEMENT_DESC>& input, const void* pShaderBytecodeWithInputSignature,UINT sizeShaderBytecode)
{
	ID3D11InputLayout* pResult;
	pDevice->CreateInputLayout(input.data(), input.size(), pShaderBytecodeWithInputSignature, sizeShaderBytecode, &pResult);
	assert(pResult);
	return pResult;
}
bool UpdateBufferData(ID3D11DeviceContext* pContext,ID3D11Buffer* pBuffer, void* pdata, size_t dataSize)
{
	D3D11_MAPPED_SUBRESOURCE mapBuffer;
	pContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapBuffer);
	memcpy(mapBuffer.pData, pdata, dataSize);
	pContext->Unmap(pBuffer, 0);
	return true;
}