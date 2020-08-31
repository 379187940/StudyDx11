#pragma once
//#include "d3d11.h"


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
	UINT miscFlag,
	const char* pInitData = NULL
);
HRESULT CompileShaderFromFile(
	CString fileName,
	const D3D10_SHADER_MACRO* pMacro,
	LPD3D10INCLUDE pInclude,
	char* functionName,
	char* proFile,
	UINT flag1,
	UINT flag2,
	ID3DBlob** pShader 
);
void DeleteComPtr(IUnknown* comPtr);
//会在exe目录或者工作目录查找
bool GetFullPath(CString fileName, CString& result);

char* CustomW2A(const WCHAR* wstr);
#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {delete (p) ; (p) = NULL ;}
#endif
#ifndef V_RETURN
#define V_RETURN(p) (p)
#endif
ID3D11InputLayout* CreateInputLayout(ID3D11Device* pDevice, const std::vector<D3D11_INPUT_ELEMENT_DESC>& input, const void* pShaderBytecodeWithInputSignature, UINT sizeShaderBytecode);
bool UpdateBufferData(ID3D11DeviceContext* pContext, ID3D11Buffer* pBuffer, void* pdata, size_t dataSize);

HWND AfxGetMainWnd();
int2 AfxGetWindowSize();
ID3D11Device* AfxGetDevice();
ID3D11DeviceContext* AfxGetDeviceContext();
 void AfxSetDevice(ID3D11Device*);
 void AfxSetDeviceContext(ID3D11DeviceContext*);