#pragma once
#include "unit.h"
class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();
private:
	std::map<string, ID3D10Texture2D*> m_name_texture;

};

