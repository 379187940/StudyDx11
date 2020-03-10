#include "unit.h"
#include "Common.h"
#include <assert.h>
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
	UINT miscFlag)
{
	D3D11_BUFFER_DESC constBufferDesc;
	ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
	constBufferDesc.BindFlags = bindFlag;
	constBufferDesc.CPUAccessFlags = cpuAccessFlag;
	byteWidth = (byteWidth + 15)&~15;
	constBufferDesc.ByteWidth = byteWidth;
	constBufferDesc.Usage = usage;
	ID3D11Buffer* reBuffer;
	HRESULT hr = pDevice->CreateBuffer(&constBufferDesc, NULL, &reBuffer);
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
	ID3DX11ThreadPump* pPump,
	ID3D10Blob** pShader
)
{
	CString fullFilePath;
	if (!GetFullPath(fileName, fullFilePath))
		return S_FALSE;
	HRESULT hr;
	ID3D10Blob*	pBlob_Errors = NULL;
	D3DX11CompileFromFile(fullFilePath, pMacro, pInclude, functionName, proFile, flag1, flag2, pPump, pShader,&pBlob_Errors , &hr);
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