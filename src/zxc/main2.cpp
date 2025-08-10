#include <print>
#include <string>
#include <string_view>
#include <format>
#include <vector>

#include <steampp/steampp.h>
#include <vpkpp/format/VPK.h>

#pragma comment(lib, "d3d11.lib")

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <ShObjIdl_core.h>

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool                     g_SwapChainOccluded = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool isCollapsed = false;

#define WIDTH 800
#define HEIGHT 640

#define TESTHASH "ede785e54fba84706f7306ac3861646e132e06895bd21ed16909d49fcb275c27"

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int x = (screenWidth - WIDTH) / 2;
	int y = (screenHeight - HEIGHT) / 2;

	WNDCLASSEXW wc = {
		sizeof(wc),
		CS_CLASSDC,
		WndProc,
		0L,
		0L,
		GetModuleHandle(nullptr),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		L"S2ToolsClass",
		nullptr
	};

	::RegisterClassExW(&wc);

	HWND hWnd = ::CreateWindowExW(
		0,
		wc.lpszClassName,
		L"",
		WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
		x,
		y,
		//(int)(1280 * main_scale),
		//(int)(800 * main_scale),
		WIDTH,
		HEIGHT,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	if (!CreateDeviceD3D(hWnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	::ShowWindow(hWnd, SW_HIDE);
	::UpdateWindow(hWnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);
	style.FontScaleDpi = main_scale;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	std::string vpkPath = "Select vpk file";

	bool show_randomizer_window = false;
	bool show_mods_window = false;
	bool show_main_window = true;
	bool is_first_launch = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	HRESULT hr = CoInitializeEx(
		nullptr,
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
	);

	IFileOpenDialog* pfd = nullptr;

	COMDLG_FILTERSPEC vpkSpec[] =
	{
		{L"Valve PacK (*.vpk)", L"*.vpk"}
	};

	bool done = false;
	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
		}

		if (done)
		{
			break;
		}

		// Handle window being minimized or screen locked
		if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
		{
			::Sleep(10);
			continue;
		}
		g_SwapChainOccluded = false;

		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		//ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT), ImGuiCond_Once);
		//ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
			//ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			//static float f = 0.0f;
			//static int counter = 0;
			//static bool dragging = false;
			//static ImVec2 drag_offset;
			//static ImVec2 last_size = ImVec2(-1, -1);

			ImGui::Begin("Source 2 Tools", &show_main_window);

			//if (!show_main_window)
			//{
			//	::PostQuitMessage(0);
			//	return 0;
			//}

			//ImVec2 mouse_pos = ImGui::GetMousePos();
			//ImVec2 window_pos = ImGui::GetWindowPos();

			//if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0))
			//{
			//	dragging = true;
			//	drag_offset = ImVec2(mouse_pos.x - window_pos.x, mouse_pos.y - window_pos.y);
			//}

			//if (dragging)
			//{
			//	if (ImGui::IsMouseDown(0))
			//	{
			//		ImVec2 new_pos = ImVec2(mouse_pos.x - drag_offset.x, mouse_pos.y - drag_offset.y);

			//		::SetWindowPos(hWnd, nullptr, static_cast<int>(new_pos.x), static_cast<int>(new_pos.y), 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			//	}
			//	else
			//	{
			//		dragging = false;
			//	}
			//}

			//ImVec2 size = ImGui::GetWindowSize();

			//if (size.x != last_size.x || size.y != last_size.y)
			//{
			//	last_size = size;

			//	::SetWindowPos(hWnd, nullptr, 0, 0, size.x, size.y, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			//}

			//ImGui::SetWindowCollapsed(isCollapsed, ImGuiCond_Always);

			//if (ImGui::IsWindowCollapsed())
			//{
			//	::ShowWindow(hWnd, SW_MINIMIZE);
			//}

			ImGui::Text(vpkPath.c_str());

			if (ImGui::Button("Change"))
			{
				HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

				pfd->SetFileTypes(ARRAYSIZE(vpkSpec), vpkSpec);
				hr = pfd->Show(hWnd);

				if (SUCCEEDED(hr))
				{
					IShellItem* psiResult = nullptr;
					hr = pfd->GetResult(&psiResult);

					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath = nullptr;
						hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr))
						{
							size_t convertedChars = 0;
							size_t origsize = wcslen(pszFilePath) + 1;
							const size_t newsize = origsize * 2;
							char* nstring = new char[newsize];
							wcstombs_s(&convertedChars, nstring, newsize, pszFilePath, _TRUNCATE);

							vpkPath = std::format("VPK Path:\n{}", nstring);

							delete[] nstring;
							CoTaskMemFree(pszFilePath);
						}
						psiResult->Release();
					}
				}
			}

			ImGui::Checkbox("Randomizer", &show_randomizer_window);
			ImGui::Checkbox("Mods", &show_mods_window);

			//	//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			//	//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			//	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			//		//counter++;
			//	//ImGui::SameLine();
			//	//ImGui::Text("counter = %d", counter);

			//	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}

		if (show_randomizer_window)
		{
			ImGui::Begin("Randomizer", &show_randomizer_window);
			ImGui::Text("randomizes some shit for ya");
			if (ImGui::Button("CLOSE ME YOU FUCKING IDIOT"))
			{
				show_randomizer_window = false;
			}
			ImGui::End();
		}

		if (show_mods_window)
		{
			ImGui::Begin("Mods", &show_mods_window);

			if (ImGui::Button("Verify Mods"))
			{

			}

			if (ImGui::Button("Install Mods"))
			{

			}

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		//g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		// Present
		HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
		//HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
		g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hWnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	return 0;
}

bool CreateDeviceD3D(HWND hWnd)
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		//if (wParam == SIZE_MINIMIZED)
		//	return 0;
		//g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		//g_ResizeHeight = (UINT)HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
		{
			isCollapsed = true;
		}
		else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
		{
			isCollapsed = false;
		}

		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
		//case WM_ACTIVATE: 
		//	if (LOWORD(wParam) != WA_INACTIVE)
		//	{
		//		isCollapsed = false;
		//	}
		//	else
		//	{
		//		isCollapsed = true;
		//	}
		//	break;

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;

		if ((wParam & 0xfff0) == SC_MINIMIZE)
		{
			isCollapsed = true;
			return 0;
		}

		//if ((wParam & 0xfff0) == SC_RESTORE)
		//{
		//	isCollapsed = false;
		//	return 0;
		//}

		break;

	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

#pragma comment(lib, "d3d11.lib")

#include "pch.h"
#include "ui.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdline, int nCmdShow)
{
	UI::Render();
	return 0;
}