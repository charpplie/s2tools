#include "Renderer.h"
#include "UI.h"

ID3D11Device* Renderer::pd3dDevice = NULL;
ID3D11DeviceContext* Renderer::pd3dDeviceContext = NULL;
IDXGISwapChain* Renderer::pSwapChain = NULL;
ID3D11RenderTargetView* Renderer::pMainRenderTargetView = NULL;
ImVec4 Renderer::clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
ImGuiIO* Renderer::io = NULL;

bool Renderer::CreateDeviceD3D(HWND hWnd)
{
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

	const UINT createDeviceFlags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &featureLevel, &pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void Renderer::CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	if (pBackBuffer != nullptr)
	{
		pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pMainRenderTargetView);
		pBackBuffer->Release();
	}
}

void Renderer::CleanupRenderTarget()
{
	if (pMainRenderTargetView)
	{
		pMainRenderTargetView->Release();
		pMainRenderTargetView = nullptr;
	}
}

void Renderer::CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (pSwapChain)
	{
		pSwapChain->Release();
		pSwapChain = nullptr;
	}

	if (pd3dDeviceContext)
	{
		pd3dDeviceContext->Release();
		pd3dDeviceContext = nullptr;
	}

	if (pd3dDevice)
	{
		pd3dDevice->Release();
		pd3dDevice = nullptr;
	}
}

int Renderer::Init(HWND hWnd)
{
	ImGui_ImplWin32_EnableDpiAwareness();

	if (!CreateDeviceD3D(hWnd))
	{
		CleanupDeviceD3D();

		return -1;
	}

	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO(); (void)io;
	(*io).ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	(*io).ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 4.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	const HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO info = {};
	info.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(monitor, &info);
	const int monitor_height = info.rcMonitor.bottom - info.rcMonitor.top;

	ImGui::GetIO().IniFilename = nullptr;

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);

	return 0;
}

void Renderer::Render()
{

	//if (monitor_height > 1080)
	//{
	//    const float fScale = 2.0f;
	//    ImFontConfig cfg;
	//    cfg.SizePixels = 13 * fScale;
	//    ImGui::GetIO().Fonts->AddFontDefault(&cfg);
	//}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	{
		UI::Draw();
	}

	ImGui::EndFrame();

	ImGui::Render();
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
	pd3dDeviceContext->OMSetRenderTargets(1, &pMainRenderTargetView, nullptr);
	pd3dDeviceContext->ClearRenderTargetView(pMainRenderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	pSwapChain->Present(1, 0);
}

void Renderer::Destroy()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDeviceD3D();
}

void Renderer::WM_SIZE_CALLBACK(WPARAM wParam, LPARAM lParam)
{
	if (pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
	{
		CleanupRenderTarget();
		pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
		CreateRenderTarget();
	}
}