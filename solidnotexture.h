#pragma once
class solidnotexture
{
public:
	struct ViewProjMatrix
	{
		float4x4 viewporjMatrix;
	};
	struct solidColor
	{
		float4 color;
	};
	solidnotexture();
	~solidnotexture();
	bool Initialize(ID3D11Device*);
	//void Shutdown();
	bool Render(ID3D11DeviceContext*, float4x4, float4, int);

private:

	bool SetShaderParameters(ID3D11DeviceContext*, float4x4 , float4);
	//void RenderShader(ID3D11DeviceContext*, float4x4 , float4 , int);

private:
	CComPtr<ID3D11VertexShader> m_vertexShader;
	CComPtr<ID3D11PixelShader> m_pixelShader;
	CComPtr<ID3D11InputLayout> m_layout;
	CComPtr<ID3D11Buffer> m_matrixBuffer;
	CComPtr<ID3D11Buffer> m_pixelBuffer;
};