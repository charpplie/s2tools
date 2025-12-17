#pragma once

#ifndef _S2_RENDERER_H__
#define _S2_RENDERER_H__

#define S2_GUI
#include "pch.h"
#include "UI.h"

class Renderer
{
#ifdef _WIN32
private:
	static ID3D11Device* pd3dDevice;
	static ID3D11DeviceContext* pd3dDeviceContext;
	static IDXGISwapChain* pSwapChain;
	static ID3D11RenderTargetView* pMainRenderTargetView;
	static ImVec4 clear_color;
	static ImGuiIO* io;

	static bool CreateDeviceD3D(HWND hWnd);
	static void CleanupDeviceD3D();
	static void CreateRenderTarget();
	static void CleanupRenderTarget();

public:
	static HMODULE hCurrentModule;

	static int Init(HWND hWnd);
	static void Render();
	static void Destroy();
	static void WM_SIZE_CALLBACK(WPARAM wParam, LPARAM lParam);
	//static void WM_DPICHANGED_CALLBACK();
#endif //_WIN32
};

#endif //_S2_RENDERER_H__