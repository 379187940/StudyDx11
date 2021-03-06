#include "unit.h"
#include "Clinder.h"
#include "Scene.h"


CCylinder::CCylinder(wstring strName ):
	CBaseRenderObject(strName)
{
	m_prevVertextSize = 0;
}


CCylinder::~CCylinder()
{

}
bool CCylinder::Render(DWORD dwTimes)
{
	UINT stride = sizeof(float3);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer.p, &stride, &offset);
	m_pContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_pMaterailSolidColor->Render(m_pContext, m_ViewProj, float4(1.0f, 0.0f, 0.0f, 1.0f), m_index.size());
	return true;
}
bool CCylinder::Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext)
{
	m_pd3dDevice = pd3dDevice;
	m_pContext = pContext;
	m_pMaterailSolidColor.reset(new solidnotexture());
	m_pMaterailSolidColor->Initialize(pd3dDevice);
	int2 windowSize;
	windowSize = AfxGetWindowSize();
	m_BoneName.Initialize(pd3dDevice, pContext, windowSize.x, windowSize.y, 30, false, &AfxGetScene()->GetDefaultFont(), "", 20, 10, 1.0f, 0, 0);
	return true;
}
bool CCylinder::UpdateRenderParams(const RenderParams& renderParams)
{
	m_ViewProj = renderParams.m_worldMatrix * renderParams.m_viewMatrix * renderParams.m_projMatrix;
	return true;
}
void CCylinder::Tick(DWORD dwTimes)
{

}
void CCylinder::UpdateProperty(float3 start, float3 end, uint subdivide, float radius, float4 color)
{
	m_start = start;
	m_end = end;
	m_radius = radius;
	m_color = color;
	m_subdivide = subdivide;
	BuildGeometry();
	int iSize = m_vertexs.size();
	int iIndexSize = m_index.size();
	if (m_prevVertextSize != iSize)//需要重新分配顶点buffer
	{
		m_prevVertextSize = iSize;
		m_pVertexBuffer.Release();
		m_pIndexBuffer.Release();
		m_pVertexBuffer = ::CreateBuffer(m_pd3dDevice, iSize * 3 * sizeof(float), D3D11_USAGE_DYNAMIC
			, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
		
		m_pIndexBuffer = ::CreateBuffer(m_pd3dDevice, iIndexSize * sizeof(uint16_t), D3D11_USAGE_DYNAMIC
			, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, NULL);
	}
	UpdateBufferData(m_pContext, m_pVertexBuffer, m_vertexs.data(), iSize * 3 * sizeof(float));
	UpdateBufferData(m_pContext, m_pIndexBuffer, m_index.data(), iIndexSize* sizeof(uint16_t));

}
void CCylinder::BuildGeometry()
{
	m_vertexs.clear();
	m_index.clear();
	float3 dir = m_end - m_start;
	dir = normalize(dir);
	float3 up(dir.z, dir.x, dir.y);
	float3 right = cross(up, dir);
	up = cross(dir, right);
	float subAngle = 2 * PI / m_subdivide;
	//顶点
	for (int i = 0; i < m_subdivide; i++)
	{
		float3 pos1 = m_start + (up*cos(subAngle*i) + right*sin(subAngle*i)) * m_radius;
		float3 pos2 = m_end + (up*cos(subAngle*i) + right*sin(subAngle*i)) * m_radius;
		m_vertexs.push_back(pos1);
		m_vertexs.push_back(pos2);
	}
	//侧面索引
	for (int i = 0; i < m_subdivide; i++)
	{
		m_index.push_back(i*2);
		m_index.push_back(i*2+1);
		m_index.push_back((i*2+2)%(2*m_subdivide-1) );
		m_index.push_back((i * 2 + 2) % (2 * m_subdivide - 1));
		m_index.push_back(i*2 + 1);
		m_index.push_back((i*2 + 3) % (2 * m_subdivide - 1));
	}
	m_vertexs.push_back(m_start);
	uint16_t startIndex = m_vertexs.size() - 1;
	m_vertexs.push_back(m_end);
	uint16_t endIndex = m_vertexs.size() - 1;
	//底面索引   上底 下底
	for (int i = 0; i < m_subdivide; i++)
	{
		m_index.push_back(startIndex);
		m_index.push_back(i*2);
		m_index.push_back((i * 2 +2)% (2 * m_subdivide - 1));
		m_index.push_back(endIndex);
		m_index.push_back((i * 2 + 3) % (2 * m_subdivide - 1));
		m_index.push_back(i * 2+1);
	}
}
