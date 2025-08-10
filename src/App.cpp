#include <cstdlib>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>

#include <openssl/sha.h>

#include <httplib.h>

#include "App.h"
#include "pch.h"
#include "Renderer.h"
#include "UI.h"

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

HWND App::hWnd = {};
steampp::Steam App::steam;

void App::Start()
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

	hWnd = CreateWindow(
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
		return;
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
}

int App::Download(std::string host, std::string filename, std::string out)
{
	httplib::Client client(host);
	client.set_connection_timeout(10);
	client.set_read_timeout(30);

	std::ofstream ofs(out, std::ios::binary);
	if (!ofs)
	{
		return -1;
	}

	auto res = client.Get(std::format("/{}", filename), [&](const char* data, size_t len)
	{
		ofs.write(data, len);
		return true;
	});

	if (!res)
	{
		return -2;
	}

	return 0;
}

std::string App::GetSha256(const char* path)
{
	std::ifstream ifs(path, std::ios::in | std::ios::binary);

	constexpr const std::size_t bufSize{ 1 << 12 };
	char buffer[bufSize];

	unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };

	SHA256_CTX ctx;
	SHA256_Init(&ctx);

	while (ifs.good())
	{
		ifs.read(buffer, bufSize);
		SHA256_Update(&ctx, buffer, ifs.gcount());
	}

	SHA256_Final(hash, &ctx);
	ifs.close();

	std::ostringstream os;
	os << std::hex << std::setfill('0');

	for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
	{
		os << std::setw(2) << static_cast<unsigned int>(hash[i]);
	}

	return os.str();
}

std::vector<VpkInfo> App::readHashes(const char* path)
{
	std::ifstream ifs(path, std::ios::in);
	std::string str;

	std::vector<VpkInfo> vecVi;

	while (std::getline(ifs, str))
	{
		VpkInfo vi = {};
		const size_t separatorIndex = str.find(' ');
		vi.name = str.substr(0, separatorIndex);
		vi.hash = str.substr(separatorIndex + 1, str.length());
		vecVi.push_back({ vi });
	}

	return vecVi;
}

std::string App::GetAppInstallDir(uint32_t appId)
{
	return steam.getAppInstallDir(appId);
}
