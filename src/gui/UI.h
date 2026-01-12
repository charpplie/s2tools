#pragma once

#ifndef _S2_DRAWING_H__
#define _S2_DRAWING_H__

#include <string>

#define S2_GUI
#include "pch.h"

class UI
{
private:
	static LPCSTR lpWindowName;
	static ImVec2 vWindowSize;
	static ImGuiWindowFlags WindowFlags;
	static bool show_main_window;

public:
	static bool isActive();
	static void Draw();
	static std::string FileOpenDialog(HWND hWnd);
};

#endif //_S2_DRAWING_H__