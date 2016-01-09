#include "Effects.h"

Efx::Efx()
{
	LightingEffect = 0;
	LightingEffectSimple = 0;
	BloomEffect = 0;
	CellEffect = 0;
	OutlineEffect = 0;

	LightTechSHandle = 0;
	LightTechHandle = 0;
	LightTechNoTexturesHandle = 0;
	BloomTechHandle = 0;
	CellTechHandle = 0;

	FogOnMHandle = 0;
	AmbientMtrlMHandle = 0;
	DiffuseMtrlMHandle = 0;
	ViewProjMHandle = 0;
	ViewMatrixMHandle = 0;
	ViewProjMatrixMHandle = 0;
	LightDirMHandle = 0;
	TexAMHandle = 0;
	TexBMHandle = 0;
	TexCMHandle = 0;
	OpacTexBMHandle = 0;
	OpacTexCMHandle = 0;
	CameraPositionMHandle = 0;

	AmbientMtrlSHandle = 0;
	DiffuseMtrlSHandle = 0;
	ViewProjSHandle = 0;
	ViewMatrixSHandle = 0;
	ViewProjMatrixSHandle = 0;
	LightDirSHandle = 0;
	TexSHandle = 0;
	AmbientIntensitySHandle = 0;
	DiffuseIntensitySHandle = 0;
	FogOnSHandle = 0;
	CameraPositionSHandle = 0;

	BloomTextureHandle = 0;
	BloomGameTextureHandle = 0;

	ViewProjCHandle = 0;
	ViewMatrixCHandle = 0;
	ViewProjMatrixCHandle = 0;
	LightDirCHandle = 0;
	TexCHandle = 0;
	FogOnCHandle = 0;
	CameraPositionCHandle = 0;

	ViewMatrixOHandle = 0;
	ProjOHandle = 0;
}

bool Efx::Initialize()
{
	HRESULT hresult = 0;

	ID3DXBuffer* errorBuffer = 0;

	hresult = D3DXCreateEffectFromFile(
		d3ddev,
		"Lighting Effect.txt", 
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&LightingEffect,
		&errorBuffer);


	if( errorBuffer )
	{
		MessageBox(0, (char*)errorBuffer->GetBufferPointer(), 0, 0);
		errorBuffer->Release();
	}

	if(FAILED(hresult))
	{
		MessageBox(0, "D3DXCompileEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	ID3DXBuffer* errorBufferS = 0;

	hresult = D3DXCreateEffectFromFile(
		d3ddev,
		"Lighting Effect Simple.txt", 
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&LightingEffectSimple,
		&errorBufferS);

	if( errorBufferS )
	{
		MessageBox(0, (char*)errorBufferS->GetBufferPointer(), 0, 0);
		errorBufferS->Release();
	}

	if(FAILED(hresult))
	{
		MessageBox(0, "D3DXCompileEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	ID3DXBuffer* errorBufferBloom = 0;

	hresult = D3DXCreateEffectFromFile(
		d3ddev,
		"Bloom Effect.txt", 
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&BloomEffect,
		&errorBufferBloom);

	if( errorBufferBloom )
	{
		MessageBox(0, (char*)errorBufferBloom->GetBufferPointer(), 0, 0);
		errorBufferBloom->Release();
	}

	if(FAILED(hresult))
	{
		MessageBox(0, "D3DXCompileEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	ID3DXBuffer* errorBufferCell = 0;

	hresult = D3DXCreateEffectFromFile(
		d3ddev,
		"Cell Shading Effect.txt", 
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&CellEffect,
		&errorBufferCell);

	if( errorBufferCell )
	{
		MessageBox(0, (char*)errorBufferCell->GetBufferPointer(), 0, 0);
		errorBufferCell->Release();
	}

	if(FAILED(hresult))
	{
		MessageBox(0, "D3DXCompileEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	ID3DXBuffer* errorBufferOutline = 0;

	hresult = D3DXCreateEffectFromFile(
		d3ddev,
		"Outline Effect.txt", 
		NULL,
		NULL,
		D3DXSHADER_DEBUG,
		NULL,
		&OutlineEffect,
		&errorBufferOutline);

	if( errorBufferOutline )
	{
		MessageBox(0, (char*)errorBufferOutline->GetBufferPointer(), 0, 0);
		errorBufferOutline->Release();
	}

	if(FAILED(hresult))
	{
		MessageBox(0, "D3DXCompileEffectFromFile() - FAILED", 0, 0);
		return false;
	}

	ViewProjMHandle = LightingEffect->GetParameterByName(0, "ViewProj");
	ViewMatrixMHandle = LightingEffect->GetParameterByName(0, "ViewMatrix");
	ViewProjMatrixMHandle = LightingEffect->GetParameterByName(0, "ViewProjMatrix");
	AmbientMtrlMHandle = LightingEffect->GetParameterByName(0, "AmbientMtrl");
	DiffuseMtrlMHandle = LightingEffect->GetParameterByName(0, "DiffuseMtrl");
	LightDirMHandle = LightingEffect->GetParameterByName(0, "LightDirection");
	FogOnMHandle = LightingEffect->GetParameterByName(0, "FogOn");

	AmbientIntensitySHandle = LightingEffectSimple->GetParameterByName(0, "AmbientIntensity");
	DiffuseIntensitySHandle = LightingEffectSimple->GetParameterByName(0, "DiffuseIntensity");
	ViewProjSHandle = LightingEffectSimple->GetParameterByName(0, "ViewProj");
	ViewMatrixSHandle = LightingEffectSimple->GetParameterByName(0, "ViewMatrix");
	ViewProjMatrixSHandle = LightingEffectSimple->GetParameterByName(0, "ViewProjMatrix");
	AmbientMtrlSHandle = LightingEffectSimple->GetParameterByName(0, "AmbientMtrl");
	DiffuseMtrlSHandle = LightingEffectSimple->GetParameterByName(0, "DiffuseMtrl");
	LightDirSHandle = LightingEffectSimple->GetParameterByName(0, "LightDirection");
	FogOnSHandle = LightingEffectSimple->GetParameterByName(0, "FogOn");

	TexAMHandle = LightingEffect->GetParameterByName(0, "TexA");
	TexBMHandle = LightingEffect->GetParameterByName(0, "TexB");
	TexCMHandle = LightingEffect->GetParameterByName(0, "TexC");
	OpacTexBMHandle = LightingEffect->GetParameterByName(0, "OpacTexB");
	OpacTexCMHandle = LightingEffect->GetParameterByName(0, "OpacTexC");
	CameraPositionMHandle = LightingEffect->GetParameterByName(0, "CameraPosition");
	CameraPositionSHandle = LightingEffectSimple->GetParameterByName(0, "CameraPosition");

	TexSHandle = LightingEffectSimple->GetParameterByName(0, "Tex");

	BloomGameTextureHandle = BloomEffect->GetParameterByName(0, "GameColor");
	BloomTextureHandle = BloomEffect->GetParameterByName(0, "BloomColor");

	ViewProjCHandle = CellEffect->GetParameterByName(0, "ViewProj");
	ViewMatrixCHandle = CellEffect->GetParameterByName(0, "ViewMatrix");
	ViewProjMatrixCHandle = CellEffect->GetParameterByName(0, "ViewProjMatrix");
	LightDirCHandle = CellEffect->GetParameterByName(0, "LightDirection");
	TexCHandle = CellEffect->GetParameterByName(0, "Tex");
	FogOnCHandle = CellEffect->GetParameterByName(0, "FogOn");
	CameraPositionCHandle = CellEffect->GetParameterByName(0, "CameraPosition");

	ProjOHandle = OutlineEffect->GetParameterByName(0, "Proj");
	ViewMatrixOHandle = OutlineEffect->GetParameterByName(0, "ViewMatrix");

	return true;
}

void Efx::GetTechniques()
{
	LightTechHandle = LightingEffect->GetTechniqueByName("Lighting_Tech");
	LightTechNoTexturesHandle = LightingEffect->GetTechniqueByName("Lighting_Tech_No_Textures");
	LightTechSHandle = LightingEffectSimple->GetTechniqueByName("Lighting_Tech_Simple");
	BloomTechHandle = BloomEffect->GetTechniqueByName("Bloom Effect");
	CellTechHandle = CellEffect->GetTechniqueByName("Cell_Tech");
	OutlineTechHandle = OutlineEffect->GetTechniqueByName("Outline");
}