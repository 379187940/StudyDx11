#pragma once
#include "unit.h"
#include "DDSTextureLoader.h"
class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();
	ID3D11ShaderResourceView* GetTexture2D(string path);
private:
	std::map<string, ID3D11ShaderResourceView*> m_name_texture;
	
};

