#include <random>
#include "unit.h"
#include "Terrain.h"
#include "Common.h"

#include "Material.h"
#include "FreeImage.h"
CTerrain::CTerrain(wstring strName) :
	CBaseRenderObject(strName)
{

}


CTerrain::~CTerrain()
{
}
bool CTerrain::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	Release();
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
	LoadHeightMap("heightmap.bmp");
	InitGeometry();
	return true;
}
bool CTerrain::Release()
{
	m_HeightData.heightData.clear();
	return true;
}
bool CTerrain::LoadHeightMap(char* heightImageFilename)
{
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(heightImageFilename);
	assert(fif != FIF_UNKNOWN);
	FIBITMAP *image = FreeImage_Load(fif, heightImageFilename);
	assert(image);

	FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(image);
	// 获取保存图片的字节数组
	unsigned char *bits1 = FreeImage_GetBits(image);
	uint width = FreeImage_GetWidth(image);
	uint height = FreeImage_GetHeight(image);

	// 获取保存每个像素的字节数 这里为3,分别为RGB
	unsigned int byte_per_pixel = FreeImage_GetLine(image) / width;

	//uint dstSize = sizeof(uint) * width * height;
	m_HeightData.row = height;
	m_HeightData.col = width;
	m_HeightData.heightData.resize(width*height);
	unsigned int srcPitchByte = FreeImage_GetPitch(image);
	//unsigned int dstPitchByte = width * byte_per_pixel;
	for (UINT32 row = 0; row < height; ++row)
	{
		for (UINT32 col = 0; col < width; ++col)//gltf_image->pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
		{
			switch (type)
			{
			case FIC_RGB:
			{
				int index = (height - row - 1)*srcPitchByte + col*byte_per_pixel;
				m_HeightData.heightData[row*width + col] = *(bits1 + index);
				break;
			}
			default:
				assert(0);
				break;
			}
		}
	}
	FreeImage_Unload(image);
	return true;
}
bool CTerrain::InitGeometry()
{
	int row = m_HeightData.row;
	int col = m_HeightData.col;
	int xTranslation = col / 2 + 1;
	int zTranslation = row / 2 + 1;
	std::default_random_engine e;
	std::uniform_int_distribution<unsigned> u(0, 255); //生成随机颜色
	for ( int i = 0 ; i < row ; i++ )
		for (int j = 0; j < col; j++)
		{
			float3 pos;
			pos.x = j- xTranslation ;
			pos.y = m_HeightData.heightData[i*col + j]* m_heightScale;
			pos.z = i - zTranslation ;
			m_VertexBuffer.push_back(pos);
			float3 color;
			color.r = float(u(e))/255;
			color.g = float(u(e)) / 255;
			color.b = float(u(e)) / 255;
			m_VertexColorBuffer.push_back(color);
		}
	/*
		3|-----------|4
		 |  \        |
		 |    \      |
		 |      \    |
		 |         \ |
		1|-----------|2
		two triangle:
		(1,3,2) （2,3,4）
	*/
	//construct indexbuffer
	for (int i = 0; i < row-1; i++)
		for (int j = 0; j < col-1; j++)
		{
			int index1 = i*col + j;
			int index2 = i*col + j + 1;
			int index3 = (i + 1)*col + j;
			int index4 = (i + 1)*col + j + 1;
			m_indexBuffer.push_back(index1);
			m_indexBuffer.push_back(index3);
			m_indexBuffer.push_back(index2);
			m_indexBuffer.push_back(index2);
			m_indexBuffer.push_back(index3);
			m_indexBuffer.push_back(index4);
		}
	m_pCameraAttBuffer = CreateBuffer(m_pd3dDevice, sizeof(CameraAtrribute), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pCameraAttBuffer);
	m_pVertexBuffer = CreateBuffer(m_pd3dDevice, m_VertexBuffer.size() * sizeof(float3), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pVertexBuffer);
	m_pVertexColorBuffer = CreateBuffer(m_pd3dDevice, m_VertexColorBuffer.size() * sizeof(float3), D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pVertexColorBuffer);
	m_pIndexBuffer = CreateBuffer(m_pd3dDevice, m_indexBuffer.size() * sizeof(int), D3D11_USAGE_DYNAMIC, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	assert(m_pIndexBuffer);

	ID3D10Blob* pVertexShaderBlob = NULL;
	assert(SUCCEEDED(CompileShaderFromFile("terrain.hlsl", NULL, NULL, "vs_main", "vs_4_0", 0, 0, NULL, &pVertexShaderBlob)));
	vector<D3D11_INPUT_ELEMENT_DESC> allDesc;
	allDesc.push_back({"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0});
	allDesc.push_back({"COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,1,0,D3D11_INPUT_PER_VERTEX_DATA,0 });
	m_pLayoutInput = CreateInputLayout(m_pd3dDevice, allDesc, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize());
	m_pd3dDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &m_pVertexShader);
	pVertexShaderBlob->Release();

	ID3D10Blob* pPixelShaderBlob = NULL;
	assert(SUCCEEDED(CompileShaderFromFile("terrain.hlsl", NULL, NULL, "ps_main", "ps_4_0", 0, 0, NULL, &pPixelShaderBlob)));
	m_pd3dDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &m_pPixelShader);
	pPixelShaderBlob->Release();
 
	return true;
}	

void CTerrain::Tick(DWORD dwTimes)
{
}

bool CTerrain::Render(DWORD dwTimes)
{
	UINT stride = sizeof(float3);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);
	m_pContext->IASetVertexBuffers(1, 1, &m_pVertexColorBuffer.p, &stride, &offset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT,0);
	m_pContext->VSSetShader(m_pVertexShader, nullptr, 0);
	m_pContext->PSSetShader(m_pPixelShader, nullptr, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &m_pCameraAttBuffer.p);
	m_pContext->IASetInputLayout(m_pLayoutInput);
	m_pContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pContext->DrawIndexed(m_indexBuffer.size(), 0, 0);
	return false;
}
bool CTerrain::UpdateRenderParams(const RenderParams& renderParams)
{
	
	CameraAtrribute cameraAttribute;
	cameraAttribute.viewproj = renderParams.m_worldMatrix * renderParams.m_viewMatrix * renderParams.m_projMatrix;
	cameraAttribute.viewproj = cameraAttribute.viewproj.Transpose();
	//cameraAttribute.mView = renderParams.m_viewMatrix.Transpose();
	//cameraAttribute.mProjection = renderParams.m_projMatrix.Transpose();
	UpdateBufferData(m_pContext, m_pCameraAttBuffer, &cameraAttribute, sizeof(cameraAttribute));
	UpdateBufferData(m_pContext, m_pVertexBuffer, m_VertexBuffer.data(), m_VertexBuffer.size() * sizeof(float3));
	UpdateBufferData(m_pContext, m_pVertexColorBuffer, m_VertexColorBuffer.data(), m_VertexColorBuffer.size() * sizeof(float3));
	UpdateBufferData(m_pContext, m_pIndexBuffer, m_indexBuffer.data(), m_indexBuffer.size() * sizeof(int));
	return true;
}