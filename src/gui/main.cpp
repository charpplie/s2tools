#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "ws2_32.lib")

#define S2_GUI
#include "pch.h"
#include "Renderer.h"
#include "UI.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_SIZE:
		Renderer::WM_SIZE_CALLBACK(wParam, lParam);
		return 0;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
		{
			return 0;
		}

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_DPICHANGED:
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
		{
			const RECT* suggested_rect = (RECT*)lParam;
			SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		_T("S2Tools"),
		NULL
	};

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow(
		wc.lpszClassName,
		_T("Source 2 Tools"),
		WS_OVERLAPPEDWINDOW,
		100, 100, 50, 50,
		NULL,
		NULL,
		wc.hInstance,
		NULL
	);

	if (Renderer::Init(hWnd) != 0)
	{
		UnregisterClass(wc.lpszClassName, wc.hInstance);
		return -1;
	}

	bool done = false;

	while (!done)
	{
		MSG msg = {};

		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				done = true;
			}
		}

		if (GetAsyncKeyState(VK_END) & 1)
		{
			done = true;
		}

		if (done)
		{
			break;
		}

		Renderer::Render();

		if (!UI::isActive())
		{
			break;
		}
	}

	Renderer::Destroy();

	DestroyWindow(hWnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	return 0;
}