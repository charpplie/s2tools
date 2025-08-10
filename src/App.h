#pragma once

#ifndef _S2_APP_H__
#define _S2_APP_H__

#include <vector>
#include <string>

#include <steampp/steampp.h>

#include "pch.h"
#include "Renderer.h"
#include "UI.h"

#define HOST "176.124.204.212"

#define HASHES_FILENAME_TMP "hashes_tmp"
#define HASHES_FILENAME_HOST "csnd/hashes"
#define VPK_TMP_FOLDER "tmp_vpk/"
#define MAKE_VPK_PATH_HOST(x) "csnd/" # x
#define MAKE_VPK_PATH_LOCAL(x) VPK_TMP_FOLDER # x

#define EXIT_IMGUI(x, y) \
	lastMsg = # x; \
	ImGui::End(); \
	goto y;

#define EXIT_IMGUI_NOSTATUS(x) \
	ImGui::End(); \
	goto x;

#define MODS_NOT_VERIFIED 0
#define MODS_NOT_INSTALLED 1
#define MODS_VERIFIED_FRESH 2
#define MODS_VERIFIED_OUTOFDATE 3

struct VpkInfo
{
	std::string name;
	std::string hash;
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class App
{
private:
	static steampp::Steam steam;

public:
	static HWND hWnd;
	static void Start();
	static int Download(std::string host, std::string filename, std::string out);
	static std::string GetSha256(const char* path);
	static std::vector<VpkInfo> readHashes(const char* path);
	static std::string GetAppInstallDir(uint32_t appId);
};

#endif