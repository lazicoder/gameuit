#include "game.h"'
#include <Windows.h>
#include <stdio.h>


#define KEY_DOWN(vk_code) ( (GetAsyncKeyState(vk_code)&0x8000)?1:0 )
#define WN_DESTROY 0x0002 //virtual keycode
#define ARR_DOWN 0x28
#define ARR_UP 0x26
#define ARR_LEFT 0x25
#define ARR_RIGHT 0x27




Game::Game(int scr_Width, int scr_Height, bool isWindow) {
	this->m_Width = scr_Width;
	this->m_Height = scr_Height;
	this->m_IsWindow = isWindow;
	this->m_HandleInstance = 0;
	this->m_HandleWindow = 0;
	this->m_lpDirect3D = 0;
	this->m_lpDirect3DDevice = 0;
	_AspAladdin = NULL;
	this->x = 30;
	this->y = 400;
	this->iswalking = 0;
	this->lookright = 1;
}

Game::~Game()
{
	delete _AspAladdin;
}

bool Game::initHandleWindow()
{
	WNDCLASSEX WndcEx;
	ZeroMemory(&WndcEx, sizeof(WndcEx));
	WndcEx.cbSize = sizeof(WNDCLASSEX);
	WndcEx.hInstance = this->m_HandleInstance;
	WndcEx.lpfnWndProc = (WNDPROC)WndProceduce;
	WndcEx.lpszClassName = "Game";
	if (!RegisterClassEx(&WndcEx))
	{
		return false;
	}
	this->m_HandleWindow = CreateWindow(
		"Game", // �?t trung v?i WndcEx.lpszClassName
		TITLE, // TITLE c?a c?a s? #define TITLE "GAME"
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // style c?a c?a s?
		100, // t?a �? x khi c?a s? xu?t hi?n
		100, // t?a �? y khi c?a s? xu?t hi?n
		this->m_Width, // chi?u d�i c?a s?
		this->m_Height, // chi?u cao c?a s?
		0, // handle window parent c?a c?a s? hi?n t?i. �? l� 0
		0, // handle �?n Menu. �? l� 0
		this->m_HandleInstance, // Handle Instance c?a ?ng d?ng
		0); // gi? nguy�n theo default l� 0
	if (!this->m_HandleWindow) // Ki?m tra t?o th�nh c�ng.
	{
		return false;
	}
	ShowWindow(m_HandleWindow, SW_SHOW); // show c?a s? l�n m�n h?nh
	UpdateWindow(m_HandleWindow); // Update c?a s? theo th?i gian
	return true;
}

bool Game::initDirectX3DEnviroment()
{
	this->m_lpDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_lpDirect3D) {
		return true;
	}
	return false;
}

bool Game::initDirect3DDevice()
{
	D3DPRESENT_PARAMETERS d3dpp; // khai b�o bi?n
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // Kh?i t?o v�ng nh?
	d3dpp.BackBufferCount = 1; // S? l�?ng Back buffer
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Ch?n UNKNOW, DirectX s? t? ch?n 1 �?nh d?ng ph� h?p cho Device
	d3dpp.BackBufferHeight = HEIGHT; // chi?u cao c?u BackBuffer
	d3dpp.BackBufferWidth = WIDTH; // Chi?u d�i c?a BackBuffer
	d3dpp.Windowed = true; // Window ? d?ng c?a s?, kh�ng ph?i full m�n h?nh
	d3dpp.hDeviceWindow = this->m_HandleWindow; // G�n HandleWindow cho Device
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD; // Ph��ng th?c swap c?a Buffer
	m_lpDirect3D->CreateDevice(
		D3DADAPTER_DEFAULT, // Ch?n DEFAULT ADAPTER
		D3DDEVTYPE_HAL, // 
		m_HandleWindow, // Handle Window c?a c?a s?
		D3DCREATE_HARDWARE_VERTEXPROCESSING, // Hard ware vertex processing
		&d3dpp,
		&m_lpDirect3DDevice); // H�m t?o Device
	if (FAILED(m_lpDirect3DDevice))
	{
		MessageBox(NULL, "Failed to create device", "Error", MB_OK);
		return false;
	}

	


	return true;
}


void Game::GameInit()
{
	initHandleWindow();
	initDirectX3DEnviroment();
	initDirect3DDevice();

	_AspAladdin = new CAnimationSprite(m_lpDirect3DDevice);
	_AspAladdin->Init();
	_AspAladdin->Load(SPRITE_TITLE_ALADDIN, SPRITE_TITLE_ALADDIN_XML, D3DCOLOR_XRGB(0, 0, 0));
}

void Game::GameRun()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg)); // T?o m?t Mesage �? l?ng nghe c�c s? ki?n c?a c?a s?
	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) // L?y message v� truy?n v�o bi?n msg �? khai b�o
		{
			if (msg.message == WM_QUIT) // Ki?m tra n?u msg y�u c?u QUIT ?ng d?ng th? ?ng d?ng s? tho�t
				break;
			TranslateMessage(&msg); // x? l? input v� chuy?n v�o msg
			DispatchMessage(&msg); // g?i message �?n h�m WndProceduce �? x? l?
		}
		else
		{
			// Begin Draw something in Game
			m_lpDirect3DDevice->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_XRGB(100, 100, 100), 1.0f, 0); // Clear c?a s? 1 l?n tr�?c khi v? l�n
			
			LPDIRECT3DSURFACE9 back_buffer = NULL;
			LPDIRECT3DSURFACE9 surface = NULL;


			m_lpDirect3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);
			int result =
				m_lpDirect3DDevice->CreateOffscreenPlainSurface(
					100,
					100,				// width 			// height
					D3DFMT_X8R8G8B8,		// format
					D3DPOOL_DEFAULT,		// where? (VRAM or RAM)
					&surface,
					NULL);

			if (m_lpDirect3DDevice->BeginScene())
			{

				/*
				
				RECT rect;
				rect.left = x;
				rect.top = y;
				rect.right = x + 200;
				rect.bottom = y + 200;
				if (KEY_DOWN(ARR_DOWN))
				{
					rect_init = -rect_init;
				}
				if (y == 0)
				{
					x += rect_init;
					rect.right = x + 200;
				}
				if (x == 600)
				{
					y += rect_init;
					rect.bottom = y + 200;
				}

				if (y == 600)
				{
					x -= rect_init;
					rect.right = x + 200;
				}

				if (x == 0)
				{
					y -= rect_init;
					rect.bottom = y + 200;
				}
*/
				/*int r = 255;
				int g = 255;
				int b = 255;
				m_lpDirect3DDevice->ColorFill(surface, NULL, D3DCOLOR_XRGB(r, g, b));*/
				
				if (KEY_DOWN(ARR_DOWN))
				{
					y += 5;
					iswalking = 30;
				}
				if (KEY_DOWN(ARR_UP))
				{
					y -= 5;
					iswalking = 30;
				}
				if (KEY_DOWN(ARR_LEFT))
				{
					x -= 5;
					iswalking = 30;
					lookright = -1;
				}
				if (KEY_DOWN(ARR_RIGHT))
				{
					x += 5;
					iswalking = 30;
					lookright = 1;
				}

				if (!KEY_DOWN(ARR_DOWN) && !KEY_DOWN(ARR_UP) && !KEY_DOWN(ARR_LEFT) && !KEY_DOWN(ARR_RIGHT))
				{
					iswalking = 0;
				}
				
				if (y <= 0)
				{
					y = 0;
				}
				if (x >= 600)
				{
					x = 600;
				}

				if (y >= 600)
				{
					y = 600;
				}

				if (x <= 0)
				{
					x = 0;
				}

				_AspAladdin->Render(iswalking, x, y, 2.5, 50, lookright);  // (animation, left, top, hi�nh to nho� so v��i hi�nh th��t, )



				//m_lpDirect3DDevice->StretchRect(
				//	surface,			// from 
				//	NULL,				// which portion?
				//	back_buffer,			// to 
				//	&rect,				// which portion?
				//	D3DTEXF_NONE);


				// Game Render Here
				m_lpDirect3DDevice->EndScene();
			}
		}

		
			m_lpDirect3DDevice->Present(0, 0, 0, 0); // Th? hi?n t?t c? nh?ng g? �? v? l�n m�n h?nh

			if (KEY_DOWN(VK_ESCAPE))
				PostMessage(m_HandleWindow, WN_DESTROY, 0, 0);

			
	}
}

void Game::GameExit()
{
	if (this->m_lpDirect3D) {
		this->m_lpDirect3D->Release();
		this->m_lpDirect3D = NULL;
	}
	if (this->m_lpDirect3DDevice) {
		this->m_lpDirect3DDevice->Release();
		this->m_lpDirect3DDevice = NULL;
	}
	_AspAladdin->Release();
}


LRESULT CALLBACK Game::WndProceduce(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


