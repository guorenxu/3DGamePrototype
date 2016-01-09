#ifndef _GAME_H
#define _GAME_H

#define D3D_DEBUG_INFO

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <dsound.h>
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "dxgraphics.h"
#include "Maps.h"
#include "Effects.h"
#include "Outlines.h"

#define APPTITLE "Star Engine"

#define FULLSCREEN 0
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 720

void Setup_Mesh();

void File_Parse(int Vertices,
				int Texcoords,
				int Faces,
				ID3DXMesh* Mesh,
				char* Name);

void Game_Loop(HWND hwnd);

void Set_Up();

void Camera_Calc();

void Game_Calc();

void Vertex_Draw();

void PP_Prep();

void PP_Effects();

void Pre_Present();

void Mouse();

void Interface();

void Present();

void Secondary();

int Game_Init(HWND);
void Game_Run(HWND);
void Game_End(HWND);

void Init_PP();
bool Init_Shaders();
bool Init_Effects();

HRESULT ReleaseLostObjects();
HRESULT RestoreLostObjects();

bool CheckLostDevices();

void GetPickingRay(D3DXVECTOR3& originW, D3DXVECTOR3& dirW);

void SetMouseChange(int x, int y);
void SetMouseClick(int button, bool click);
void SetMouseWheel(int wheeldelta);
void SetKeyboard(int key, bool value);

#endif