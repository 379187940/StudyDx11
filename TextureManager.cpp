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
void CTextureManager::ReleaseTexture2D(ID3D11ShaderResourceView* pTexture2D)
{
	std::map<ID3D11ShaderResourceView*, int>::iterator it;
	it = m_texture_refcount.find(pTexture2D);
	assert(it != m_texture_refcount.end());
	int count = it->second;
	count--;
	if (count == 0)
	{
		string name = m_texture_name[pTexture2D];
		m_texture_name.erase(pTexture2D);
		m_name_texture.erase(name);
		m_texture_refcount.erase(pTexture2D);
	}
}
ID3D11ShaderResourceView* CTextureManager::GetTexture2D(string path)
{
	USES_CONVERSION;
	std::map<string, ID3D11ShaderResourceView*>::iterator it;
	it = m_name_texture.find(path);
	if (it != m_name_texture.end())
	{
		m_texture_refcount[it->second]++;
		return it->second;
	}
	ID3D11ShaderResourceView* pResourceView = nullptr;
	CreateDDSTextureFromFile(AfxGetDevice(), A2W(path.c_str()),&pResourceView);
	assert(pResourceView);
	m_name_texture[path] = pResourceView;
	m_texture_name[pResourceView] = path;
	m_texture_refcount[pResourceView] = 1;
	return pResourceView;
}
CTextureManager* AfxGetTextureManager()
{
	static CTextureManager g_TextureManager;
	return &g_TextureManager;
}