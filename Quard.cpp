#include "quard.h"
#include <assert.h>
#include<tchar.h>
#include "Common.h"
CQuard::CQuard(wstring strName) :
	CBaseRenderObject(strName)
{

}


CQuard::~CQuard()
{
	//m_pDepthTextureSRV->Release();
	m_pDepthState->Release();
	m_pSampleState->Release();
	m_pDepthTexutre->Release();
	m_pDepthTexutreTemp->Release();
	m_pDepthView->Release();
	m_pConvertDepthToWorld->Release();
}

void CQuard::Tick(DWORD dwTimes)
{
}
bool CQuard::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	assert(pd3dDevice);
	assert(pContext);
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
	ID3D10Blob* pVertexShader = NULL;
	ID3D10Blob* pErrorMsg = NULL;
	HRESULT hr;
	hr = CompileShaderFromFile(_T("quard.hlsl"), NULL, NULL, "VS_RenderQuad", "vs_4_0", 0, 0, NULL, &pVertexShader);
	if (FAILED(hr))
		assert(false);
	D3D11_INPUT_ELEMENT_DESC desc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_pLayoutInput = NULL;
	m_pd3dDevice->CreateInputLayout(desc, ARRAYSIZE(desc), pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pLayoutInput);
	assert(m_pLayoutInput);
	SpriteVertex QuadVertices[6];
	QuadVertices[0].v3Pos = XMFLOAT3(-1.0f, -1.0f, 0.5f);
	QuadVertices[0].v2TexCoord = XMFLOAT2(0.0f, 1.0f);
	QuadVertices[1].v3Pos = XMFLOAT3(-1.0f, 1.0f, 0.5f);
	QuadVertices[1].v2TexCoord = XMFLOAT2(0.0f, 0.0f);
	QuadVertices[2].v3Pos = XMFLOAT3(1.0f, -1.0f, 0.5f);
	QuadVertices[2].v2TexCoord = XMFLOAT2(1.0f, 1.0f);
	QuadVertices[3].v3Pos = XMFLOAT3(-1.0f, 1.0f, 0.5f);
	QuadVertices[3].v2TexCoord = XMFLOAT2(0.0f, 0.0f);
	QuadVertices[4].v3Pos = XMFLOAT3(1.0f, 1.0f, 0.5f);
	QuadVertices[4].v2TexCoord = XMFLOAT2(1.0f, 0.0f);
	QuadVertices[5].v3Pos = XMFLOAT3(1.0f, -1.0f, 0.5f);
	QuadVertices[5].v2TexCoord = XMFLOAT2(1.0f, 1.0f);

	// Create the vertex buffer
	D3D11_BUFFER_DESC BD;
	BD.Usage = D3D11_USAGE_DYNAMIC;
	BD.ByteWidth = sizeof(SpriteVertex) * 6;
	BD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BD.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = QuadVertices;
	hr = pd3dDevice->CreateBuffer(&BD, &InitData, &m_pVertexBuffer);
	//assert(D3D_OK == hr);
	if (FAILED(m_pd3dDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), NULL, &m_pVertexShader)))
	{
		pVertexShader->Release();
		assert(false); return false;
	}
	ID3D10Blob* pPixelShader = NULL;
	ID3D10Blob*	pBlob_Errors = NULL;

	hr = CompileShaderFromFile(_T("quard.hlsl"), NULL, NULL, "PS_RenderCameraZ", "ps_4_0", 0, 0, NULL, &pPixelShader);
	LPVOID l_pError = NULL;
	if (FAILED(hr))
	{
		l_pError = pBlob_Errors->GetBufferPointer();
		assert(false); return false;
	}
	if (FAILED(m_pd3dDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), NULL, &m_pPixelShader)))
	{
		assert(false);
		return false;
	}
	static UINT viewportNum = 1;
	D3D11_VIEWPORT viewPort;
	pContext->RSGetViewports(&viewportNum, &viewPort);
	m_pDepthTexutre = CreateTexture2d(
		m_pd3dDevice,
		viewPort.Width,
		viewPort.Height,
		D3D11_BIND_SHADER_RESOURCE,
		DXGI_FORMAT_R32_TYPELESS,
		D3D11_USAGE_DEFAULT,
		0
		);
	m_pDepthTexutreTemp = CreateTexture2d(
		m_pd3dDevice,
		viewPort.Width,
		viewPort.Height,
		0,
		DXGI_FORMAT_X24_TYPELESS_G8_UINT,
		D3D11_USAGE_STAGING,
		D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE
	);
	D3D11_SHADER_RESOURCE_VIEW_DESC SRDesc;
	SRDesc.Format = DXGI_FORMAT_R32_FLOAT;
	SRDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRDesc.Texture2D.MostDetailedMip = 0;
	SRDesc.Texture2D.MipLevels = 1;
	hr = m_pd3dDevice->CreateShaderResourceView(m_pDepthTexutre, &SRDesc, &m_pDepthView);
	assert(SUCCEEDED(hr));
	D3D11_DEPTH_STENCIL_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthDesc.DepthEnable = false;
	m_pd3dDevice->CreateDepthStencilState(&depthDesc, &m_pDepthState);
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = m_pd3dDevice->CreateSamplerState(&sampDesc, &m_pSampleState);
	if (FAILED(hr))
		return hr;
	m_pConvertDepthToWorld = CreateBuffer(m_pd3dDevice, sizeof(CBufferConvertToWorld), D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER, 0, 0);
	return true;
}
extern ID3D11RenderTargetView* g_pRenderTargetView ;
extern ID3D11DepthStencilView* g_pDepthStencilView ;
bool CQuard::Render(DWORD dwTimes)
{
	HRESULT hr;
	static UINT viewportNum = 1;
	D3D11_VIEWPORT viewPort;
	m_pContext->RSGetViewports(&viewportNum, &viewPort);
	
	ID3D11Resource* pResource = NULL;
	m_pDepthTextureSRV->GetResource(&pResource);
	D3D11_RESOURCE_DIMENSION resDimension;
	pResource->GetType(&resDimension);
	m_pContext->CopyResource(m_pDepthTexutreTemp, pResource);

	D3D11_MAPPED_SUBRESOURCE mappedTexture;
	hr = m_pContext->Map(m_pDepthTexutreTemp, 0, D3D11_MAP_READ_WRITE, 0, &mappedTexture);
	/*D3D11_MAPPED_SUBRESOURCE mappedTextureReal;
	hr = m_pContext->Map(m_pDepthTexutre, 0, D3D11_MAP_READ, 0, &mappedTextureReal);*/
	DWORD* pDepthData = static_cast<DWORD*>(mappedTexture.pData);
	float* pDepthDataReal = new float[viewPort.Height*viewPort.Width]/*static_cast<DWORD*>(mappedTextureReal.pData)*/;
	for ( int i = 0 ; i < viewPort.Height ; i++ )
		for (int j = 0; j < viewPort.Width; j++)
		{
			DWORD temp =  pDepthData[(int)viewPort.Width*i  + j] ;
			temp = temp & 0x00FFFFFF;
				
			float depth = (float)temp/0x00FFFFFF;
			//if (depth != 1.0f)
			pDepthDataReal[(int)viewPort.Width*i + j] = depth;
		}

	D3D11_BOX destRegion;
	destRegion.left = 0;
	destRegion.right = viewPort.Width;
	destRegion.top = 0;
	destRegion.bottom = viewPort.Height;
	destRegion.front = 0;
	destRegion.back = 1;

	m_pContext->UpdateSubresource(m_pDepthTexutre, 0, &destRegion, pDepthDataReal, mappedTexture.RowPitch, mappedTexture.DepthPitch);
	/*m_pContext->Unmap(m_pDepthTexutre, 0);*/
	m_pContext->Unmap(m_pDepthTexutreTemp, 0);
	//m_pContext->CopyResource(pResource, m_pDepthTexutre);
	pResource->Release();
	D3D11_TEXTURE2D_DESC  desc;
	m_pDepthTexutre->GetDesc(&desc);
	UINT Stride = sizeof(SpriteVertex);
	UINT Offset = 0;
	m_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
	ID3D11DepthStencilState* oldState = NULL ;
	UINT temp;
	m_pContext->OMGetDepthStencilState(&oldState, &temp);
	m_pContext->OMSetDepthStencilState(m_pDepthState,0);
	//g_pRenderCameraZ->GetPassByIndex(0)->Apply(0);
	
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->VSSetShader(m_pVertexShader, NULL, 0);
	m_pContext->PSSetShader(m_pPixelShader, NULL, 0);
	m_pContext->PSSetShaderResources(0, 1, &m_pDepthView);
	m_pContext->PSSetSamplers(0, 1, &m_pSampleState);
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &Stride, &Offset);
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->Draw(6, 0);
	ID3D11ShaderResourceView* tempNULL[1] = { NULL };
	m_pContext->PSSetShaderResources(0, 1, tempNULL);

	//m_pContext->PSSetSamplers(0, 1, NULL);
	m_pContext->VSSetShader(NULL, NULL, 0);
	m_pContext->PSSetShader(NULL, NULL, 0);
	m_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
	m_pContext->OMSetDepthStencilState(oldState, temp);
	if ( oldState )
		oldState->Release();
	//g_pDepthTextureVar->SetResource(NULL);
	//g_pRenderCameraZ->GetPassByIndex(0)->Apply(0);
	return false;
}
