#include "unit.h"
#include "solidnotexture.h"


//static char* vsFile = "solidnotexture.hlsl";

solidnotexture::solidnotexture()
{
}


solidnotexture::~solidnotexture()
{
	
}
bool solidnotexture::Initialize(ID3D11Device* pDevice )
{
	CString shaderFile;
	assert(GetFullPath("solidnotexture.hlsl", shaderFile));
	HRESULT result;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;
	result = CompileShaderFromFile(shaderFile, NULL, NULL, "vs_main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer);
	assert(SUCCEEDED(result));
	result = CompileShaderFromFile(shaderFile, NULL, NULL, "ps_main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer);
	assert(SUCCEEDED(result));
	result = pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	assert(SUCCEEDED(result));
	result = pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	assert(SUCCEEDED(result));
	D3D11_INPUT_ELEMENT_DESC desc= {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA , 0};
	result = pDevice->CreateInputLayout(&desc, 1, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	assert(SUCCEEDED(result));
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth = sizeof(ViewProjMatrix);
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	result = pDevice->CreateBuffer(&bufferDesc, nullptr, &m_matrixBuffer);
	assert(SUCCEEDED(result));
	bufferDesc.ByteWidth = sizeof(solidColor);
	result = pDevice->CreateBuffer(&bufferDesc, nullptr, &m_pixelBuffer);
	assert(SUCCEEDED(result));
	
	return true;
}
bool solidnotexture::SetShaderParameters(ID3D11DeviceContext* pContext, float4x4 matrixViweProj, float4 solidColor)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	UpdateBufferData(pContext, m_matrixBuffer, &matrixViweProj, sizeof(float4x4));
	UpdateBufferData(pContext, m_pixelBuffer, &solidColor, sizeof(float4));

	pContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer.p);
	pContext->PSSetConstantBuffers(1, 1, &m_pixelBuffer.p);
	return true;
}
bool solidnotexture::Render(ID3D11DeviceContext* pContext , float4x4 matrixViweProj, float4 solidColor , int indexCount)
{
	pContext->VSSetShader(m_vertexShader, nullptr, 0);
	pContext->PSSetShader(m_pixelShader, nullptr, 0);
	matrixViweProj = matrixViweProj.Transpose();
	SetShaderParameters(pContext, matrixViweProj, solidColor);
	pContext->IASetInputLayout(m_layout);
	pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pContext->DrawIndexed(indexCount, 0, 0);
	return true;
}