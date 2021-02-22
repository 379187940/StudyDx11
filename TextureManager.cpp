#include "TextureManager.h"



CTextureManager::CTextureManager()
{
}


CTextureManager::~CTextureManager()
{
	std::map<string, ID3D11ShaderResourceView*>::iterator it;
	for (it = m_name_texture.begin() ; it != m_name_texture.end() ;it++ )
	{
		it->second->Release();
	}
	m_name_texture.clear();
}
ID3D11ShaderResourceView* CTextureManager::GetTexture2D(string path)
{
	USES_CONVERSION;
	std::map<string, ID3D11ShaderResourceView*>::iterator it;
	it = m_name_texture.find(path);
	if (it != m_name_texture.end())
		return it->second;
	ID3D11ShaderResourceView* pResourceView = nullptr;
	CreateDDSTextureFromFile(AfxGetDevice(), A2W(path.c_str()),&pResourceView);
	assert(pResourceView);
	return pResourceView;
}
CTextureManager* AfxGetTextureManager()
{
	static CTextureManager g_TextureManager;
	return &g_TextureManager;
}