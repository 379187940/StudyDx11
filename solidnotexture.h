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
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_pixelBuffer;
};