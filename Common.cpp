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
