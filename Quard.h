#pragma once
#include "IRenderObject.h"
#include <xnamath.h>
#include "BaseRenderObject.h"
class CQuard :
	public CBaseRenderObject, public CCommonRenderData
{
	struct SpriteVertex
	{
		XMFLOAT3 v3Pos;
		XMFLOAT2 v2TexCoord;
	};
	struct CBufferConvertToWorld
	{
		float g_fQ;                         // far / (far - near)
		float g_fQTimesZNear;               // Q * near
	};
public:
	CQuard(wstring strName);
	virtual ~CQuard();
	virtual void Tick(DWORD dwTimes) override;
	virtual bool Render(DWORD dwTimes) override;
	virtual bool UpdateRenderParams(const RenderParams& renderParams) { return true; }
	bool Init(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pContext);
	void SetDepthTexture(ID3D11ShaderResourceView* pTemp) { m_pDepthTextureSRV = pTemp; }
private:
	ID3D11ShaderResourceView*    m_pDepthTextureSRV = NULL;
	ID3D11DepthStencilState*     m_pDepthState = NULL;
	ID3D11SamplerState*          m_pSampleState = NULL;
	ID3D11Texture2D* m_pDepthTexutre = NULL;
	//通过depthtexture建立的shaderresource不能通过cpu直接访问，所以需要一个中间层进行读取
	ID3D11Texture2D* m_pDepthTexutreTemp = NULL;
	ID3D11ShaderResourceView* m_pDepthView = NULL;
	ID3D11Buffer*              m_pConvertDepthToWorld = NULL;
};
