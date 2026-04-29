#include <string>
#include <fstream>
#include <istream>
#include <vector>
#include <filesystem>
#include <numeric>
#include <algorithm>
#include <wrl/client.h>
#include <ShObjIdl_core.h>

#include "UI.h"
#include "TaskManager.h"
#include "AppearanceSettings.h"
#include "tabs/Mods.h"
#include "tabs/Randomizer.h"
#include "tabs/Changer.h"
#include "tabs/Tools.h"
#include "tabs/Settings.h"

#define WIDTH 600
#define HEIGHT 400

LPCSTR UI::lpWindowName = "Dotamod Toolkit";
ImVec2 UI::vWindowSize = { WIDTH, HEIGHT };
ImGuiWindowFlags UI::WindowFlags = 0;
bool UI::show_main_window = true;

const COMDLG_FILTERSPEC vpkSpec[1] = { {L"Valve PacK (*.vpk)", L"*.vpk"} };

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

float x = (screenWidth - WIDTH) / 2;
float y = (screenHeight - HEIGHT) / 2;

bool UI::isActive()
{
	return show_main_window == true;
}

void UI::Draw()
{
	if (isActive())
	{
		ImVec4 activeBorder = AppearanceSettings::GetActiveBorderColor();

		ImGui::PushStyleColor(ImGuiCol_Border, activeBorder);
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(activeBorder.x * 0.5f, activeBorder.y * 0.5f, activeBorder.z * 0.5f, activeBorder.w));

		ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Once);
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowPos({ x, y }, ImGuiCond_Once);
		ImGui::Begin(lpWindowName, &show_main_window, WindowFlags);

		if (ImGui::BeginTabBar("MainTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Mods"))
			{
				Tabs::ModsTab::Draw();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Randomizer"))
			{
				Tabs::RandomizerTab::Draw();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Changer"))
			{
				Tabs::ChangerTab::Draw();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Tools"))
			{
				Tabs::ToolsTab::Draw();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Settings"))
			{
				Tabs::SettingsTab::Draw();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		if (TaskManager::IsDebugWindowVisible())
		{
			bool visible = TaskManager::IsDebugWindowVisible();
			ImGui::Begin("Debug", &visible);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
			TaskManager::SetDebugWindowVisible(visible);
		}

		const ImVec2 windowPos = ImGui::GetWindowPos();
		const ImVec2 windowSize = ImGui::GetWindowSize();

		ImGui::End();

		AppearanceSettings::DrawFlowingBorder(ImVec2(0,0), ImVec2(100, 100));

		ImGui::PopStyleColor(2);
	}
}

std::string UI::FileOpenDialog(HWND hWnd)
{
	Microsoft::WRL::ComPtr<IFileOpenDialog> dialog;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(dialog.GetAddressOf()));
	if (FAILED(hr))
	{
		return {};
	}

	dialog->SetFileTypes(ARRAYSIZE(vpkSpec), vpkSpec);
	hr = dialog->Show(hWnd);
	if (FAILED(hr))
	{
		return {};
	}

	Microsoft::WRL::ComPtr<IShellItem> result;
	hr = dialog->GetResult(result.GetAddressOf());
	if (FAILED(hr))
	{
		return {};
	}

	PWSTR pszFilePath = NULL;
	hr = result->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
	if (FAILED(hr) || pszFilePath == NULL)
	{
		return {};
	}

	std::string ret;
	{
		const std::filesystem::path p(pszFilePath);
		const std::u8string u8 = p.u8string();
		ret.assign(u8.begin(), u8.end());
	}

	CoTaskMemFree(pszFilePath);
	return ret;
}