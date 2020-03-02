//--------------------------------------------------------------------------------------
// File: MeshLoader10.h
//
// Wrapper class for ID3DX10Mesh interface. Handles loading mesh data from an .obj file
// and resource management for material textures.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#ifndef _MESHLOADER11_H_
#define _MESHLOADER11_H_
#pragma once

#define ERROR_RESOURCE_VALUE 1
//#include <xnamath.h>
#include <D3D11.h>
#include "xnamath.h"
#include <vector>
template<typename TYPE> BOOL IsErrorResource( TYPE data )
{
    if( ( TYPE )ERROR_RESOURCE_VALUE == data )
        return TRUE;
    return FALSE;
}

// Vertex format
struct VERTEX
{
    XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;
};


// Used for a hashtable vertex cache when creating the mesh from a .obj file
struct CacheEntry
{
    UINT index;
    CacheEntry* pNext;
};


// Material properties per mesh subset
struct Material
{
    WCHAR   strName[MAX_PATH];

	XMFLOAT3 vAmbient;
	XMFLOAT3 vDiffuse;
	XMFLOAT3 vSpecular;

    int nShininess;
    float fAlpha;

    bool bSpecular;

    WCHAR   strTexture[MAX_PATH];
    ID3D11ShaderResourceView* pTextureRV10;
    //ID3D11EffectTechnique*  pTechnique;
};


class CMeshLoader11
{
public:
            CMeshLoader11();
            ~CMeshLoader11();

    HRESULT Create( ID3D11Device* pd3dDevice, const WCHAR* strFilename );
    void    Destroy();


    UINT    GetNumMaterials() const
    {
		return m_Materials.size();
    }
    Material* GetMaterial( UINT iMaterial )
    {
        return m_Materials[iMaterial];
    }

    UINT GetNumSubsets()
    {
        return m_NumAttribTableEntries;
    }
    /*Material* GetSubsetMaterial( UINT iSubset )
    {
        return m_Materials.GetAt( m_pAttribTable[iSubset].AttribId );
    }*/

   
    WCHAR* GetMediaDirectory()
    {
        return m_strMediaDir;
    }

private:

    HRESULT LoadGeometryFromOBJ( const WCHAR* strFilename );
    HRESULT LoadMaterialsFromMTL( const WCHAR* strFileName );
    void    InitMaterial( Material* pMaterial );

    DWORD   AddVertex( UINT hash, VERTEX* pVertex );
    void    DeleteCache();

    ID3D11Device* m_pd3dDevice;    // Direct3D Device object associated with this mesh
    

    std::vector<CacheEntry*> m_VertexCache;   // Hashtable cache for locating duplicate vertices
	std::vector<VERTEX> m_Vertices;      // Filled and copied to the vertex buffer
	std::vector<DWORD> m_Indices;       // Filled and copied to the index buffer
	std::vector<DWORD> m_Attributes;    // Filled and copied to the attribute buffer
	std::vector<Material*> m_Materials;     // Holds material properties per subset

    UINT        m_NumAttribTableEntries;
	//D3DX10_ATTRIBUTE_RANGE *m_pAttribTable;

    WCHAR   m_strMediaDir[ MAX_PATH ];               // Directory where the mesh was found
};

#endif // _MESHLOADER_H_

