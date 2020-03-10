#pragma once
#include "d3d11.h"
#include "unit.h"
#include "d3dx11core.h"
ID3D11Texture2D* CreateTexture2d(
	ID3D11Device* pDevice,
	DWORD width,
	DWORD height,
	UINT bindFlag,
	DXGI_FORMAT format,
	D3D11_USAGE usage,
	UINT cpuAccessFlag,
	UINT  arraySize = 1,
	UINT mipLevel = 1,
	UINT miscFlags = 0,
	DXGI_SAMPLE_DESC sampleDesc = {1,0}
);
ID3D11Buffer* CreateBuffer(
	ID3D11Device* pDevice,
	UINT byteWidth ,
	D3D11_USAGE usage,
	UINT bindFlag,
	UINT cpuAccessFlag,
	UINT miscFlag
);
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
);
void DeleteComPtr(IUnknown* comPtr);
//����exeĿ¼���߹���Ŀ¼����
bool GetFullPath(CString fileName, CString& result);
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {delete (p) ; (p) = NULL ;}
#endif
#ifndef V_RETURN
#define V_RETURN(p) (p)
#endif