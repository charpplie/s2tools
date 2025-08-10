#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "ws2_32.lib")

#include "App.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	App::Start();

	return 0;
}