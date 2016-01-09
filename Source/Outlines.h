#ifndef _OUTLINES_H
#define _OUTLINES_H

#include "dxgraphics.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <dsound.h>
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "CString"
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

struct OutlineVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 norm;
	D3DXVECTOR3 facenormA;
	D3DXVECTOR3 facenormB;
};

struct MeshVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 norm;
	static const DWORD FVF;
};

class Outlines
{
	public:

		Outlines(ID3DXMesh* mesh, ID3DXBuffer* adj);

		~Outlines();

		bool CreateVertexDeclaration();

		void GetFaceNormal(ID3DXMesh* mesh,
			DWORD faceindex,
			D3DXVECTOR3* facenormal);

		void GetFaceNormals(ID3DXMesh* mesh,
			ID3DXBuffer* adj,
			D3DXVECTOR3* currentfacenormal,
			D3DXVECTOR3 adjfacenormals[3],
			DWORD faceindex);

		void GenerateOutlineVertices(ID3DXMesh* mesh,
			ID3DXBuffer* adj);

		void GenerateOutlineIndices(ID3DXMesh* mesh);

		void Draw();

		IDirect3DVertexBuffer9* vb;
		IDirect3DIndexBuffer9*  ib;
		IDirect3DVertexDeclaration9* vertexdecl;

		UINT VertexNumber;
		UINT FaceNumber;
};

#endif