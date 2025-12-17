#pragma once

#ifndef _S2_DRAWING_H__
#define _S2_DRAWING_H__

#include <string_view>
#define S2_GUI
#include "pch.h"

#define EXIT_IMGUI(x, y) \
	lastMsg = # x; \
	ImGui::End(); \
	goto y;

#define EXIT_IMGUI_NOSTATUS(x) \
	ImGui::End(); \
	goto x;

class UI
{
private:
	static LPCSTR lpWindowName;
	static ImVec2 vWindowSize;
	static ImGuiWindowFlags WindowFlags;
	static bool show_main_window;
	static bool show_mods_window;
	static bool show_debug_window;

public:
	static bool isActive();
	static void Draw();
	static std::string FileOpenDialog(HWND hWnd);
};

#endif //_S2_DRAWING_H__