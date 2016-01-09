#ifndef _EFFECTS_H
#define _EFFECTS_H

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

#include "dxgraphics.h"

class Efx
{
	public:

	Efx();												//Constructor sets everything to NULL
	~Efx(){};

	bool Initialize();									//Loads the effects from their .txt's
														//and sets the parameter names for handles

	void GetTechniques();								//Matches all techniques to their handles

	ID3DXEffect* LightingEffect;
	ID3DXEffect* LightingEffectSimple;
	ID3DXEffect* BloomEffect;
	ID3DXEffect* CellEffect;
	ID3DXEffect* OutlineEffect;

	D3DXHANDLE AmbientMtrlSHandle;
	D3DXHANDLE DiffuseMtrlSHandle;
	D3DXHANDLE ViewProjSHandle;
	D3DXHANDLE ViewMatrixSHandle;
	D3DXHANDLE ViewProjMatrixSHandle;
	D3DXHANDLE LightDirSHandle;
	D3DXHANDLE TexSHandle;
	D3DXHANDLE AmbientIntensitySHandle;
	D3DXHANDLE DiffuseIntensitySHandle;
	D3DXHANDLE FogOnSHandle;
	D3DXHANDLE CameraPositionSHandle;

	D3DXHANDLE FogOnMHandle;
	D3DXHANDLE AmbientMtrlMHandle;
	D3DXHANDLE DiffuseMtrlMHandle;
	D3DXHANDLE ViewProjMHandle;
	D3DXHANDLE ViewMatrixMHandle;
	D3DXHANDLE ViewProjMatrixMHandle;
	D3DXHANDLE LightDirMHandle;
	D3DXHANDLE TexAMHandle;
	D3DXHANDLE TexBMHandle;
	D3DXHANDLE TexCMHandle;
	D3DXHANDLE OpacTexBMHandle;
	D3DXHANDLE OpacTexCMHandle;
	D3DXHANDLE CameraPositionMHandle;

	D3DXHANDLE LightTechSHandle;
	D3DXHANDLE LightTechHandle;
	D3DXHANDLE LightTechNoTexturesHandle;

	D3DXHANDLE BloomTextureHandle;
	D3DXHANDLE BloomGameTextureHandle;
	D3DXHANDLE BloomTechHandle;

	D3DXHANDLE ViewProjCHandle;
	D3DXHANDLE ViewMatrixCHandle;
	D3DXHANDLE ViewProjMatrixCHandle;
	D3DXHANDLE LightDirCHandle;
	D3DXHANDLE TexCHandle;
	D3DXHANDLE FogOnCHandle;
	D3DXHANDLE CameraPositionCHandle;
	D3DXHANDLE CellTechHandle;

	D3DXHANDLE OutlineTechHandle;
	D3DXHANDLE ProjOHandle;
	D3DXHANDLE ViewMatrixOHandle;
};

#endif