#pragma once
#include "d3d11.h"
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

