#pragma once
#include "d3d11.h"
#include "unit.h"
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
