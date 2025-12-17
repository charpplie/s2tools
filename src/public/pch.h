#pragma once

#ifndef _S2_PCH_H__
#define _S2_PCH_H__

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_
#define NOMINMAX

#include <Windows.h>
#include <tchar.h>

#ifdef S2_GUI
#include <d3d11.h>
#include <d3d11.h>
#include "../thirdparty/imgui/include/imgui.h"
#include "../thirdparty/imgui/include/imgui_impl_dx11.h"
#include "../thirdparty/imgui/include/imgui_impl_win32.h"
#endif //S2_GUI

#endif //_WIN32

#endif //_S2_PCH_H__
