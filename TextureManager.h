#pragma once
#include "unit.h"
#include "DDSTextureLoader.h"
class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();
	ID3D11ShaderResourceView* GetTexture2D(string path);
	void ReleaseTexture2D(ID3D11ShaderResourceView* pTexture2D);
private:
	std::map<string, ID3D11ShaderResourceView*> m_name_texture;
	std::map<ID3D11ShaderResourceView*, string> m_texture_name;
	std::map<ID3D11ShaderResourceView*, int> m_texture_refcount;
};

