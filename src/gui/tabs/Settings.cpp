#include "Settings.h"
#include "appframework.h"
#include "../AppearanceSettings.h"
#include "../TaskManager.h"

#define S2_GUI
#include "pch.h"

namespace
{
	const std::string& GetDotaPath()
	{
		static std::string path = App::GetAppInstallDir(570);
		return path;
	}

	const std::filesystem::path& GetBasePath()
	{
		static std::filesystem::path path = std::filesystem::path(GetDotaPath()) / "game" / "dota_russian";
		return path;
	}
}

namespace Tabs
{
	void SettingsTab::Draw()
	{
		if (ImGui::BeginTabBar("SettingsTabBar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("General"))
			{
				ImGui::PushID("GeneralTab");
				DrawGeneralSettings();
				ImGui::PopID();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Appearance"))
			{
				ImGui::PushID("AppearanceTab");
				DrawAppearanceSettings();
				ImGui::PopID();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void SettingsTab::DrawGeneralSettings()
	{
		ImGui::Text("Game dir: %s", std::filesystem::path(GetDotaPath()).string().c_str());
		ImGui::Text("Mods dir: %s", GetBasePath().string().c_str());
		ImGui::Text("VRC dir: NONE");
		if (ImGui::Button("Change"))
		{
			// TODO: Implement directory picker
		}

		ImGui::Separator();

		bool showDebug = TaskManager::IsDebugWindowVisible();
		if (ImGui::Checkbox("Show debug window", &showDebug))
		{
			TaskManager::SetDebugWindowVisible(showDebug);
		}
	}

	void SettingsTab::DrawAppearanceSettings()
	{
		bool darkTheme = AppearanceSettings::IsDarkTheme();
		if (ImGui::Checkbox("Dark theme", &darkTheme))
		{
			AppearanceSettings::SetDarkTheme(darkTheme);
			if (darkTheme)
				ImGui::StyleColorsDark();
			else
				ImGui::StyleColorsLight();
		}

		ImGui::Text("ImGui border color:");
		ImGui::SameLine();
		ImGui::TextDisabled("(applies to ImGuiCol_Border)");

		bool rgbMode = AppearanceSettings::IsBorderColorEditorRgb();
		if (ImGui::Checkbox("Color editor RGB mode", &rgbMode))
		{
			AppearanceSettings::SetBorderColorEditorRgb(rgbMode);
		}
		ImGui::SameLine();

		ImGuiColorEditFlags flags = AppearanceSettings::IsBorderColorEditorRgb() 
			? ImGuiColorEditFlags_DisplayRGB 
			: ImGuiColorEditFlags_DisplayHSV;
		
		ImVec4 borderColor = AppearanceSettings::GetBorderColor();
		if (ImGui::ColorEdit4("Border color", reinterpret_cast<float*>(&borderColor), flags))
		{
			AppearanceSettings::SetBorderColor(borderColor);
		}

		bool borderAnimated = AppearanceSettings::IsBorderAnimated();
		if (ImGui::Checkbox("Animate border (RGB mode)", &borderAnimated))
		{
			AppearanceSettings::SetBorderAnimated(borderAnimated);
		}

		if (AppearanceSettings::IsBorderAnimated())
		{
			ImGui::Indent();
			
			bool borderFlow = AppearanceSettings::IsBorderFlowing();
			if (ImGui::Checkbox("Flow color along border (perimeter)##BorderFlow", &borderFlow))
			{
				AppearanceSettings::SetBorderFlowing(borderFlow);
				int a = 7;
			}

			if (AppearanceSettings::IsBorderFlowing())
			{
				int segmentWidth = AppearanceSettings::GetBorderSegmentWidth();
				if (ImGui::SliderInt("Border segment width (px)", &segmentWidth, 1, 16))
				{
					AppearanceSettings::SetBorderSegmentWidth(segmentWidth);
				}
			}
			else
			{
				float speed = AppearanceSettings::GetBorderRgbSpeed();
				if (ImGui::SliderFloat("RGB speed", &speed, 0.01f, 2.0f, "%.2f"))
				{
					AppearanceSettings::SetBorderRgbSpeed(speed);
				}
			}
			
			ImGui::Unindent();
		}

		ImGui::Separator();
		ImGui::Text("Progress bar rainbow:");
		
		bool progressRainbow = AppearanceSettings::IsProgressRainbowEnabled();
		if (ImGui::Checkbox("Animate progress rainbow", &progressRainbow))
		{
			AppearanceSettings::SetProgressRainbowEnabled(progressRainbow);
		}

		if (AppearanceSettings::IsProgressRainbowEnabled())
		{
			float progressSpeed = AppearanceSettings::GetProgressRgbSpeed();
			if (ImGui::SliderFloat("Progress RGB speed", &progressSpeed, 0.01f, 2.0f, "%.2f"))
			{
				AppearanceSettings::SetProgressRgbSpeed(progressSpeed);
			}

			int progressSegmentWidth = AppearanceSettings::GetProgressSegmentWidth();
			if (ImGui::SliderInt("Segment width (px)", &progressSegmentWidth, 1, 16))
			{
				AppearanceSettings::SetProgressSegmentWidth(progressSegmentWidth);
			}
		}
	}
}